################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Lcd_Driver.c \
../src/QDTFT_demo.c \
../src/char_echo.c \
../src/delay.c \
../src/handrecog.c \
../src/ov9653.c \
../src/sub_functions.c 

OBJS += \
./src/Lcd_Driver.doj \
./src/QDTFT_demo.doj \
./src/char_echo.doj \
./src/delay.doj \
./src/handrecog.doj \
./src/ov9653.doj \
./src/sub_functions.doj 

C_DEPS += \
./src/Lcd_Driver.d \
./src/QDTFT_demo.d \
./src/char_echo.d \
./src/delay.d \
./src/handrecog.d \
./src/ov9653.d \
./src/sub_functions.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.doj: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: CrossCore Blackfin C/C++ Compiler'
	ccblkfn.exe -c -file-attr ProjectName="handrecog1" -proc ADSP-BF707 -flags-compiler --no_wrap_diagnostics -si-revision 0.0 -O -Ov100 -DNDEBUG -DNO_UTILITY_ROM_TWI -DNO_UTILITY_ROM_SPI -DNO_UTILITY_ROM_PPI -I"D:\Programs\CrossCore_Embedded_Studio\handrecog1\system" -I"G:/Program_Files/Analog Devices/Vision_Analytics_Toolbox/ImageProcessingToolbox-BF-Rel2.4.0/Include" -structs-do-not-overlap -no-multiline -warn-protos -double-size-32 -decls-strong -cplbs -gnu-style-dependencies -MD -Mo "$(basename $@).d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


