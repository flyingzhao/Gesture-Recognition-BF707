################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../system/adi_initialize.c 

OBJS += \
./system/adi_initialize.doj 

C_DEPS += \
./system/adi_initialize.d 


# Each subdirectory must supply rules for building sources it contributes
system/%.doj: ../system/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: CrossCore Blackfin C/C++ Compiler'
	ccblkfn.exe -c -file-attr ProjectName="handrecog1" -proc ADSP-BF707 -flags-compiler --no_wrap_diagnostics -si-revision 0.0 -O -Ov100 -DNDEBUG -DNO_UTILITY_ROM_TWI -DNO_UTILITY_ROM_SPI -DNO_UTILITY_ROM_PPI -I"D:\Programs\CrossCore_Embedded_Studio\handrecog1\system" -I"G:/Program_Files/Analog Devices/Vision_Analytics_Toolbox/ImageProcessingToolbox-BF-Rel2.4.0/Include" -structs-do-not-overlap -no-multiline -warn-protos -double-size-32 -decls-strong -cplbs -gnu-style-dependencies -MD -Mo "$(basename $@).d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


