//handle SPI logic from DRV8323S data sheet
/**
 * @file    SPI_driver.c
 * @brief   DRV8323 Three-Phase Gate Driver Implementation (SPI, STM32 HAL)
 *
 * Usage — quick-start for motor w/ SPI:
 *
 *   DRV8323_Handle_t motor;
 *
 *   // Motor — 6-PWM mode
 *   motor.hspi       = &hspi;
 *   motor.cs_port    = GPIOA;  motor.cs_pin    = GPIO_PIN_4;
 *   motor.en_port    = GPIOB;  motor.en_pin    = GPIO_PIN_0;
 *   motor.nfault_port= GPIOB;  motor.nfault_pin= GPIO_PIN_1;
 *   motor.motor_id   = 1;
 *   DRV8323_DefaultConfig(&motor, DRV8323_MODE_6PWM, DRV8323_DT_100NS);
 *   DRV8323_Init(&motor);
 */

//i named this poorly but this is file that interacts with the DRV8323 driver, main communication protocol is SPI
#include "SPI_driver.h"


/** Assert CS low — DRV8323 is active-low CS */
static inline void cs_assert(DRV8323_Handle_t *hdrv)
{
    HAL_GPIO_WritePin(hdrv->cs_port, hdrv->cs_pin, GPIO_PIN_RESET);
}

/** Deassert CS high */
static inline void cs_deassert(DRV8323_Handle_t *hdrv)
{
    HAL_GPIO_WritePin(hdrv->cs_port, hdrv->cs_pin, GPIO_PIN_SET);
}

/* =========================================================================
 * Low-level SPI transfers
 * ========================================================================= */

/**
 * @brief  Read a single 11-bit register value from the DRV8323.
 *
 * The DRV8323 requires the read frame to be sent first; the register data
 * is returned in the 11 LSBs of the 16-bit response word.
 */
DRV8323_Result_t DRV8323_ReadReg(DRV8323_Handle_t *hdrv,
                                   uint8_t reg, uint16_t *out)
{
    if (!hdrv || !out) return DRV8323_ERR_ARG; //check hdrv (driver handler), write to out

    uint16_t tx = DRV8323_READ_FRAME(reg);
    uint16_t rx = 0;

    cs_assert(hdrv);
    HAL_StatusTypeDef ret = HAL_SPI_TransmitReceive(
        hdrv->hspi,
        (uint8_t *)&tx,
        (uint8_t *)&rx,
        1,          /* 1 x 16-bit word */
        1000);
    cs_deassert(hdrv);

    if (ret != HAL_OK) return DRV8323_ERR_SPI;

    *out = rx & DRV8323_SPI_DATA_MASK;
    return DRV8323_OK;
}

/**
 * @brief  Write an 11-bit value to a writable DRV8323 register.
 *
 * The LOCK field in GATE_HS must be written 0b011 before any other
 * register can be modified; DRV8323_DefaultConfig() handles this.
 */
DRV8323_Result_t DRV8323_WriteReg(DRV8323_Handle_t *hdrv,
                                    uint8_t reg, uint16_t val)
{
    if (!hdrv) return DRV8323_ERR_ARG;

    uint16_t tx = DRV8323_WRITE_FRAME(reg, val);

    cs_assert(hdrv);
    HAL_StatusTypeDef ret = HAL_SPI_Transmit(
        hdrv->hspi,
        (uint8_t *)&tx,
        1,
        1000);
    cs_deassert(hdrv);

    /* Minimum 400 ns CS de-assert time between frames */
    /* HAL_Delay(1) is sufficient at any typical clock rate */
    HAL_Delay(1);

    return (ret == HAL_OK) ? DRV8323_OK : DRV8323_ERR_SPI;
}

/* =========================================================================
 * Configuration
 * ========================================================================= */

/**
 * @brief  Build all five register shadows from safe, validated defaults.
 *
 * Register details:
 *
 *  DRV_CTRL (0x02):
 *    DIS_CPUV = 0  — Charge-pump UV protection enabled
 *    DIS_GDF  = 0  — Gate-drive fault detection enabled
 *    OTW_REP  = 0  — Over-temperature warning NOT reported on nFAULT
 *    PWM_MODE = caller-supplied (6-PWM / 3-PWM / 1-PWM / Independent)
 *    COAST    = 0
 *    BRAKE    = 0
 *    CLR_FLT  = 0
 *
 *  GATE_HS (0x03):
 *    LOCK     = 0b011  — Unlock all writable registers (MANDATORY)
 *    IDRIVEP  = 0x3    — 80 mA peak source current
 *    IDRIVEN  = 0x3    — 120 mA peak sink current
 *
 *  GATE_LS (0x04):
 *    CBC      = 1      — Enable cycle-by-cycle overcurrent limiting
 *    TDRIVE   = 0x2    — 2000 ns peak drive time
 *    IDRIVEP  = 0x3
 *    IDRIVEN  = 0x3
 *
 *  OCP_CTRL (0x05):
 *    TRETRY   = 0      — 4 ms retry time
 *    DEAD_TIME = caller-supplied (default 100 ns → 0b01)
 *    OCP_MODE = 0b01   — Auto-retry on overcurrent
 *    OCP_DEG  = 0b10   — 4 µs deglitch
 *    VDS_LVL  = 0x0    — 0.06 V VDS threshold (lowest, safest)
 *
 *  CSA_CTRL (0x06):
 *    CSA_FET    = 1    — Sense across low-side FET (no shunt required) (TODO: update code since we have shunt resistor)
 *    VREF_DIV   = 1    — VREF = AVDD/2
 *    LS_REF     = 0
 *    CSA_GAIN   = 0b10 — 20 V/V
 *    DIS_SEN    = 0    — Overcurrent sense enabled
 *    CAL_A/B/C  = 0    — No forced offset calibration (use NOCALIB mode)
 *    SEN_LVL    = 0x3  — 1.0 V over-current sense level
 */
void DRV8323_DefaultConfig(DRV8323_Handle_t   *hdrv,
                             DRV8323_PhaseMode_t mode,
                             DRV8323_DeadTime_t  dt)
{
    if (!hdrv) return;

    hdrv->phase_mode = mode;
    hdrv->dead_time  = dt;

    /* ---- DRV_CTRL --------------------------------------------------- */
    hdrv->reg_drv_ctrl =
        (0U << DRV_CTRL_DIS_CPUV) |
        (0U << DRV_CTRL_DIS_GDF)  |
        (0U << DRV_CTRL_OTW_REP)  |
        ((uint16_t)(mode & 0x3U) << DRV_CTRL_PWM_MODE_SHIFT);
    /* COAST, BRAKE, CLR_FLT all remain 0 */

    /* ---- GATE_HS ----------------------------------------------------- */
    /* CRITICAL: LOCK bits must be 0b011 to allow any register writes.    */
    hdrv->reg_gate_hs =
        ((uint16_t)(GATE_HS_LOCK_UNLOCK)   << GATE_HS_LOCK_SHIFT)   |
        ((uint16_t)(GATE_HS_IDRIVEP_DEF)   << GATE_HS_IDRIVEP_SHIFT)|
        ((uint16_t)(GATE_HS_IDRIVEN_DEF)   << GATE_HS_IDRIVEN_SHIFT);

    /* ---- GATE_LS ----------------------------------------------------- */
    hdrv->reg_gate_ls =
        GATE_LS_CBC                                                   |
        ((uint16_t)(GATE_LS_TDRIVE_DEF)    << GATE_LS_TDRIVE_SHIFT)  |
        ((uint16_t)(GATE_LS_IDRIVEP_DEF)   << GATE_LS_IDRIVEP_SHIFT) |
        ((uint16_t)(GATE_LS_IDRIVEN_DEF)   << GATE_LS_IDRIVEN_SHIFT);

    /* ---- OCP_CTRL ---------------------------------------------------- */
    hdrv->reg_ocp_ctrl =
        (0U << OCP_TRETRY_BIT)                                        |
        ((uint16_t)(dt & 0x3U)             << OCP_DEAD_TIME_SHIFT)   |
        ((uint16_t)(OCP_MODE_RETRY)        << OCP_MODE_SHIFT)        |
        ((uint16_t)(OCP_DEG_4US)           << OCP_DEG_SHIFT)         |
        (0x0U << OCP_VDS_LVL_SHIFT);   /* 0.06 V threshold          */

    /* ---- CSA_CTRL ---------------------------------------------------- */
    /*
     * CSA_GAIN = 20 V/V (0b10)
     * board uses external shunt resistors so clear CSA_FET_BIT
     */
    hdrv->reg_csa_ctrl =
        (1U << CSA_FET_BIT)                                           |
        (1U << CSA_VREF_DIV_BIT)                                      |
        (0U << CSA_LS_REF_BIT)                                        |
        ((uint16_t)(CSA_GAIN_20VV)         << CSA_GAIN_SHIFT)        |
        (0U << CSA_DIS_SEN_BIT)                                       |
        (0U << CSA_CAL_A_BIT)                                         |
        (0U << CSA_CAL_B_BIT)                                         |
        (0U << CSA_CAL_C_BIT)                                         |
        (0x3U << CSA_SEN_LVL_SHIFT);   /* 1.0 V OCS level            */
}

/* =========================================================================
 * Initialization
 * ========================================================================= */

/**
 * @brief  Initialize one DRV8323 instance.
 *
 * Steps:
 *   1. Ensure CS is deasserted (idle high).
 *   2. Assert ENABLE — chip wakes from low-power state.
 *   3. Wait >= 1 ms for the chip to power up (datasheet t_ready).
 *   4. Write GATE_HS first (contains the LOCK=0b011 unlock sequence).
 *   5. Write remaining four control registers.
 *   6. Read back FAULT_STAT and VGS_STAT and return error if any fault.
 *
 * @note   DRV8323_DefaultConfig() (or manual shadow setup) must be called
 *         before this function.
 */
DRV8323_Result_t DRV8323_Init(DRV8323_Handle_t *hdrv)
{
    if (!hdrv || !hdrv->hspi) return DRV8323_ERR_ARG;

    DRV8323_Result_t ret;

    /* Step 1: CS idle high */
    cs_deassert(hdrv);

    /* Step 2: Assert enable pin */
    HAL_GPIO_WritePin(hdrv->en_port, hdrv->en_pin, GPIO_PIN_SET);

    /* Step 3: Allow DRV8323 to complete power-on reset */
    HAL_Delay(5);

    /*
     * Step 4: Write GATE_HS first to unlock the register file.
     * The DRV8323 ignores writes to any register until LOCK = 0b011.
     * The TI reference firmware writes GATE_HS twice for safety.
     */
    ret = DRV8323_WriteReg(hdrv, DRV8323_REG_GATE_HS, hdrv->reg_gate_hs);
    if (ret != DRV8323_OK) return ret;
    HAL_Delay(1);
    ret = DRV8323_WriteReg(hdrv, DRV8323_REG_GATE_HS, hdrv->reg_gate_hs);
    if (ret != DRV8323_OK) return ret;
    HAL_Delay(1);

    /* Step 5: Write remaining control registers */
    ret = DRV8323_WriteReg(hdrv, DRV8323_REG_DRV_CTRL,  hdrv->reg_drv_ctrl);
    if (ret != DRV8323_OK) return ret;
    HAL_Delay(1);

    ret = DRV8323_WriteReg(hdrv, DRV8323_REG_GATE_LS,   hdrv->reg_gate_ls);
    if (ret != DRV8323_OK) return ret;
    HAL_Delay(1);

    ret = DRV8323_WriteReg(hdrv, DRV8323_REG_OCP_CTRL,  hdrv->reg_ocp_ctrl);
    if (ret != DRV8323_OK) return ret;
    HAL_Delay(1);

    ret = DRV8323_WriteReg(hdrv, DRV8323_REG_CSA_CTRL,  hdrv->reg_csa_ctrl);
    if (ret != DRV8323_OK) return ret;
    HAL_Delay(1);

    /* Step 6: Read status to confirm no active faults */
    ret = DRV8323_ReadStatus(hdrv);
    if (ret != DRV8323_OK) return ret;

    hdrv->initialized = true;
    return DRV8323_OK;
}

/* =========================================================================
 * Status / Fault Handling
 * ========================================================================= */

/**
 * @brief  Read FAULT_STAT and VGS_STAT registers.
 *
 * FAULT_STAT bits (read-only):
 *   10  FAULT   — Gate or system fault
 *    9  VDS_OCP — VDS overcurrent detected
 *    8  GDF     — Gate drive fault
 *    7  UVLO    — Undervoltage lockout
 *    6  OTSD    — Overtemperature shutdown
 *    5  VDS_HA  — VDS high-side phase A OCP
 *    4  VDS_LA  — VDS low-side  phase A OCP
 *    3  VDS_HB  — ...
 *    2  VDS_LB
 *    1  VDS_HC
 *    0  VDS_LC
 */
DRV8323_Result_t DRV8323_ReadStatus(DRV8323_Handle_t *hdrv)
{
    if (!hdrv) return DRV8323_ERR_ARG;

    DRV8323_Result_t ret;

    ret = DRV8323_ReadReg(hdrv, DRV8323_REG_FAULT_STAT, &hdrv->status_fault);
    if (ret != DRV8323_OK) return ret;

    ret = DRV8323_ReadReg(hdrv, DRV8323_REG_VGS_STAT,   &hdrv->status_vgs);
    if (ret != DRV8323_OK) return ret;

    /* Any non-zero FAULT_STAT means the driver is in a fault condition */
    return (hdrv->status_fault != 0) ? DRV8323_ERR_FAULT : DRV8323_OK;
}

/**
 * @brief  Clear all latched faults.
 *
 * Sets CLR_FLT bit, writes register, then clears the bit so it is not
 * permanently asserted (CLR_FLT auto-clears on the device but we keep
 * the shadow consistent).
 */
DRV8323_Result_t DRV8323_ClearFaults(DRV8323_Handle_t *hdrv)
{
    if (!hdrv) return DRV8323_ERR_ARG;

    DRV8323_Result_t ret;

    /* Set CLR_FLT */
    hdrv->reg_drv_ctrl |= DRV_CTRL_CLR_FLT;
    ret = DRV8323_WriteReg(hdrv, DRV8323_REG_DRV_CTRL, hdrv->reg_drv_ctrl);
    if (ret != DRV8323_OK) return ret;

    HAL_Delay(1);

    /* Clear CLR_FLT in shadow (bit is self-clearing on device) */
    hdrv->reg_drv_ctrl &= ~DRV_CTRL_CLR_FLT;
    ret = DRV8323_WriteReg(hdrv, DRV8323_REG_DRV_CTRL, hdrv->reg_drv_ctrl);

    return ret;
}

/* =========================================================================
 * Enable / Disable
 * ========================================================================= */

void DRV8323_Enable(DRV8323_Handle_t *hdrv)
{
    if (!hdrv) return;
    HAL_GPIO_WritePin(hdrv->en_port, hdrv->en_pin, GPIO_PIN_SET);
}

void DRV8323_Disable(DRV8323_Handle_t *hdrv)
{
    if (!hdrv) return;
    HAL_GPIO_WritePin(hdrv->en_port, hdrv->en_pin, GPIO_PIN_RESET);
}

/* =========================================================================
 * Runtime Updates
 * ========================================================================= */

/**
 * @brief  Change the PWM phase commutation mode.
 *
 * Phase mode determines which combination of PWM inputs drives the
 * three half-bridges:
 *
 *   6-PWM  (DRV8323_MODE_6PWM):
 *     All six gate signals driven independently. Best for FOC / sinusoidal
 *     control where software generates all gate-high and gate-low signals.
 *     Dead-time MUST be handled in firmware (timer complementary outputs)
 *     OR by the DRV8323 hardware dead-time insertion (configured here).
 *
 *   3-PWM  (DRV8323_MODE_3PWM):
 *     Three inputs, each controlling one complementary half-bridge pair.
 *     Dead-time insertion handled automatically by DRV8323 hardware.
 *     Simpler firmware, suitable for trapezoidal commutation.
 *
 *   1-PWM  (DRV8323_MODE_1PWM):
 *     Single PWM speed input + DIR direction pin. Lowest complexity.
 *     The DRV8323 generates commutation internally (1PWM_COM / 1PWM_DIR
 *     bits select commutation behaviour).
 *
 *   Independent (DRV8323_MODE_INDEP):
 *     Like 6-PWM but without automatic recirculation diode mapping.
 *     Used in advanced inverter designs.
 */
DRV8323_Result_t DRV8323_SetPhaseMode(DRV8323_Handle_t   *hdrv,
                                        DRV8323_PhaseMode_t mode)
{
    if (!hdrv) return DRV8323_ERR_ARG;

    hdrv->phase_mode = mode;

    /* Clear old mode bits, insert new value */
    hdrv->reg_drv_ctrl &= ~((uint16_t)(0x3U << DRV_CTRL_PWM_MODE_SHIFT));
    hdrv->reg_drv_ctrl |=  ((uint16_t)((mode & 0x3U) << DRV_CTRL_PWM_MODE_SHIFT));

    return DRV8323_WriteReg(hdrv, DRV8323_REG_DRV_CTRL, hdrv->reg_drv_ctrl);
}

/**
 * @brief  Update hardware dead-time insertion.
 *
 * Dead time is inserted between the falling edge of one gate signal and
 * the rising edge of the complementary gate signal to prevent shoot-through.
 *
 * Choose based on your MOSFET gate charge and gate driver current:
 *   50 ns  — fast-switching FETs (Qg < 10 nC), high-frequency PWM
 *   100 ns — general-purpose FETs, 20–50 kHz PWM  (default here)
 *   200 ns — larger FETs or higher gate impedance
 *   400 ns — power modules, paralleled FETs
 *
 * Note: In 6-PWM mode the STM32 timer complementary output dead-time AND
 * this hardware dead-time both apply. Set only one
 */
DRV8323_Result_t DRV8323_SetDeadTime(DRV8323_Handle_t  *hdrv,
                                       DRV8323_DeadTime_t dt)
{
    if (!hdrv) return DRV8323_ERR_ARG;

    hdrv->dead_time = dt;

    /* Clear old dead-time bits, insert new value */
    hdrv->reg_ocp_ctrl &= ~((uint16_t)(0x3U << OCP_DEAD_TIME_SHIFT));
    hdrv->reg_ocp_ctrl |=  ((uint16_t)((dt & 0x3U) << OCP_DEAD_TIME_SHIFT));

    return DRV8323_WriteReg(hdrv, DRV8323_REG_OCP_CTRL, hdrv->reg_ocp_ctrl);
}

/* =========================================================================
 * Fault Pin
 * ========================================================================= */

/**
 * @brief  Sample nFAULT GPIO (active LOW).
 *
 * @return true if a fault is present (pin driven LOW by DRV8323).
 *         false if no fault, or if nfault_port was set to NULL.
 */
bool DRV8323_IsFaulted(DRV8323_Handle_t *hdrv)
{
    if (!hdrv || !hdrv->nfault_port) return false;
    return (HAL_GPIO_ReadPin(hdrv->nfault_port, hdrv->nfault_pin) == GPIO_PIN_RESET);
}

void Motor_InitAll(void)
{
      // --- Motor 0: 6-PWM for FOC, 100 ns dead-time ---
      g_motor[0].hspi        = &hspi1;
      g_motor[0].cs_port     = GPIOA;  
      g_motor[0].cs_pin     = GPIO_PIN_4;

    // Not sure about enable and fault signals. In SPI research I did there was only CS, clk, mosi, and miso
    // https://www.analog.com/en/resources/analog-dialogue/articles/introduction-to-spi-interface.html
    // https://controllerstech.com/how-to-use-spi-with-stm32/ 
      //g_motor[0].en_port     = GPIOB;  
      //g_motor[0].en_pin     = GPIO_PIN_0;
      //g_motor[0].nfault_port = GPIOB;  
      //g_motor[0].nfault_pin = GPIO_PIN_1;
      //g_motor[0].motor_id    = 0;
      DRV8323_DefaultConfig(&g_motor[0], DRV8323_MODE_6PWM, DRV8323_DT_100NS);
      DRV8323_Init(&g_motor[0]);
}

/* =========================================================================
 * Example: application-level multi-motor setup (place in main.c or
 * a motor_controller.c file — shown here as reference only).
 *
 * #ifdef EXAMPLE_USAGE
 *
 * extern SPI_HandleTypeDef hspi1;
 *
 * DRV8323_Handle_t g_motor[3];
 *
 * void Motor_InitAll(void)
 * {
 *     // --- Motor 0: 6-PWM for FOC, 100 ns dead-time ---
 *     g_motor[0].hspi        = &hspi1;
 *     g_motor[0].cs_port     = GPIOA;  g_motor[0].cs_pin     = GPIO_PIN_4;
 *     g_motor[0].en_port     = GPIOB;  g_motor[0].en_pin     = GPIO_PIN_0;
 *     g_motor[0].nfault_port = GPIOB;  g_motor[0].nfault_pin = GPIO_PIN_1;
 *     g_motor[0].motor_id    = 0;
 *     DRV8323_DefaultConfig(&g_motor[0], DRV8323_MODE_6PWM, DRV8323_DT_100NS);
 *     DRV8323_Init(&g_motor[0]);
 *
 *     // --- Motor 1: 3-PWM for trapezoidal BLDC ---
 *     g_motor[1].hspi        = &hspi1;
 *     g_motor[1].cs_port     = GPIOA;  g_motor[1].cs_pin     = GPIO_PIN_5;
 *     g_motor[1].en_port     = GPIOB;  g_motor[1].en_pin     = GPIO_PIN_2;
 *     g_motor[1].nfault_port = NULL;
 *     g_motor[1].motor_id    = 1;
 *     DRV8323_DefaultConfig(&g_motor[1], DRV8323_MODE_3PWM, DRV8323_DT_100NS);
 *     DRV8323_Init(&g_motor[1]);
 *
 *     // --- Motor 2: 1-PWM simple speed control ---
 *     g_motor[2].hspi        = &hspi1;
 *     g_motor[2].cs_port     = GPIOA;  g_motor[2].cs_pin     = GPIO_PIN_6;
 *     g_motor[2].en_port     = GPIOB;  g_motor[2].en_pin     = GPIO_PIN_3;
 *     g_motor[2].nfault_port = GPIOB;  g_motor[2].nfault_pin = GPIO_PIN_4;
 *     g_motor[2].motor_id    = 2;
 *     DRV8323_DefaultConfig(&g_motor[2], DRV8323_MODE_1PWM, DRV8323_DT_200NS);
 *     DRV8323_Init(&g_motor[2]);
 * }
 *
 * void Motor_FaultCheck(void)
 * {
 *     for (int i = 0; i < 3; i++) {
 *         if (DRV8323_IsFaulted(&g_motor[i])) {
 *             DRV8323_ReadStatus(&g_motor[i]);
 *             // Log g_motor[i].status_fault ...
 *             DRV8323_ClearFaults(&g_motor[i]);
 *         }
 *     }
 * }
 * #endif // EXAMPLE_USAGE
 * ========================================================================= */
