CC=gcc
CFLAGS=-Wall -g -std=c99

objects = SimpleModbusSlaveSoftwareSerial.o HoldingRegisters.o ModbusCRC.o ModbusException.o test_ModbusSlave.o

all: run

run:
	./test_ModbusSlave.exe

test : $(objects)
	gcc -o test_ModbusSlave.exe $(objects)

test_ModbusSlave.exe : test_ModbusSlave.h SimpleModbusSlaveSoftwareSerial.h HoldingRegisters.h ModbusCRC.h 
SimpleModbusSlaveSoftwareSerial.o : SimpleModbusSlaveSoftwareSerial.h HoldingRegisters.h ModbusCRC.h
ModbusCRC.o : ModbusCRC.h
ModbusException.o : ModbusException.h ModbusCRC.h

.PHONY : clean
clean:
	rm -f $(objects)
