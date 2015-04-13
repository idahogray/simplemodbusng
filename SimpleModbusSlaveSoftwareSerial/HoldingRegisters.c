#include "HoldingRegisters.h"
#include "ModbusCRC.h"

void read_holding_registers(unsigned char *frame, unsigned char frame_size, unsigned int *holding_registers, unsigned int holding_registers_size)
{
  /*// combine the starting address bytes
  unsigned int startingAddress = ((frame[2] << 8) | frame[3]);
#ifdef DEBUG
  Serial.print("Starting Address: 0x");
  Serial.println(startingAddress, HEX);
#endif
  if (startingAddress >= holding_registers_size) // check exception 2 ILLEGAL DATA ADDRESS
  {
    //exceptionResponse(2); // exception 2 ILLEGAL DATA ADDRESS
    return;
  }

  // combine the number of register bytes  
  unsigned int no_of_registers = ((frame[4] << 8) | frame[5]);
  unsigned int maxData = startingAddress + no_of_registers;
#ifdef DEBUG
  Serial.print("no_of_registers: 0x");
  Serial.println(no_of_registers, HEX);
  Serial.print("maxData: 0x");
  Serial.println(maxData, HEX);
#endif
  if (maxData >= holding_registers_size) // check exception 3 ILLEGAL DATA VALUE
  {
    //exceptionResponse(3); // exception 3 ILLEGAL DATA VALUE
    return;
  }

  unsigned char noOfBytes = no_of_registers * 2;
  // ID, function, noOfBytes, (dataLo + dataHi) * number of registers, crcLo, crcHi
  unsigned char responseFrameSize = 5 + noOfBytes;
  unsigned char address;
  unsigned char index;
  unsigned int crc16;
  frame[0] = slaveID;
  //frame[1] = function_code;
  frame[2] = noOfBytes;
  address = 3; // PDU starts at the 4th byte
  unsigned int temp;
  
#ifdef DEBUG
  Serial.print("noOfBytes: 0x");
  Serial.println(noOfBytes, HEX);
  Serial.print("maxData: 0x");
  Serial.println(maxData, HEX);
#endif
  for (index = startingAddress; index < maxData; index++)
  {
#ifdef DEBUG
    Serial.print("Frame Index: 0x");
    Serial.println(index, HEX);
#endif
    temp = holding_registers[index];
    frame[address] = temp >> 8; // split the register into 2 bytes
#ifdef DEBUG
    Serial.print("Frame Value: 0x");
    Serial.println(frame[address], HEX);
#endif
    address++;
    frame[address] = temp & 0xFF;
    address++;
#ifdef DEBUG
    Serial.print("Frame Value: 0x");
    Serial.println(frame[address], HEX);
#endif
  } 
  
  crc16 = calculateCRC(responseFrameSize - 2);
  frame[responseFrameSize - 2] = crc16 >> 8; // split crc into 2 bytes
  frame[responseFrameSize - 1] = crc16 & 0xFF;
#ifdef DEBUG
  Serial.println("Response Sent");
#endif
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


