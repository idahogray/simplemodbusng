#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "HoldingRegisters.h"
#include "ModbusCRC.h"
#include "ModbusException.h"

bool starting_address_valid(unsigned int starting_address, unsigned int holding_registers_size);
bool register_count_valid(unsigned int starting_address, unsigned int number_of_registers, unsigned int holding_registers_size);

bool starting_address_valid(unsigned int starting_address, unsigned int holding_registers_size)
{
    if (starting_address >= holding_registers_size)
    {
        return false;
    }
    return true;
}

bool register_count_valid(unsigned int starting_address, unsigned int number_of_registers, unsigned int holding_registers_size)
{
    unsigned int max_data = starting_address + number_of_registers;
    if (max_data >= holding_registers_size)
    {
        return false;
    }
    return true;
}

unsigned char *read_holding_registers(unsigned char slave_id, unsigned int starting_address, unsigned int number_of_registers, unsigned int *holding_registers, unsigned int holding_registers_size)
{
    extern unsigned char READ_HOLDING_REGISTERS;
    if (!starting_address_valid(starting_address, holding_registers_size) || !register_count_valid(starting_address, number_of_registers, holding_registers_size))
    {
        return illegal_data_address_exception(slave_id, READ_HOLDING_REGISTERS);
    }

    struct modbus_response *response;
    response = malloc(sizeof(struct modbus_response) + number_of_registers * 2);

    (*response).slave_id = slave_id;
    (*response).function_code = READ_HOLDING_REGISTERS;
    (*response).registers = malloc(number_of_registers * 2);
    int holding_register_index;
    int response_register_index = 0;
    for (holding_register_index = starting_address; holding_register_index < starting_index + number_of_registers; holding_register_index++, response_registers_index++)
    {
        (*response).registers[response_register_index] = holding_registers[holding_register_index];
    }
/*
  crc16 = calculateCRC(responseFrameSize - 2);
  frame[responseFrameSize - 2] = crc16 >> 8; // split crc into 2 bytes
  frame[responseFrameSize - 1] = crc16 & 0xFF;
*/
}

/*void write_single_register(unsigned int *holdingRegs)
{
  unsigned int startingAddress = ((frame[2] << 8) | frame[3]);
  if (startingAddress < holdingRegsSize) // check exception 2 ILLEGAL DATA ADDRESS
  {
    exceptionResponse(2); // exception 2 ILLEGAL DATA ADDRESS
  }
  unsigned int regStatus = ((frame[4] << 8) | frame[5]);
  unsigned char responseFrameSize = 8;
  unsigned int crc16;
  
  holdingRegs[startingAddress] = regStatus;
  
  crc16 = calculateCRC(responseFrameSize - 2);
  frame[responseFrameSize - 2] = crc16 >> 8; // split crc into 2 bytes
  frame[responseFrameSize - 1] = crc16 & 0xFF;
}*/

/*void write_multiple_registers(unsigned int *holdingRegs, unsigned char buffer_size)
{
  unsigned int startingAddress = ((frame[2] << 8) | frame[3]);
  // combine the number of register bytes  
  unsigned int no_of_registers = ((frame[4] << 8) | frame[5]);
  unsigned int maxData = startingAddress + no_of_registers;
  unsigned char address;
  unsigned char index;
  unsigned int crc16;
  // check if the recieved number of bytes matches the calculated bytes minus the request bytes
  // id + function_code + (2 * address bytes) + (2 * no of register bytes) + byte count + (2 * CRC bytes) = 9 bytes
  if (frame[6] == (buffer_size - 9)) 
  {
    if (startingAddress < holdingRegsSize) // check exception 2 ILLEGAL DATA ADDRESS
    {
      if (maxData <= holdingRegsSize) // check exception 3 ILLEGAL DATA VALUE
      {
        address = 7; // start at the 8th byte in the frame
        
        for (index = startingAddress; index < maxData; index++)
        {
          holdingRegs[index] = ((frame[address] << 8) | frame[address + 1]);
          address += 2;
        } 
        
        // only the first 6 bytes are used for CRC calculation
        crc16 = calculateCRC(6); 
        frame[6] = crc16 >> 8; // split crc into 2 bytes
        frame[7] = crc16 & 0xFF;
        
      }
      else  
        exceptionResponse(3); // exception 3 ILLEGAL DATA VALUE
    }
    else
      exceptionResponse(2); // exception 2 ILLEGAL DATA ADDRESS
  }
  else 
    errorCount++; // corrupted packet
}*/


