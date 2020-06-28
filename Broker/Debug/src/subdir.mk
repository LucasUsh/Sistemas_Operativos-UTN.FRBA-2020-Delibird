################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Broker.c \
../src/Particiones.c \
../src/pruebas.c 

OBJS += \
./src/Broker.o \
./src/Particiones.o \
./src/pruebas.o 

C_DEPS += \
./src/Broker.d \
./src/Particiones.d \
./src/pruebas.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


