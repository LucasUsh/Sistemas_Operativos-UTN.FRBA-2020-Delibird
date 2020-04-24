################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Sockets/socketsCliente.c \
../Sockets/socketsServidor.c 

OBJS += \
./Sockets/socketsCliente.o \
./Sockets/socketsServidor.o 

C_DEPS += \
./Sockets/socketsCliente.d \
./Sockets/socketsServidor.d 


# Each subdirectory must supply rules for building sources it contributes
Sockets/%.o: ../Sockets/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


