################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ili9341/fonts.c \
../ili9341/ili9341.c \
../ili9341/ili9341_touch.c 

OBJS += \
./ili9341/fonts.o \
./ili9341/ili9341.o \
./ili9341/ili9341_touch.o 

C_DEPS += \
./ili9341/fonts.d \
./ili9341/ili9341.d \
./ili9341/ili9341_touch.d 


# Each subdirectory must supply rules for building sources it contributes
ili9341/%.o ili9341/%.su ili9341/%.cyclo: ../ili9341/%.c ili9341/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/Muhammed/Desktop/Genel/Projeler/Youtube/STM32_Files/SPI_TFT_AI/ili9341" -I"C:/Users/Muhammed/Desktop/Genel/Projeler/Youtube/STM32_Files/SPI_TFT_AI/AI/App" -I"C:/Users/Muhammed/Desktop/Genel/Projeler/Youtube/STM32_Files/SPI_TFT_AI/ai_generated_network" -I"C:/Users/Muhammed/Desktop/Genel/Projeler/Youtube/STM32_Files/SPI_TFT_AI/Middlewares/ST/AI/Inc" -I"C:/Users/Muhammed/Desktop/Genel/Projeler/Youtube/STM32_Files/SPI_TFT_AI/Middlewares/ST/AI/Misc/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-ili9341

clean-ili9341:
	-$(RM) ./ili9341/fonts.cyclo ./ili9341/fonts.d ./ili9341/fonts.o ./ili9341/fonts.su ./ili9341/ili9341.cyclo ./ili9341/ili9341.d ./ili9341/ili9341.o ./ili9341/ili9341.su ./ili9341/ili9341_touch.cyclo ./ili9341/ili9341_touch.d ./ili9341/ili9341_touch.o ./ili9341/ili9341_touch.su

.PHONY: clean-ili9341

