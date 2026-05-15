# Setup
1) Download [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html) and [STM32CubeMX](https://www.st.com/en/development-tools/stm32cubemx.html). This will be the easiest (in my opinion) way to write code for STM32.
2) Clone this repository to your device, and open in STM32CubeIDE by clicking the .project file.
3) Connect either a dev board or a PCB using a programmer. If you are using a PCB, you must make the following change: Click on the huskyrobotics-fw-template.ioc file to open CubeMX. Search and select 'RCC', then under 'Mode', change the High Speed CLock (HSE) setting from Bypass Clock Source to Crystal/Ceramic Resonator. Press Generate Code to update your project with the correct clock settings.
4) You should be able to program your board by pressing the green play button in STM32CubeIDE (it should say Run huskyrobotics-fw-template when you hover over it). By default, the board has the following functionality:

## UART Debugging
The board has LPUART1 enabled by default, with baud rate 115200. The pins are mapped to the VCP on the dev board so you can communicate over USB; for a PCB you will have to connect to the TX (PA2) and RX (PA3) pins. \
The basic debug loop can be enabled/disabled by changing `#define DEBUG_UART_ENABLE 1 ` to 1 or 0 respectively. Inside the loop, you can add code to execute when you send certain characters over UART. The default example is implemented such that:
| Character | Result | 
|------------|--------|
|"a" | Turns on user LED (PA5) |
|"b" | Turns off user LED (PA5) |
| "c" | Sends  a PWM Set Duty Cycle packet over CAN to CAN_UUID_DEBUG2, peripheral ID 0x01, with duty cycle 50.0 |

Add your own code/character mappings as needed.

## CAN
The default CAN settings are: \
**UUID**: CAN_UUID_DEBUG1 \
**Broadcast Domains** : All \
**CAN Instance** FDCAN1, TX on PA12, RX on PA11
|Packets Implemented | |
|---|---|
| Firmware Version Get | Responds with current firmware revision |
| Heartbeat | Sends a heartbeat to cAN_UUID_JETSON every 200ms (configurable by adjusting `#define HEARTBEAT_PERIOD_MS 200`). No state/error defined yet | 
|PWM Set Duty Cycle | Peripheral ID 0x01 maps to pin PA5 (User LED on dev board). Takes duty cycle percentage between 0.0 - 100.0. |
| All other packets| If ack requested, will respond with general ack showing failure. |
  
