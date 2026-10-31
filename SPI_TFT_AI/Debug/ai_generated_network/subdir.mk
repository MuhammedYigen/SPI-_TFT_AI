################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ai_generated_network/network.c \
../ai_generated_network/network_data.c 

OBJS += \
./ai_generated_network/network.o \
./ai_generated_network/network_data.o 

C_DEPS += \
./ai_generated_network/network.d \
./ai_generated_network/network_data.d 


# Each subdirectory must supply rules for building sources it contributes
ai_generated_network/%.o ai_generated_network/%.su ai_generated_network/%.cyclo: ../ai_generated_network/%.c ai_generated_network/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/Muhammed/Desktop/Genel/Projeler/Youtube/STM32_Files/SPI_TFT_AI/ili9341" -I"C:/Users/Muhammed/Desktop/Genel/Projeler/Youtube/STM32_Files/SPI_TFT_AI/AI/App" -I"C:/Users/Muhammed/Desktop/Genel/Projeler/Youtube/STM32_Files/SPI_TFT_AI/ai_generated_network" -I"C:/Users/Muhammed/Desktop/Genel/Projeler/Youtube/STM32_Files/SPI_TFT_AI/Middlewares/ST/AI/Inc" -I"C:/Users/Muhammed/Desktop/Genel/Projeler/Youtube/STM32_Files/SPI_TFT_AI/Middlewares/ST/AI/Misc/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-ai_generated_network

clean-ai_generated_network:
	-$(RM) ./ai_generated_network/network.cyclo ./ai_generated_network/network.d ./ai_generated_network/network.o ./ai_generated_network/network.su ./ai_generated_network/network_data.cyclo ./ai_generated_network/network_data.d ./ai_generated_network/network_data.o ./ai_generated_network/network_data.su

.PHONY: clean-ai_generated_network

