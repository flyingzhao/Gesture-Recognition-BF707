################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL := ${ComSpec}

-include ../makefile.init

RM := clean

# All of the sources participating in the build are defined here
-include sources.mk
-include system/startup_ldf/subdir.mk
-include system/services/dma/subdir.mk
-include system/pinmux/GeneratedSources/subdir.mk
-include system/drivers/uart/subdir.mk
-include system/drivers/twi/subdir.mk
-include system/drivers/spi/subdir.mk
-include system/drivers/ppi/subdir.mk
-include system/subdir.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(DSP_DEPS)),)
-include $(DSP_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LINKER_LOG_OUTPUTS += \
linker_log.xml \


# All Target
all: handrecog5.dxe

# Tool invocations
handrecog5.dxe: $(OBJS) $(LDF_SRCS)
	@echo 'Building target: $@'
	@echo 'Invoking: CrossCore Blackfin Linker'
	ccblkfn.exe -proc ADSP-BF707 -si-revision 0.0 -TE:/CCES/handrecog5/system/startup_ldf/app.ldf -no-mem -LE:/CCES/handrecog5/Debug/system/startup_ldf -L/Lib-Production-CCES -flags-link -MDDEBUG,-MDNO_UTILITY_ROM_TWI,-MDNO_UTILITY_ROM_SPI,-MDNO_UTILITY_ROM_PPI -add-debug-libpaths -flags-link libadi_image_tool_box_cces_bf70x.dlb -o handrecog5.dxe $(OBJS) $(LDF_SRCS)  -flags-link -od,.\.
	@echo 'Finished building target: $@'
	@echo ' '

linker_log.xml: handrecog5.dxe $(OBJS) $(LDF_SRCS)

# Other Targets
clean:
	-$(RM) $(C++_DEPS)$(OBJS)$(C_DEPS)$(ASM_DEPS)$(CC_DEPS)$(DSP_DEPS)$(DXE)$(CPP_DEPS)$(LINKER_LOG_OUTPUTS)$(S_DEPS)$(CXX_DEPS) handrecog5.dxe
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
