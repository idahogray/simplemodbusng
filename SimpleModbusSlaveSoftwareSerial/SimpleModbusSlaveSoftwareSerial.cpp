#include "SimpleModbusSlaveSoftwareSerial.h"
#include "SoftwareSerial.h"

#define BUFFER_SIZE 128

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

// function definitions
void exceptionResponse(unsigned char exception);
unsigned int calculateCRC(unsigned char bufferSize); 
void sendPacket(unsigned char bufferSize);

SoftwareSerial mySerial(10, 11);

bool process_serial_data()
{
  unsigned char buffer_position = 0;
  unsigned char overflow = 0;
  
  while (mySerial.available())
  {
    Serial.print("Reading Data from serial port: 0x");
    // The maximum number of bytes is limited to the serial buffer size of 128 bytes
    // If more bytes is received than the BUFFER_SIZE the overflow flag will be set and the 
    // serial buffer will be red untill all the data is cleared from the receive buffer.
    if (overflow) 
      mySerial.read();
    else
    {
      if (buffer_position == BUFFER_SIZE)
        overflow = 1;
      frame[buffer_position] = mySerial.read();
      Serial.println(frame[buffer_position], HEX);
      buffer_position++;
    }
    delayMicroseconds(T1_5); // inter character time out
  }
  
  // If an overflow occurred increment the errorCount
  // variable and return to the main sketch without 
  // responding to the request i.e. force a timeout
  if (overflow)
    return BUFFER_ERROR;
  else
    return buffer_position;
}

bool verify_crc(unsigned char buffer_size)
{
  // combine the crc Low & High bytes
  unsigned int crc = ((frame[buffer_size - 2] << 8) | frame[buffer_size - 1]);
  return calculateCRC(buffer_size - 2) == crc;
}

bool verify_frame_size(unsigned char buffer_size)
{
  if (frame[FUNCTION_CODE_POSITION] == 1 || frame[FUNCTION_CODE_POSITION] == 3 ||
      frame[FUNCTION_CODE_POSITION] == 5 || frame[FUNCTION_CODE_POSITION] == 6)
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
  else if (frame[FUNCTION_CODE_POSITION] == 15 || 
            frame[FUNCTION_CODE_POSITION] == 16)
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
}

unsigned char get_slave_id()
{
  return frame[SLAVE_ID_POSITION];
}

bool verify_slave_id_matches()
{
  return get_slave_id() == slaveID;
}

bool is_broadcast_message()
{
  Serial.print("Slave ID: 0x");
  Serial.println(get_slave_id(), HEX);
  Serial.print("Broadcast Address: 0x");
  Serial.println(BROADCAST_ADDRESS, HEX);
  return get_slave_id() == BROADCAST_ADDRESS;
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

bool destined_for_me()
{
  Serial.print("Verify Slave ID Matches: 0x");
  Serial.println(verify_slave_id_matches(), HEX);
  Serial.print("Broadcast Flag: 0x");
  Serial.println(broadcastFlag, HEX);
  if (verify_slave_id_matches() || broadcastFlag)
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
  if (startingAddress < holdingRegsSize) // check exception 2 ILLEGAL DATA ADDRESS
  {
    exceptionResponse(2); // exception 2 ILLEGAL DATA ADDRESS
  }

  // combine the number of register bytes  
  unsigned int no_of_registers = ((frame[4] << 8) | frame[5]);
  unsigned int maxData = startingAddress + no_of_registers;
  if (maxData <= holdingRegsSize) // check exception 3 ILLEGAL DATA VALUE
  {
    exceptionResponse(3); // exception 3 ILLEGAL DATA VALUE
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
  
  for (index = startingAddress; index < maxData; index++)
  {
    temp = holdingRegs[index];
    frame[address] = temp >> 8; // split the register into 2 bytes
    address++;
    frame[address] = temp & 0xFF;
    address++;
  } 
  
  crc16 = calculateCRC(responseFrameSize - 2);
  frame[responseFrameSize - 2] = crc16 >> 8; // split crc into 2 bytes
  frame[responseFrameSize - 1] = crc16 & 0xFF;
  sendPacket(responseFrameSize);
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
  sendPacket(responseFrameSize);
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
        
        // a function_code 16 response is an echo of the first 6 bytes from the request + 2 crc bytes
        if (!broadcastFlag) // don't respond if it's a broadcast message
          sendPacket(8); 
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

unsigned int modbus_update(unsigned int *holdingRegs, bool *coils)
{
  unsigned char buffer_size = 0;

  buffer_size = process_serial_data();
  if (buffer_size == BUFFER_ERROR)
  {
    Serial.println("BUFFER_ERROR");
    return errorCount++;
  }
	
  broadcastFlag = is_broadcast_message();
  if (!destined_for_me())
  {
    Serial.println("Different Modbus ID");
    return errorCount;
  }

  // The minimum request packet is 8 bytes for function 3 & 16
  if (verify_frame_size(buffer_size) == false)
  {
    Serial.println("Packet size below minimum");
    return errorCount++;
  }

  
  function_code = frame[FUNCTION_CODE_POSITION];
  if (!verify_broadcast_and_function_code())
  {
      exceptionResponse(1); // exception 1 ILLEGAL FUNCTION
      return errorCount++;
  }

  if (!verify_crc(buffer_size))
  {
    return errorCount++;
  }

      
  Serial.print("Function Code: ");
  Serial.println(function_code, HEX);
  if (function_code == 1)
  {
    read_coils(coils);
  }
  else if (function_code == 3)
  {
    Serial.print("Function Code: ");
    Serial.println(function_code);
    read_holding_registers(holdingRegs);
  }
  else if (function_code == 6)
  {
    write_single_register(holdingRegs);
  }
  else if (function_code == 16)
  {
    write_multiple_registers(holdingRegs, buffer_size);
  }         
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
    sendPacket(5); // exception response is always 5 bytes ID, function_code + 0x80, exception code, 2 bytes crc
  }
}

void modbus_configure(
  long baud, unsigned char _slaveID, unsigned char _TxEnablePin, 
  unsigned int _holdingRegsSize, unsigned int _coilsSize)
{
  slaveID = _slaveID;
  mySerial.begin(baud);
  
  if (_TxEnablePin > 1) 
  { // pin 0 & pin 1 are reserved for RX/TX. To disable set txenpin < 2
    TxEnablePin = _TxEnablePin; 
    pinMode(TxEnablePin, OUTPUT);
    digitalWrite(TxEnablePin, LOW);
  }
  
  // Modbus states that a baud rate higher than 19200 must use a fixed 750 us 
  // for inter character time out and 1.75 ms for a frame delay.
  // For baud rates below 19200 the timeing is more critical and has to be calculated.
  // E.g. 9600 baud in a 10 bit packet is 960 characters per second
  // In milliseconds this will be 960characters per 1000ms. So for 1 character
  // 1000ms/960characters is 1.04167ms per character and finaly modbus states an
  // intercharacter must be 1.5T or 1.5 times longer than a normal character and thus
  // 1.5T = 1.04167ms * 1.5 = 1.5625ms. A frame delay is 3.5T.
  
  if (baud > 19200)
  {
    T1_5 = 150; 
    T3_5 = 350; 
  }
  else 
  {
    T1_5 = 15000000/baud; // 1T * 1.5 = T1.5
    T3_5 = 35000000/baud; // 1T * 3.5 = T3.5
  }
  
  holdingRegsSize = _holdingRegsSize;
  coilsSize = _coilsSize;
  errorCount = 0; // initialize errorCount
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

void sendPacket(unsigned char bufferSize)
{
  if (TxEnablePin > 1)
    digitalWrite(TxEnablePin, HIGH);
    
  for (unsigned char i = 0; i < bufferSize; i++)
    mySerial.write(frame[i]);
    
  mySerial.flush();
  
  // allow a frame delay to indicate end of transmission
  delayMicroseconds(T3_5); 
  
  if (TxEnablePin > 1)
    digitalWrite(TxEnablePin, LOW);
}
