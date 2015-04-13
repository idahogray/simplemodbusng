#include "ModbusCRC.h"

unsigned int calculate_crc(unsigned char *frame, unsigned char frame_size) 
{
  unsigned int temp, temp2, flag;
  temp = 0xFFFF;
  for (unsigned char i = 0; i < frame_size; i++)
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
  return temp; // the returned value is already swapped - crcLo byte is first & crcHi byte is last
}

bool verify_crc(unsigned char *frame, unsigned char frame_size)
{
  unsigned int crc = ((frame[frame_size - 2] << 8) | frame[frame_size - 1]);
  return calculate_crc(frame, frame_size - 2) == crc;
}

