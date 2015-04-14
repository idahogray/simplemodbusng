extern unsigned int error_count;
struct MODBUS_EXCEPTION_CODES {
    unsigned char ILLEGAL_FUNCTION;
    unsigned char ILLEGAL_DATA_ADDRESS;
    unsigned char ILLEGAL_DATA_VALUE;
    unsigned char SERVER_DEVICE_FAILURE;
    unsigned char ACKNOWLEDGE;
    unsigned char SERVER_DEVICE_BUSY;
    unsigned char MEMORY_PARITY_ERROR;
    unsigned char GATEWAY_PATH_UNAVAILABLE;
    unsigned char GATEWAY_TARGET_DEVICE_FAILED_TO_RESPOND;
};
extern const struct MODBUS_EXCEPTION_CODES MODBUS_EXCEPTIONS;

unsigned char *exception_response(unsigned char slave_id, unsigned char function_code, unsigned char exception);
unsigned char *illegal_data_address_exception(unsigned char slave_id, unsigned char function_code);
