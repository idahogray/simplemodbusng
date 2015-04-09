#include <assert.h>
#include "SimpleModbusSlaveSoftwareSerial.h"


const char BROADCAST_ADDRESS = 0;
const char SLAVE_ID_POSITION = 0;
const char FUNCTION_CODE_POSITION = 1;
const char BUFFER_ERROR = -1;
const char READ_COILS = 1;
const char READ_HOLDING_REGISTERS = 3;
const char WRITE_SINGLE_COIL = 5;
const char WRITE_SINGLE_REGISTER = 6;
const char WRITE_MULTIPLE_COILS = 15;
const char WRITE_MULTIPLE_REGISTERS = 16;

// frame[] is used to recieve and transmit packages. 
// The maximum serial ring buffer size is 128
unsigned char frame[BUFFER_SIZE];
unsigned int holdingRegsSize; // size of the register array 
unsigned int coilsSize; // size of the register array 
unsigned char broadcastFlag;
unsigned char slaveID;
unsigned char function_code;
unsigned char TxEnablePin;
unsigned int errorCount;
unsigned int T1_5; // inter character time out
unsigned int T3_5; // frame delay

struct modbus_response modbus_update()
{
    struct modbus_response response = { 0, { 0 } };
    return response;
}

bool verify_crc(unsigned char buffer_size)
{
  // combine the crc Low & High bytes
  unsigned int crc = ((frame[buffer_size - 2] << 8) | frame[buffer_size - 1]);
  return calculateCRC(buffer_size - 2) == crc;
}

bool verify_frame_size(unsigned char *frame, unsigned char buffer_size)
{
  if (
        frame[FUNCTION_CODE_POSITION] == READ_COILS || 
        frame[FUNCTION_CODE_POSITION] == READ_HOLDING_REGISTERS ||
        frame[FUNCTION_CODE_POSITION] == WRITE_SINGLE_COIL ||
        frame[FUNCTION_CODE_POSITION] == WRITE_SINGLE_REGISTER
    )
  {
#ifdef DEBUG
    Serial.print("Function Code: 0x");
    Serial.println(frame[FUNCTION_CODE_POSITION], HEX);
    Serial.print("Buffer Size: 0x");
    Serial.println(buffer_size, HEX);
#endif
    if (buffer_size < 8)
    {
      return false;
    }
    else
    {
      return true;
    }
  }
  else if (frame[FUNCTION_CODE_POSITION] == WRITE_MULTIPLE_COILS || 
            frame[FUNCTION_CODE_POSITION] == WRITE_MULTIPLE_REGISTERS)
  {
    if (buffer_size < 10)
    {
      return false;
    }
    else
    {
      return true;
    }
  }
  else
  {
      return false;
  }
}

unsigned char get_slave_id(unsigned char *frame, unsigned char buffer_size)
{
    assert(buffer_size > SLAVE_ID_POSITION);
    unsigned char slave_id = frame[SLAVE_ID_POSITION];
    assert(slave_id >= 0 && slave_id < 248);
    return slave_id;
}

bool verify_slave_id_matches(unsigned char *frame, unsigned char buffer_size, unsigned char slave_id)
{
  return get_slave_id(frame, buffer_size) == slave_id;
}

bool is_broadcast_message(unsigned char *frame)
{
#ifdef DEBUG
  Serial.print("Slave ID: 0x");
  Serial.println(get_slave_id(frame, sizeof(frame)), HEX);
  Serial.print("Broadcast Address: 0x");
  Serial.println(BROADCAST_ADDRESS, HEX);
#endif
  return get_slave_id(frame, sizeof(frame)) == BROADCAST_ADDRESS;
}

bool verify_broadcast_and_function_code()
{
  if (broadcastFlag && (
        function_code == READ_COILS || 
        function_code == READ_HOLDING_REGISTERS))
  {
    return false;
  }
  else
  {
    return true;
  }
}

bool destined_for_me(unsigned char *frame, unsigned char frame_size, unsigned char my_slave_id)
{
#ifdef DEBUG
  Serial.print("Verify Slave ID Matches: 0x");
  Serial.println(verify_slave_id_matches(frame, frame_size, my_slave_id), HEX);
  Serial.print("Broadcast Flag: 0x");
  Serial.println(broadcastFlag, HEX);
#endif
  if (verify_slave_id_matches(frame, frame_size, my_slave_id) || broadcastFlag)
  {
    return true;
  }
  else
  {
    return false;
  }
}

void read_coils(bool *coils)
{
  // combine the starting address bytes
  unsigned int startingAddress = ((frame[2] << 8) | frame[3]);
  if (startingAddress < coilsSize) // chec/k exception 2 ILLEGAL DATA ADDRESS
  {
    exceptionResponse(2); // exception 2 ILLEGAL DATA ADDRESS
  }
  // combine the number of register bytes  
  unsigned int no_of_coils = ((frame[4] << 8) | frame[5]);
  unsigned int maxData = startingAddress + no_of_coils;
  if (maxData <= coilsSize || maxData > 2000) // check exception 3 ILLEGAL DATA VALUE
  {
    exceptionResponse(3); // exception 3 ILLEGAL DATA VALUE
  }

  unsigned char noOfBytes = no_of_coils / 8;
  if (no_of_coils % 8 > 0)
  {
    noOfBytes += 1;
  }
  unsigned char responseFrameSize = 5 + noOfBytes;
  unsigned char address;
  unsigned char index;
  unsigned char crc16;
  frame[0] = slaveID;
  frame[1] = function_code;
  frame[2] = noOfBytes;
  address = 3;
  unsigned char temp;
  unsigned char bit_position = 0;
  unsigned char val;

  for (index = startingAddress; index < maxData; index++)
  {
    if (index % 8 == 0)
    {
      frame[address] = temp;
      temp = 0;
      address++;
    }
    if (coils[index] == true)
    {
      val = 1 << (index % 8);
    }
    else
    {
      val = 0;
    }
    temp |= val;
  }
}

void read_holding_registers(unsigned int *holdingRegs)
{
  // combine the starting address bytes
  unsigned int startingAddress = ((frame[2] << 8) | frame[3]);
#ifdef DEBUG
  Serial.print("Starting Address: 0x");
  Serial.println(startingAddress, HEX);
#endif
  if (startingAddress >= holdingRegsSize) // check exception 2 ILLEGAL DATA ADDRESS
  {
    exceptionResponse(2); // exception 2 ILLEGAL DATA ADDRESS
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
  if (maxData >= holdingRegsSize) // check exception 3 ILLEGAL DATA VALUE
  {
    exceptionResponse(3); // exception 3 ILLEGAL DATA VALUE
    return;
  }

  unsigned char noOfBytes = no_of_registers * 2;
  // ID, function, noOfBytes, (dataLo + dataHi) * number of registers, crcLo, crcHi
  unsigned char responseFrameSize = 5 + noOfBytes;
  unsigned char address;
  unsigned char index;
  unsigned int crc16;
  frame[0] = slaveID;
  frame[1] = function_code;
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
    temp = holdingRegs[index];
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
}

void write_single_register(unsigned int *holdingRegs)
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
}

void write_multiple_registers(unsigned int *holdingRegs, unsigned char buffer_size)
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
}


void exceptionResponse(unsigned char exception)
{
  errorCount++; // each call to exceptionResponse() will increment the errorCount
  if (!broadcastFlag) // don't respond if its a broadcast message
  {
    frame[0] = slaveID;
    frame[1] = (function_code | 0x80); // set the MSB bit high, informs the master of an exception
    frame[2] = exception;
    unsigned int crc16 = calculateCRC(3); // ID, function_code + 0x80, exception code == 3 bytes
    frame[3] = crc16 >> 8;
    frame[4] = crc16 & 0xFF;
  }
}

unsigned int calculateCRC(byte bufferSize) 
{
  unsigned int temp, temp2, flag;
  temp = 0xFFFF;
  for (unsigned char i = 0; i < bufferSize; i++)
  {
    temp = temp ^ frame[i];
    for (unsigned char j = 1; j <= 8; j++)
    {
      flag = temp & 0x0001;
      temp >>= 1;
      if (flag)
        temp ^= 0xA001;
    }
  }
  // Reverse byte order. 
  temp2 = temp >> 8;
  temp = (temp << 8) | temp2;
  temp &= 0xFFFF;
  return temp; // the returned value is already swopped - crcLo byte is first & crcHi byte is last
}
