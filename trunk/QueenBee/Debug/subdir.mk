################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../DebugLog.cpp \
../Packet.cpp \
../Protocol.cpp \
../QueenBee.cpp \
../RS232Port.cpp 

OBJS += \
./DebugLog.o \
./Packet.o \
./Protocol.o \
./QueenBee.o \
./RS232Port.o 

CPP_DEPS += \
./DebugLog.d \
./Packet.d \
./Protocol.d \
./QueenBee.d \
./RS232Port.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


