################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
ads1115.o: ../ads1115.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Arm Compiler - building file: "$<"'
	"C:/ti/ccs2100/ccs/tools/compiler/ti-cgt-armllvm_5.1.1.LTS/bin/tiarmclang.exe" -c -mcpu=cortex-m33 -mfloat-abi=hard -mfpu=fpv5-sp-d16 -mlittle-endian -mthumb -O0 -I"F:/Projects/testbench/testbench/firmware/testbench" -I"C:/ti/am13e230x_sdk_26_01_00_03/source/compiler/m33_ti_arm_clang" -I"C:/ti/am13e230x_sdk_26_01_00_03/source/device/am13e230x/include" -I"C:/ti/am13e230x_sdk_26_01_00_03/source/device/am13e230x/include/hw" -I"C:/ti/am13e230x_sdk_26_01_00_03/ti_sdk_config/am13e230x/default/device_support/include" -I"C:/ti/am13e230x_sdk_26_01_00_03/source/arch/include" -I"C:/ti/am13e230x_sdk_26_01_00_03/source/arch/m33/include" -I"C:/ti/am13e230x_sdk_26_01_00_03/ti_sdk_config/am13e230x/default/arch_cfg" -I"C:/ti/am13e230x_sdk_26_01_00_03/ti_sdk_config/am13e230x/default/arch_cfg/m33" -I"C:/ti/am13e230x_sdk_26_01_00_03/source/cmsis/Core/Include" -I"C:/ti/am13e230x_sdk_26_01_00_03/source/driverlib/am13e230x" -I"C:/ti/am13e230x_sdk_26_01_00_03/source/utils/log/include" -I"C:/ti/am13e230x_sdk_26_01_00_03/ti_sdk_config/am13e230x/default/utils_cfg" -I"F:/Projects/testbench/testbench/firmware/testbench/am13e230x_lp/m33_nortos" -I"F:/Projects/testbench/testbench/firmware/testbench/am13e230x_lp" -I"F:/Projects/testbench/testbench/firmware/testbench/" -I"C:/ti/ccs2100/ccs/tools/compiler/ti-cgt-armllvm_5.1.1.LTS/include" -D_DEBUG -g -Wextra -Wall -Werror -Wno-gnu-variable-sized-type-not-at-end -Wno-unused-function -Wno-unused-command-line-argument -Wno-unused-parameter -march=armv8.1-m.main+cdecp0 -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(basename\ $(<F)).o" -I"F:/Projects/testbench/testbench/firmware/testbench/Debug/syscfg"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

%.o: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Arm Compiler - building file: "$<"'
	"C:/ti/ccs2100/ccs/tools/compiler/ti-cgt-armllvm_5.1.1.LTS/bin/tiarmclang.exe" -c -mcpu=cortex-m33 -mfloat-abi=hard -mfpu=fpv5-sp-d16 -mlittle-endian -mthumb -Og -I"F:/Projects/testbench/testbench/firmware/testbench" -I"C:/ti/am13e230x_sdk_26_01_00_03/source/compiler/m33_ti_arm_clang" -I"C:/ti/am13e230x_sdk_26_01_00_03/source/device/am13e230x/include" -I"C:/ti/am13e230x_sdk_26_01_00_03/source/device/am13e230x/include/hw" -I"C:/ti/am13e230x_sdk_26_01_00_03/ti_sdk_config/am13e230x/default/device_support/include" -I"C:/ti/am13e230x_sdk_26_01_00_03/source/arch/include" -I"C:/ti/am13e230x_sdk_26_01_00_03/source/arch/m33/include" -I"C:/ti/am13e230x_sdk_26_01_00_03/ti_sdk_config/am13e230x/default/arch_cfg" -I"C:/ti/am13e230x_sdk_26_01_00_03/ti_sdk_config/am13e230x/default/arch_cfg/m33" -I"C:/ti/am13e230x_sdk_26_01_00_03/source/cmsis/Core/Include" -I"C:/ti/am13e230x_sdk_26_01_00_03/source/driverlib/am13e230x" -I"C:/ti/am13e230x_sdk_26_01_00_03/source/utils/log/include" -I"C:/ti/am13e230x_sdk_26_01_00_03/ti_sdk_config/am13e230x/default/utils_cfg" -I"F:/Projects/testbench/testbench/firmware/testbench/am13e230x_lp/m33_nortos" -I"F:/Projects/testbench/testbench/firmware/testbench/am13e230x_lp" -I"F:/Projects/testbench/testbench/firmware/testbench/" -I"C:/ti/ccs2100/ccs/tools/compiler/ti-cgt-armllvm_5.1.1.LTS/include" -D_DEBUG -g -Wextra -Wall -Werror -Wno-gnu-variable-sized-type-not-at-end -Wno-unused-function -Wno-unused-command-line-argument -Wno-unused-parameter -march=armv8.1-m.main+cdecp0 -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)" -I"F:/Projects/testbench/testbench/firmware/testbench/Debug/syscfg"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

build-914985350: ../example.syscfg
	@echo 'SysConfig - building file: "$<"'
	"C:/ti/ccs2100/ccs/utils/sysconfig_1.28.0/sysconfig_cli.bat" -s "C:/ti/am13e230x_sdk_26_01_00_03/.metadata/product.json" --script "F:/Projects/testbench/testbench/firmware/testbench/example.syscfg" -o "syscfg" --compiler ticlang
	@echo 'Finished building: "$<"'
	@echo ' '

syscfg/ti_sdk_dl_config.c: build-914985350 ../example.syscfg
syscfg/ti_sdk_dl_config.h: build-914985350
syscfg/internalConnections.dot: build-914985350
syscfg: build-914985350

syscfg/%.o: ./syscfg/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Arm Compiler - building file: "$<"'
	"C:/ti/ccs2100/ccs/tools/compiler/ti-cgt-armllvm_5.1.1.LTS/bin/tiarmclang.exe" -c -mcpu=cortex-m33 -mfloat-abi=hard -mfpu=fpv5-sp-d16 -mlittle-endian -mthumb -Og -I"F:/Projects/testbench/testbench/firmware/testbench" -I"C:/ti/am13e230x_sdk_26_01_00_03/source/compiler/m33_ti_arm_clang" -I"C:/ti/am13e230x_sdk_26_01_00_03/source/device/am13e230x/include" -I"C:/ti/am13e230x_sdk_26_01_00_03/source/device/am13e230x/include/hw" -I"C:/ti/am13e230x_sdk_26_01_00_03/ti_sdk_config/am13e230x/default/device_support/include" -I"C:/ti/am13e230x_sdk_26_01_00_03/source/arch/include" -I"C:/ti/am13e230x_sdk_26_01_00_03/source/arch/m33/include" -I"C:/ti/am13e230x_sdk_26_01_00_03/ti_sdk_config/am13e230x/default/arch_cfg" -I"C:/ti/am13e230x_sdk_26_01_00_03/ti_sdk_config/am13e230x/default/arch_cfg/m33" -I"C:/ti/am13e230x_sdk_26_01_00_03/source/cmsis/Core/Include" -I"C:/ti/am13e230x_sdk_26_01_00_03/source/driverlib/am13e230x" -I"C:/ti/am13e230x_sdk_26_01_00_03/source/utils/log/include" -I"C:/ti/am13e230x_sdk_26_01_00_03/ti_sdk_config/am13e230x/default/utils_cfg" -I"F:/Projects/testbench/testbench/firmware/testbench/am13e230x_lp/m33_nortos" -I"F:/Projects/testbench/testbench/firmware/testbench/am13e230x_lp" -I"F:/Projects/testbench/testbench/firmware/testbench/" -I"C:/ti/ccs2100/ccs/tools/compiler/ti-cgt-armllvm_5.1.1.LTS/include" -D_DEBUG -g -Wextra -Wall -Werror -Wno-gnu-variable-sized-type-not-at-end -Wno-unused-function -Wno-unused-command-line-argument -Wno-unused-parameter -march=armv8.1-m.main+cdecp0 -MMD -MP -MF"syscfg/$(basename $(<F)).d_raw" -MT"$(@)" -I"F:/Projects/testbench/testbench/firmware/testbench/Debug/syscfg"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

main.o: ../main.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Arm Compiler - building file: "$<"'
	"C:/ti/ccs2100/ccs/tools/compiler/ti-cgt-armllvm_5.1.1.LTS/bin/tiarmclang.exe" -c -mcpu=cortex-m33 -mfloat-abi=hard -mfpu=fpv5-sp-d16 -mlittle-endian -mthumb -O0 -I"F:/Projects/testbench/testbench/firmware/testbench" -I"C:/ti/am13e230x_sdk_26_01_00_03/source/compiler/m33_ti_arm_clang" -I"C:/ti/am13e230x_sdk_26_01_00_03/source/device/am13e230x/include" -I"C:/ti/am13e230x_sdk_26_01_00_03/source/device/am13e230x/include/hw" -I"C:/ti/am13e230x_sdk_26_01_00_03/ti_sdk_config/am13e230x/default/device_support/include" -I"C:/ti/am13e230x_sdk_26_01_00_03/source/arch/include" -I"C:/ti/am13e230x_sdk_26_01_00_03/source/arch/m33/include" -I"C:/ti/am13e230x_sdk_26_01_00_03/ti_sdk_config/am13e230x/default/arch_cfg" -I"C:/ti/am13e230x_sdk_26_01_00_03/ti_sdk_config/am13e230x/default/arch_cfg/m33" -I"C:/ti/am13e230x_sdk_26_01_00_03/source/cmsis/Core/Include" -I"C:/ti/am13e230x_sdk_26_01_00_03/source/driverlib/am13e230x" -I"C:/ti/am13e230x_sdk_26_01_00_03/source/utils/log/include" -I"C:/ti/am13e230x_sdk_26_01_00_03/ti_sdk_config/am13e230x/default/utils_cfg" -I"F:/Projects/testbench/testbench/firmware/testbench/am13e230x_lp/m33_nortos" -I"F:/Projects/testbench/testbench/firmware/testbench/am13e230x_lp" -I"F:/Projects/testbench/testbench/firmware/testbench/" -I"C:/ti/ccs2100/ccs/tools/compiler/ti-cgt-armllvm_5.1.1.LTS/include" -D_DEBUG -g -Wextra -Wall -Werror -Wno-gnu-variable-sized-type-not-at-end -Wno-unused-function -Wno-unused-command-line-argument -Wno-unused-parameter -march=armv8.1-m.main+cdecp0 -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(basename\ $(<F)).o" -I"F:/Projects/testbench/testbench/firmware/testbench/Debug/syscfg"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


