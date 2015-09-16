################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Program\ Files/CCES/Blackfin/lib/src/drivers/source/ppi/adi_ppi.c 

OBJS += \
./system/drivers/ppi/adi_ppi.doj 

C_DEPS += \
./system/drivers/ppi/adi_ppi.d 


# Each subdirectory must supply rules for building sources it contributes
system/drivers/ppi/adi_ppi.doj: C:/Program\ Files/CCES/Blackfin/lib/src/drivers/source/ppi/adi_ppi.c
	@echo 'Building file: $<'
	@echo 'Invoking: CrossCore Blackfin C/C++ Compiler'
	ccblkfn.exe -c -file-attr ProjectName="handrecog5" -proc ADSP-BF707 -flags-compiler --no_wrap_diagnostics -si-revision 0.0 -g -D_DEBUG -DNO_UTILITY_ROM_TWI -DNO_UTILITY_ROM_SPI -DNO_UTILITY_ROM_PPI -I"E:\CCES\handrecog5\system" -I"/Include" -structs-do-not-overlap -no-multiline -warn-protos -double-size-32 -decls-strong -cplbs -gnu-style-dependencies -MD -Mo "$(basename $@).d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


