#include <stdlib.h>
#include "ModbusException.h"
#include "ModbusCRC.h"

unsigned int error_count = 0;
const struct MODBUS_EXCEPTION_CODES MODBUS_EXCEPTIONS = {1, 2, 3, 4, 5, 6, 8, 0xa, 0xb};


unsigned char *exception_response(unsigned char slave_id, unsigned char function_code, unsigned char exception)
{
    const unsigned char RESPONSE_FRAME_SIZE = 0x3;
    error_count++; // each call to exceptionResponse() will increment the errorCount
    unsigned char *response = malloc(5 * sizeof(unsigned char));
    response[0] = slave_id;
    response[1] = (function_code | 0x80); // set the MSB bit high, informs the master of an exception
    response[2] = exception;
    unsigned int crc16 = calculate_crc(response, RESPONSE_FRAME_SIZE);
    response[3] = crc16 >> 8;
    response[4] = crc16 & 0xFF;
    return response;
}

unsigned char *illegal_data_address_exception(unsigned char slave_id, unsigned char function_code)
{
    return exception_response(slave_id, function_code, MODBUS_EXCEPTIONS.ILLEGAL_DATA_ADDRESS);
}
