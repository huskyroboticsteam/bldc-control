################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Driver_Code/CAN.c \
../Driver_Code/PID_draft.c \
../Driver_Code/SPI_driver.c \
../Driver_Code/fsm.c \
../Driver_Code/hall_sensor.c \
../Driver_Code/main_fake.c \
../Driver_Code/phases.c 

OBJS += \
./Driver_Code/CAN.o \
./Driver_Code/PID_draft.o \
./Driver_Code/SPI_driver.o \
./Driver_Code/fsm.o \
./Driver_Code/hall_sensor.o \
./Driver_Code/main_fake.o \
./Driver_Code/phases.o 

C_DEPS += \
./Driver_Code/CAN.d \
./Driver_Code/PID_draft.d \
./Driver_Code/SPI_driver.d \
./Driver_Code/fsm.d \
./Driver_Code/hall_sensor.d \
./Driver_Code/main_fake.d \
./Driver_Code/phases.d 


# Each subdirectory must supply rules for building sources it contributes
Driver_Code/CAN.o: ../Driver_Code/CAN.c Driver_Code/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG '-DCHIP_TYPE = CHIP_TYPE_STM32_G4XX' -DUSE_HAL_DRIVER -DSTM32G474xx -c -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I../CAN26 -I../Driver_Code -include"C:/Users/becca/Documents/GitHub/bldc-control/Motor_driver/CAN26/CANPacket.h" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Driver_Code/%.o Driver_Code/%.su Driver_Code/%.cyclo: ../Driver_Code/%.c Driver_Code/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG '-DCHIP_TYPE = CHIP_TYPE_STM32_G4XX' -DUSE_HAL_DRIVER -DSTM32G474xx -c -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I../CAN26 -I../Driver_Code -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Driver_Code

clean-Driver_Code:
	-$(RM) ./Driver_Code/CAN.cyclo ./Driver_Code/CAN.d ./Driver_Code/CAN.o ./Driver_Code/CAN.su ./Driver_Code/PID_draft.cyclo ./Driver_Code/PID_draft.d ./Driver_Code/PID_draft.o ./Driver_Code/PID_draft.su ./Driver_Code/SPI_driver.cyclo ./Driver_Code/SPI_driver.d ./Driver_Code/SPI_driver.o ./Driver_Code/SPI_driver.su ./Driver_Code/fsm.cyclo ./Driver_Code/fsm.d ./Driver_Code/fsm.o ./Driver_Code/fsm.su ./Driver_Code/hall_sensor.cyclo ./Driver_Code/hall_sensor.d ./Driver_Code/hall_sensor.o ./Driver_Code/hall_sensor.su ./Driver_Code/main_fake.cyclo ./Driver_Code/main_fake.d ./Driver_Code/main_fake.o ./Driver_Code/main_fake.su ./Driver_Code/phases.cyclo ./Driver_Code/phases.d ./Driver_Code/phases.o ./Driver_Code/phases.su

.PHONY: clean-Driver_Code

