################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../callBack/callBack.c 

OBJS += \
./callBack/callBack.o 

C_DEPS += \
./callBack/callBack.d 


# Each subdirectory must supply rules for building sources it contributes
callBack/%.o callBack/%.su callBack/%.cyclo: ../callBack/%.c callBack/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G474xx -c -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I"/Users/romingo/Desktop/ENSEA/3A/TP/2324_ESE3745_CARONELLO_PACE/NUCLEO-G474RET6-Inverter_Pinout/callBack" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-callBack

clean-callBack:
	-$(RM) ./callBack/callBack.cyclo ./callBack/callBack.d ./callBack/callBack.o ./callBack/callBack.su

.PHONY: clean-callBack

