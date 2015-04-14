#ifndef HOLDING_REGISTERS_H
#define HOLDING_REGISTERS_H

unsigned char *read_holding_registers(unsigned char slave_id, unsigned int starting_address, unsigned int number_of_registers, unsigned int *holding_registers, unsigned int holding_registers_size);

struct modbus_response {
    unsigned char slave_id;
    unsigned char function_code;
    unsigned int *registers;
    unsigned int crc[2];
};
#endif

