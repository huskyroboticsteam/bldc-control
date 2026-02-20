################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Driver_Code/phases.c 

OBJS += \
./Driver_Code/phases.o 

C_DEPS += \
./Driver_Code/phases.d 


# Each subdirectory must supply rules for building sources it contributes
Driver_Code/%.o Driver_Code/%.su Driver_Code/%.cyclo: ../Driver_Code/%.c Driver_Code/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG '-DCHIP_TYPE = CHIP_TYPE_STM32_G4XX' -DUSE_HAL_DRIVER -DSTM32G474xx -c -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I../Driver_Code -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Driver_Code

clean-Driver_Code:
	-$(RM) ./Driver_Code/phases.cyclo ./Driver_Code/phases.d ./Driver_Code/phases.o ./Driver_Code/phases.su

.PHONY: clean-Driver_Code

