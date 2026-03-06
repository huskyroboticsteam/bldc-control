################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CAN26/Ports/PortSTM32g4xx.c 

OBJS += \
./CAN26/Ports/PortSTM32g4xx.o 

C_DEPS += \
./CAN26/Ports/PortSTM32g4xx.d 


# Each subdirectory must supply rules for building sources it contributes
CAN26/Ports/%.o CAN26/Ports/%.su CAN26/Ports/%.cyclo: ../CAN26/Ports/%.c CAN26/Ports/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G474xx -c -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I../CAN26 -I../Driver_Code -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-CAN26-2f-Ports

clean-CAN26-2f-Ports:
	-$(RM) ./CAN26/Ports/PortSTM32g4xx.cyclo ./CAN26/Ports/PortSTM32g4xx.d ./CAN26/Ports/PortSTM32g4xx.o ./CAN26/Ports/PortSTM32g4xx.su

.PHONY: clean-CAN26-2f-Ports

