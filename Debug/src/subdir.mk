################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/args.c \
../src/args_log.c \
../src/argv.c \
../src/functions.c \
../src/logread.c 

OBJS += \
./src/args.o \
./src/args_log.o \
./src/argv.o \
./src/functions.o \
./src/logread.o 

C_DEPS += \
./src/args.d \
./src/args_log.d \
./src/argv.d \
./src/functions.d \
./src/logread.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


