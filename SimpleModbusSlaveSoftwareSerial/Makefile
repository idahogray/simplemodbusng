CFLAGS=-Wall -g

all: run

run:
	./test_ModbusSlave.exe

test:
	gcc -std=c99 -c SimpleModbusSlaveSoftwareSerial.c -o SimpleModbusSlaveSoftwareSerial.o
	ar rcs libSimpleModbusSlaveSoftwareSerial.a SimpleModbusSlaveSoftwareSerial.o
	gcc -std=c99 -c HoldingRegisters.c -o HoldingRegisters.o
	ar rcs libHoldingRegisters.a HoldingRegisters.o
	gcc -std=c99 -c ModbusCRC.c -o ModbusCRC.o
	ar rcs libModbusCRC.a ModbusCRC.o
	gcc -c test_ModbusSlave.c -o test_ModbusSlave.o
	gcc -o test_ModbusSlave.exe test_ModbusSlave.o -L. -lSimpleModbusSlaveSoftwareSerial -lHoldingRegisters -lModbusCRC

clean:
	rm -f test_ModbusSlave.exe
