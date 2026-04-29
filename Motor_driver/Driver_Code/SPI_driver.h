/**
 * @file    SPI_driver.h
 * @brief   DRV8323 Three-Phase Gate Driver Interface (SPI, STM32 HAL)
 *
 *
 * Current configuration in this driver:
 *   CSA gain   = 20 V/V   (CSA_GAIN[1:0] = 0b10)
 *   Dead time  = 100 ns   (DEAD_TIME[1:0] = 0b01 in OCP_CTRL)
 *   Phase mode = 6-PWM    (default, changeable per instance at runtime)
 *
 * SPI frame format (16-bit, MSB first, CPOL=0 CPHA=1 → Mode 1):
 *   Bit 15     : R/W  — 1 = read, 0 = write
 *   Bits 14:11 : ADDR — 3-bit register address (bit 14 always 0)
 *   Bits 10:0  : DATA — 11-bit payload
 *
 * STM32CubeIDE SPI settings:
 *   Mode       : Full-Duplex Master
 *   Data Size  : 16 bits
 *   First Bit  : MSB
 *   CPOL       : Low (0)
 *   CPHA       : 2 Edge (1)
 *   NSS        : Software (manual GPIO)
 *   Baud Rate  : <= 10 MHz //check on this later...
 */

#ifndef DRV8323_H
#define DRV8323_H

#ifdef __cplusplus
extern "C" {
#endif

//#include "stm32f474RETX_hal.h" //make sure to change this to whatever MCU is on the board
#include <stdint.h>
#include <stdbool.h>

/* =========================================================================
 * Register Addresses
 * ========================================================================= */
#define DRV8323_REG_FAULT_STAT   0x00U  /* Read-only  */
#define DRV8323_REG_VGS_STAT     0x01U  /* Read-only  */
#define DRV8323_REG_DRV_CTRL     0x02U  /* Read/Write */
#define DRV8323_REG_GATE_HS      0x03U  /* Read/Write */
#define DRV8323_REG_GATE_LS      0x04U  /* Read/Write */
#define DRV8323_REG_OCP_CTRL     0x05U  /* Read/Write */
#define DRV8323_REG_CSA_CTRL     0x06U  /* Read/Write */

/* =========================================================================
 * SPI Frame Macros
 * ========================================================================= */
#define DRV8323_SPI_RW_BIT       (1U << 15)
#define DRV8323_SPI_ADDR_SHIFT   11U
#define DRV8323_SPI_DATA_MASK    0x07FFU

#define DRV8323_WRITE_FRAME(addr, data) \
    ((uint16_t)(((uint16_t)((addr) & 0x07U) << DRV8323_SPI_ADDR_SHIFT) | \
                ((uint16_t)((data) & DRV8323_SPI_DATA_MASK))))

#define DRV8323_READ_FRAME(addr) \
    ((uint16_t)(DRV8323_SPI_RW_BIT | \
                ((uint16_t)((addr) & 0x07U) << DRV8323_SPI_ADDR_SHIFT)))

/* =========================================================================
 * DRV_CTRL Register (0x02) Bits
 * ========================================================================= */
#define DRV_CTRL_DIS_CPUV        (1U << 9)  /* Disable charge-pump UV clamp  */
#define DRV_CTRL_DIS_GDF         (1U << 8)  /* Disable gate-drive fault       */
#define DRV_CTRL_OTW_REP         (1U << 7)  /* OTW reported on nFAULT         */
#define DRV_CTRL_PWM_MODE_SHIFT   5U
#define DRV_CTRL_1PWM_COM        (1U << 4)
#define DRV_CTRL_1PWM_DIR        (1U << 3)
#define DRV_CTRL_COAST           (1U << 2)  /* High-Z all outputs             */
#define DRV_CTRL_BRAKE           (1U << 1)  /* Low-side brake                 */
#define DRV_CTRL_CLR_FLT         (1U << 0)  /* Clear latched faults           */

/* PWM_MODE[1:0] — bits 6:5 */
#define DRV_PWM_MODE_6X          0x0U  /* Independent HS/LS gate signals (this would use the phases.c file) */
#define DRV_PWM_MODE_3X          0x1U  /* 3 complementary pairs          (what Adarsh is using... might switch to this) */
#define DRV_PWM_MODE_1X          0x2U  /* Single PWM + DIR               (supposedly works all on its own....) */
#define DRV_PWM_MODE_INDEP       0x3U  /* No automatic recirculation     (no clue what this is) */

/* =========================================================================
 * GATE_HS Register (0x03) Bits
 * ========================================================================= */
#define GATE_HS_LOCK_SHIFT        8U
#define GATE_HS_LOCK_UNLOCK       0x3U   /* Must be 0b011 to unlock registers */
#define GATE_HS_IDRIVEP_SHIFT     4U     /* Peak source current selection     */
#define GATE_HS_IDRIVEN_SHIFT     0U     /* Peak sink current selection      (talk to hardware about this for mosfets) */
/* Default drive strength — 80 mA source, 120 mA sink (prob wont need this much)*/
#define GATE_HS_IDRIVEP_DEF       0x3U
#define GATE_HS_IDRIVEN_DEF       0x3U

/* =========================================================================
 * GATE_LS Register (0x04) Bits
 * ========================================================================= */
#define GATE_LS_CBC               (1U << 10)  /* Cycle-by-cycle current limit */
#define GATE_LS_TDRIVE_SHIFT       8U
#define GATE_LS_IDRIVEP_SHIFT      4U
#define GATE_LS_IDRIVEN_SHIFT      0U
#define GATE_LS_TDRIVE_DEF         0x2U  /* 2000 ns peak drive window        */
#define GATE_LS_IDRIVEP_DEF        0x3U
#define GATE_LS_IDRIVEN_DEF        0x3U

/* =========================================================================
 * OCP_CTRL Register (0x05) Bits
 * ========================================================================= */
#define OCP_TRETRY_BIT            10U   /* 0 = 4 ms retry, 1 = 50 us retry  */
#define OCP_DEAD_TIME_SHIFT        8U   /* bits 9:8                          */
#define OCP_MODE_SHIFT             6U   /* bits 7:6                          */
#define OCP_DEG_SHIFT              4U   /* bits 5:4 deglitch time            */
#define OCP_VDS_LVL_SHIFT          0U   /* bits 3:0 threshold                */

/* DEAD_TIME options (for 6x choose EITHER STM timer deadtime or driver deadtime) */
#define DRV_DEAD_TIME_50NS         0x0U
#define DRV_DEAD_TIME_100NS        0x1U  /* <- Default */
#define DRV_DEAD_TIME_200NS        0x2U
#define DRV_DEAD_TIME_400NS        0x3U

/* OCP_MODE options */
#define OCP_MODE_LATCH             0x0U //prob use this to be safe?
#define OCP_MODE_RETRY             0x1U
#define OCP_MODE_REPORT_ONLY       0x2U
#define OCP_MODE_DISABLED          0x3U

/* OCP deglitch (amount of time to check for current spikes before latching)*/
#define OCP_DEG_1US                0x0U
#define OCP_DEG_2US                0x1U
#define OCP_DEG_4US                0x2U
#define OCP_DEG_8US                0x3U

/* =========================================================================
 * CSA_CTRL Register (0x06) Bits
 * ========================================================================= */
#define CSA_FET_BIT               10U   /* 1 = sense across FET, 0 = shunt  (check with hardware)*/
#define CSA_VREF_DIV_BIT           9U   /* 1 = VREF = AVDD/2                */
#define CSA_LS_REF_BIT             8U
#define CSA_GAIN_SHIFT             6U   /* CSA_GAIN[1:0]                     */
#define CSA_DIS_SEN_BIT            5U
#define CSA_CAL_A_BIT              4U   /* Enable calibration phase A        */
#define CSA_CAL_B_BIT              3U
#define CSA_CAL_C_BIT              2U
#define CSA_SEN_LVL_SHIFT          0U

/*
 * CSA_GAIN[1:0] — bits 7:6 of CSA_CTRL:
 *   0b00 =  5 V/V
 *   0b01 = 10 V/V
 *   0b10 = 20 V/V  <- Used in this driver and recommended by Jooyoung
 *   0b11 = 40 V/V
 */
#define CSA_GAIN_5VV               0x0U
#define CSA_GAIN_10VV              0x1U
#define CSA_GAIN_20VV              0x2U
#define CSA_GAIN_40VV              0x3U

/* =========================================================================
 * Enumerations
 * ========================================================================= */

typedef enum {
    DRV8323_MODE_6PWM  = DRV_PWM_MODE_6X,
    DRV8323_MODE_3PWM  = DRV_PWM_MODE_3X,
    DRV8323_MODE_1PWM  = DRV_PWM_MODE_1X,
    DRV8323_MODE_INDEP = DRV_PWM_MODE_INDEP,
} DRV8323_PhaseMode_t;

typedef enum {
    DRV8323_DT_50NS  = DRV_DEAD_TIME_50NS,
    DRV8323_DT_100NS = DRV_DEAD_TIME_100NS,
    DRV8323_DT_200NS = DRV_DEAD_TIME_200NS,
    DRV8323_DT_400NS = DRV_DEAD_TIME_400NS,
} DRV8323_DeadTime_t;

typedef enum {
    DRV8323_OK        = 0,
    DRV8323_ERR_SPI   = 1,
    DRV8323_ERR_ARG   = 2,
    DRV8323_ERR_FAULT = 3,
} DRV8323_Result_t;

/* =========================================================================
 * One instance per DRV8323 chip. Hardware fields must be populated by the
 * caller before DRV8323_DefaultConfig() / DRV8323_Init() are called.
 * ========================================================================= */
typedef struct {
    /* Hardware bindings */ //TODO
    SPI_HandleTypeDef *hspi;         /* STM32 HAL SPI handle                 */
    GPIO_TypeDef      *cs_port;      /* CS GPIO port, e.g. GPIOA             */
    uint16_t           cs_pin;       /* CS GPIO pin,  e.g. GPIO_PIN_4        */
    GPIO_TypeDef      *en_port;      /* ENABLE GPIO port                      */
    uint16_t           en_pin;       /* ENABLE GPIO pin                       */
    GPIO_TypeDef      *nfault_port;  /* nFAULT GPIO port (NULL if unused)    */
    uint16_t           nfault_pin;

    /* Configuration (set by DRV8323_DefaultConfig or caller) */
    DRV8323_PhaseMode_t phase_mode;
    DRV8323_DeadTime_t  dead_time;

    /* Register shadows — kept in sync with hardware */
    uint16_t reg_drv_ctrl;
    uint16_t reg_gate_hs;
    uint16_t reg_gate_ls;
    uint16_t reg_ocp_ctrl;
    uint16_t reg_csa_ctrl;

    /* Last-read status */
    uint16_t status_fault;
    uint16_t status_vgs;

    /* Debug */
    uint8_t  motor_id;
    bool     initialized;
} DRV8323_Handle_t;

/**
 * Populate register shadows with safe defaults (gain=20V/V, dt=100ns).
 * Call before DRV8323_Init() unless you set shadows manually.
 */
void             DRV8323_DefaultConfig(DRV8323_Handle_t   *hdrv,
                                        DRV8323_PhaseMode_t mode,
                                        DRV8323_DeadTime_t  dt);

/** Assert ENABLE and write all five control registers over SPI. */
DRV8323_Result_t DRV8323_Init        (DRV8323_Handle_t *hdrv);

/** Read FAULT_STAT and VGS_STAT into hdrv->status_*. */
DRV8323_Result_t DRV8323_ReadStatus  (DRV8323_Handle_t *hdrv);

/** Toggle CLR_FLT to clear all latched fault bits. */
DRV8323_Result_t DRV8323_ClearFaults (DRV8323_Handle_t *hdrv);

/** Drive ENABLE high (gate outputs active). */
void             DRV8323_Enable      (DRV8323_Handle_t *hdrv);

/** Drive ENABLE low (gate outputs off). */
void             DRV8323_Disable     (DRV8323_Handle_t *hdrv);

/** Change PWM phase mode and re-write DRV_CTRL. */
DRV8323_Result_t DRV8323_SetPhaseMode(DRV8323_Handle_t   *hdrv,
                                       DRV8323_PhaseMode_t mode);

/** Change dead-time and re-write OCP_CTRL. */
DRV8323_Result_t DRV8323_SetDeadTime (DRV8323_Handle_t  *hdrv,
                                       DRV8323_DeadTime_t dt);

/** Low-level 16-bit SPI register read. */
DRV8323_Result_t DRV8323_ReadReg     (DRV8323_Handle_t *hdrv,
                                       uint8_t reg, uint16_t *out);

/** Low-level 16-bit SPI register write. */
DRV8323_Result_t DRV8323_WriteReg    (DRV8323_Handle_t *hdrv,
                                       uint8_t reg, uint16_t val);

/** Returns true if nFAULT GPIO is asserted LOW. */
bool             DRV8323_IsFaulted   (DRV8323_Handle_t *hdrv);

#ifdef __cplusplus
}
#endif

#endif /* DRV8323_H */
