################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CAN26/CANPacket.c 

OBJS += \
./CAN26/CANPacket.o 

C_DEPS += \
./CAN26/CANPacket.d 


# Each subdirectory must supply rules for building sources it contributes
CAN26/%.o CAN26/%.su CAN26/%.cyclo: ../CAN26/%.c CAN26/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G474xx -c -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I../CAN26 -I../Driver_Code -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-CAN26

clean-CAN26:
	-$(RM) ./CAN26/CANPacket.cyclo ./CAN26/CANPacket.d ./CAN26/CANPacket.o ./CAN26/CANPacket.su

.PHONY: clean-CAN26

