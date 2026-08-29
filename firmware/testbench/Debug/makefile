################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ccs2100/ccs/tools/compiler/ti-cgt-armllvm_5.1.1.LTS

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./ads1115.o" \
"./capture.o" \
"./clocks.o" \
"./configurations.o" \
"./syscfg/ti_sdk_dl_config.o" \
"./hsadc.o" \
"./main.o" \
"./protocol.o" \
"./protocol_app_interface.o" \
"./pwm.o" \
"./user_gpio.o" \
"./user_i2c.o" \
"./user_spi.o" \
"./user_timer.o" \
"./user_uart.o" \
"../linker_m33_ti_arm_clang.cmd" \
$(GEN_CMDS__FLAG) \
-Wl,-llibti_sdk_cfg_default_m33_ti_arm_clang.a \
-Wl,-llibarch_ti_sdk_cfg_default_m33_ti_arm_clang.a \
-Wl,-llibdriverlib_ti_sdk_cfg_default_m33_ti_arm_clang.a \
-Wl,-llibutils_nortos_ti_sdk_cfg_default_m33_ti_arm_clang.a \
-Wl,-llibam13e230x_m33_ti_arm_clang.a \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
testbench.out 

EXE_OUTPUTS__QUOTED += \
"testbench.out" 


# All Target
all: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@$(MAKE) --no-print-directory -Onone "testbench.out"

# Tool invocations
testbench.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Arm Linker - building target: "$@"'
	"C:/ti/ccs2100/ccs/tools/compiler/ti-cgt-armllvm_5.1.1.LTS/bin/tiarmclang.exe" -mcpu=cortex-m33 -mfloat-abi=hard -mfpu=fpv5-sp-d16 -mlittle-endian -mthumb -Og -D_DEBUG -g -Wextra -Wall -Werror -Wno-gnu-variable-sized-type-not-at-end -Wno-unused-function -Wno-unused-command-line-argument -Wno-unused-parameter -march=armv8.1-m.main+cdecp0 -Wl,-m"testbench.map" -Wl,--heap_size=0 -Wl,--stack_size=256 -Wl,-i"C:/ti/am13e230x_sdk_26_01_00_03/build/am13e230x/lib/Debug" -Wl,-i"C:/ti/am13e230x_sdk_26_01_00_03" -Wl,-i"C:/ti/ccs2100/ccs/tools/compiler/ti-cgt-armllvm_5.1.1.LTS/lib" -Wl,--reread_libs -Wl,--diag_suppress=10063 -Wl,--diag_wrap=off -Wl,--display_error_number -Wl,--warn_sections -Wl,--xml_link_info="testbench_linkInfo.xml" -Wl,--rom_model -o "testbench.out" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(GEN_MISC_FILES__QUOTED)$(GEN_FILES__QUOTED)$(EXE_OUTPUTS__QUOTED)
	-$(RM) "ads1115.o" "capture.o" "clocks.o" "configurations.o" "syscfg\ti_sdk_dl_config.o" "hsadc.o" "main.o" "protocol.o" "protocol_app_interface.o" "pwm.o" "user_gpio.o" "user_i2c.o" "user_spi.o" "user_timer.o" "user_uart.o" 
	-$(RM) "ads1115.d" "capture.d" "clocks.d" "configurations.d" "syscfg\ti_sdk_dl_config.d" "hsadc.d" "main.d" "protocol.d" "protocol_app_interface.d" "pwm.d" "user_gpio.d" "user_i2c.d" "user_spi.d" "user_timer.d" "user_uart.d" 
	-$(RMDIR) $(GEN_MISC_DIRS__QUOTED)
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

