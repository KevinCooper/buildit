################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/args.c \
../src/args_log.c \
../src/argv.c \
../src/dictionary.c \
../src/functions.c \
../src/hash.c \
../src/htmlPrint.c \
../src/list.c \
../src/logread.c 

OBJS += \
./src/args.o \
./src/args_log.o \
./src/argv.o \
./src/dictionary.o \
./src/functions.o \
./src/hash.o \
./src/htmlPrint.o \
./src/list.o \
./src/logread.o 

C_DEPS += \
./src/args.d \
./src/args_log.d \
./src/argv.d \
./src/dictionary.d \
./src/functions.d \
./src/hash.d \
./src/htmlPrint.d \
./src/list.d \
./src/logread.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


