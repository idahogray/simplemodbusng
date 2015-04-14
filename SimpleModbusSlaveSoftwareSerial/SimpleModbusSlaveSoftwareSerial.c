#include <assert.h>
#include <stdio.h>
#include "SimpleModbusSlaveSoftwareSerial.h"
#include "HoldingRegisters.h"
#include "ModbusCRC.h"


const unsigned char BROADCAST_ADDRESS = 0;
const unsigned char SLAVE_ID_POSITION = 0;
const unsigned char FUNCTION_CODE_POSITION = 1;
const char BUFFER_ERROR = -1;
const unsigned char READ_COILS = 1;
const unsigned char READ_HOLDING_REGISTERS = 3;
const unsigned char WRITE_SINGLE_COIL = 5;
const unsigned char WRITE_SINGLE_REGISTER = 6;
const unsigned char WRITE_MULTIPLE_COILS = 15;
const unsigned char WRITE_MULTIPLE_REGISTERS = 16;

// frame[] is used to recieve and transmit packages. 
// The maximum serial ring buffer size is 128
unsigned char frame[BUFFER_SIZE];
unsigned int holdingRegsSize; // size of the register array 
unsigned int coilsSize; // size of the register array 
unsigned char slaveID;
unsigned char TxEnablePin;
unsigned int T1_5; // inter character time out
unsigned int T3_5; // frame delay

unsigned char get_function_code(unsigned char *frame, unsigned char frame_size)
{
    assert(frame_size >= FUNCTION_CODE_POSITION);
    unsigned char function_code = frame[FUNCTION_CODE_POSITION];
    assert(
        function_code == READ_COILS || 
        function_code == READ_HOLDING_REGISTERS ||
        function_code == WRITE_SINGLE_COIL ||
        function_code == WRITE_SINGLE_REGISTER ||
        function_code == WRITE_MULTIPLE_COILS ||
        function_code == WRITE_MULTIPLE_REGISTERS
    );
    return function_code;
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

bool is_broadcast_message(unsigned char *frame, unsigned char frame_size)
{
  return get_slave_id(frame, frame_size) == BROADCAST_ADDRESS;
}

bool verify_broadcast_and_function_code(unsigned char *frame, unsigned char frame_size)
{
    bool is_broadcast = is_broadcast_message(frame, frame_size);
    bool is_read_coils = get_function_code(frame, frame_size) == READ_COILS;
    bool is_read_holding_registers = get_function_code(frame, frame_size) == READ_HOLDING_REGISTERS;
    bool is_broadcast_and_read_coils = is_broadcast && is_read_coils;
    bool is_broadcast_and_read_holding_registers = is_broadcast && is_read_holding_registers;
    if (is_broadcast_and_read_coils || is_broadcast_and_read_holding_registers)
        return false;

    return true;
}

bool destined_for_me(unsigned char *frame, unsigned char frame_size, unsigned char my_slave_id)
{
    bool slave_id_match = verify_slave_id_matches(frame, frame_size, my_slave_id);
    bool broadcast = is_broadcast_message(frame, frame_size);
    bool slave_id_match_or_broadcast = slave_id_match || broadcast;
    if ( slave_id_match_or_broadcast )
        return true;

    return false;
}

/*void read_coils(bool *coils)
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
  //frame[1] = function_code;
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
}*/


