#ifndef MODBUS_CRC_H
#define MODBUS_CRC_H

#include <stdbool.h>

bool verify_crc(unsigned char *frame, unsigned char frame_size);
unsigned int calculate_crc(unsigned char *frame, unsigned char frame_size);

#endif

