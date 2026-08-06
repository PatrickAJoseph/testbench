################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Add inputs and outputs from these tool invocations to the build variables 
CMD_SRCS += \
../linker_m33_ti_arm_clang.cmd 

SYSCFG_SRCS += \
../example.syscfg 

C_SRCS += \
../capture.c \
../clocks.c \
./syscfg/ti_sdk_dl_config.c \
../main.c \
../protocol.c \
../protocol_app_interface.c \
../pwm.c \
../user_uart.c 

GEN_FILES += \
./syscfg/ti_sdk_dl_config.c 

GEN_MISC_DIRS += \
./syscfg 

C_DEPS += \
./capture.d \
./clocks.d \
./syscfg/ti_sdk_dl_config.d \
./main.d \
./protocol.d \
./protocol_app_interface.d \
./pwm.d \
./user_uart.d 

OBJS += \
./capture.o \
./clocks.o \
./syscfg/ti_sdk_dl_config.o \
./main.o \
./protocol.o \
./protocol_app_interface.o \
./pwm.o \
./user_uart.o 

GEN_MISC_FILES += \
./syscfg/ti_sdk_dl_config.h \
./syscfg/internalConnections.dot 

GEN_MISC_DIRS__QUOTED += \
"syscfg" 

OBJS__QUOTED += \
"capture.o" \
"clocks.o" \
"syscfg\ti_sdk_dl_config.o" \
"main.o" \
"protocol.o" \
"protocol_app_interface.o" \
"pwm.o" \
"user_uart.o" 

GEN_MISC_FILES__QUOTED += \
"syscfg\ti_sdk_dl_config.h" \
"syscfg\internalConnections.dot" 

C_DEPS__QUOTED += \
"capture.d" \
"clocks.d" \
"syscfg\ti_sdk_dl_config.d" \
"main.d" \
"protocol.d" \
"protocol_app_interface.d" \
"pwm.d" \
"user_uart.d" 

GEN_FILES__QUOTED += \
"syscfg\ti_sdk_dl_config.c" 

C_SRCS__QUOTED += \
"../capture.c" \
"../clocks.c" \
"./syscfg/ti_sdk_dl_config.c" \
"../main.c" \
"../protocol.c" \
"../protocol_app_interface.c" \
"../pwm.c" \
"../user_uart.c" 

SYSCFG_SRCS__QUOTED += \
"../example.syscfg" 


