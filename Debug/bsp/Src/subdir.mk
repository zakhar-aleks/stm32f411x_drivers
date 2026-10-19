################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../bsp/Src/ds1307.c 

OBJS += \
./bsp/Src/ds1307.o 

C_DEPS += \
./bsp/Src/ds1307.d 


# Each subdirectory must supply rules for building sources it contributes
bsp/Src/%.o bsp/Src/%.su bsp/Src/%.cyclo: ../bsp/Src/%.c bsp/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DNUCLEO_F411RE -DSTM32 -DSTM32F4 -DSTM32F411RETx -c -I../Inc -I"/Users/zaharaleksandruk/STM32CubeIDE/workspace_2.2.0/stm32f411x_drivers/bsp/Inc" -I"/Users/zaharaleksandruk/STM32CubeIDE/workspace_2.2.0/stm32f411x_drivers/bsp" -I"/Users/zaharaleksandruk/STM32CubeIDE/workspace_2.2.0/stm32f411x_drivers/drivers/Inc" -O0 -ffunction-sections -fdata-sections -Wall -Wall -Wextra -Werror -Wconversion -Wpedantic -Wshadow -Wcast-align -Wcast-qual -Wlogical-op -Wstrict-prototypes -Wuninitialized -Wmaybe-uninitialized -Wc++-compat -Wenum-conversion -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-bsp-2f-Src

clean-bsp-2f-Src:
	-$(RM) ./bsp/Src/ds1307.cyclo ./bsp/Src/ds1307.d ./bsp/Src/ds1307.o ./bsp/Src/ds1307.su

.PHONY: clean-bsp-2f-Src

