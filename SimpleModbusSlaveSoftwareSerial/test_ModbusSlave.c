#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "SimpleModbusSlaveSoftwareSerial.h"
#include "HoldingRegisters.h"
#include "ModbusCRC.h"
#include "ModbusException.h"


int test_verify_frame_size();
int test_get_slave_id();
int test_verify_slave_id_matches();
int test_is_broadcast_message();
int test_get_function_code();
int test_verify_broadcast_and_function_code();
int test_destined_for_me();
int test_calc_crc();
int test_verify_crc();
int test_exception_response();
int test_read_holding_registers();

int test_verify_frame_size_1()
{
    unsigned char frame[1] ={ 1 };
    unsigned char frame_size = sizeof(frame);
    printf("Frame Size %d\n", frame_size);
    bool frame_size_ok = verify_frame_size(frame, frame_size);
    assert(!frame_size_ok);
    return 0;
}

int test_verify_frame_size_unknown_function_code()
{
    unsigned char frame[2] ={ 1, 2 };
    unsigned char frame_size = sizeof(frame);
    printf("Frame Size %d\n", frame_size);
    bool frame_size_ok = verify_frame_size(frame, frame_size);
    assert(!frame_size_ok);
    return 0;
}

int test_verify_frame_size_function_code_1_too_small()
{
    unsigned char frame[2] ={ 1, 1 };
    unsigned char frame_size = sizeof(frame);
    printf("Frame Size %d\n", frame_size);
    bool frame_size_ok = verify_frame_size(frame, frame_size);
    assert(!frame_size_ok);
    return 0;
}

int test_verify_frame_size_function_code_15_too_small()
{
    unsigned char frame[2] ={ 1, 15 };
    unsigned char frame_size = sizeof(frame);
    printf("Frame Size %d\n", frame_size);
    bool frame_size_ok = verify_frame_size(frame, frame_size);
    assert(!frame_size_ok);
    return 0;
}

int test_verify_frame_size_function_code_16_too_small()
{
    unsigned char frame[2] ={ 1, 16 };
    unsigned char frame_size = sizeof(frame);
    printf("Frame Size %d\n", frame_size);
    bool frame_size_ok = verify_frame_size(frame, frame_size);
    assert(!frame_size_ok);
    return 0;
}

int test_verify_frame_size_function_code_1_equal_8()
{
    unsigned char frame[8] ={ 1, 1, 2, 3, 4, 5, 6, 7 };
    unsigned char frame_size = sizeof(frame);
    printf("Frame Size %d\n", frame_size);
    bool frame_size_ok = verify_frame_size(frame, frame_size);
    assert(frame_size_ok);
    return 0;
}

int test_verify_frame_size_function_code_3_equal_8()
{
    unsigned char frame[8] ={ 1, 3, 2, 3, 4, 5, 6, 7 };
    unsigned char frame_size = sizeof(frame);
    printf("Frame Size %d\n", frame_size);
    bool frame_size_ok = verify_frame_size(frame, frame_size);
    assert(frame_size_ok);
    return 0;
}

int test_verify_frame_size_function_code_5_equal_8()
{
    unsigned char frame[8] ={ 1, 3, 2, 3, 4, 5, 6, 7 };
    unsigned char frame_size = sizeof(frame);
    printf("Frame Size %d\n", frame_size);
    bool frame_size_ok = verify_frame_size(frame, frame_size);
    assert(frame_size_ok);
    return 0;
}

int test_verify_frame_size_function_code_6_equal_8()
{
    unsigned char frame[8] ={ 1, 3, 2, 3, 4, 5, 6, 7 };
    unsigned char frame_size = sizeof(frame);
    printf("Frame Size %d\n", frame_size);
    bool frame_size_ok = verify_frame_size(frame, frame_size);
    assert(frame_size_ok);
    return 0;
}

int test_verify_frame_size_function_code_15_equal_10()
{
    unsigned char frame[10] ={ 1, 15, 2, 3, 4, 5, 6, 7, 8, 9 };
    unsigned char frame_size = sizeof(frame);
    printf("Frame Size %d\n", frame_size);
    bool frame_size_ok = verify_frame_size(frame, frame_size);
    assert(frame_size_ok);
    return 0;
}

int test_verify_frame_size_function_code_16_equal_10()
{
    unsigned char frame[10] ={ 1, 15, 2, 3, 4, 5, 6, 7, 8, 9 };
    unsigned char frame_size = sizeof(frame);
    printf("Frame Size %d\n", frame_size);
    bool frame_size_ok = verify_frame_size(frame, frame_size);
    assert(frame_size_ok);
    return 0;
}

int test_get_slave_id_0()
{
    unsigned char frame[1] ={ 0 };
    unsigned char frame_size = sizeof(frame);
    unsigned char slave_id = get_slave_id(frame, frame_size);
    printf("Slave ID: %d\n", slave_id);
    assert(slave_id == 0);
    return 0;
}

int test_get_slave_id_1()
{
    unsigned char frame[1] ={ 1 };
    unsigned char frame_size = sizeof(frame);
    unsigned char slave_id = get_slave_id(frame, frame_size);
    printf("Slave ID: %d\n", slave_id);
    assert(slave_id == 1);
    return 0;
}

int test_get_slave_id_247()
{
    unsigned char frame[1] ={ 247 };
    unsigned char frame_size = sizeof(frame);
    unsigned char slave_id = get_slave_id(frame, frame_size);
    printf("Slave ID: %d\n", slave_id);
    assert(slave_id == 247);
    return 0;
}

int test_get_function_code_1()
{
    unsigned char frame[2] ={ 247, 1 };
    unsigned char frame_size = sizeof(frame);
    unsigned char result = get_function_code(frame, frame_size);
    printf("READ_COILS: %d\n", result);
    assert(result == 1);
    return 0;
}

int test_get_function_code_3()
{
    unsigned char frame[2] ={ 247, 3 };
    unsigned char frame_size = sizeof(frame);
    unsigned char result = get_function_code(frame, frame_size);
    printf("READ_HOLDING_REGISTERS: %d\n", result);
    assert(result == 3);
    return 0;
}

int test_get_function_code_5()
{
    unsigned char frame[2] ={ 247, 5 };
    unsigned char frame_size = sizeof(frame);
    unsigned char result = get_function_code(frame, frame_size);
    printf("WRITE_SINGLE_COIL: %d\n", result);
    assert(result == 5);
    return 0;
}

int test_get_function_code_6()
{
    unsigned char frame[2] ={ 247, 6 };
    unsigned char frame_size = sizeof(frame);
    unsigned char result = get_function_code(frame, frame_size);
    printf("WRITE_SINGLE_HOLDING_REGISTER: %d\n", result);
    assert(result == 6);
    return 0;
}

int test_get_function_code_15()
{
    unsigned char frame[2] ={ 247, 15 };
    unsigned char frame_size = sizeof(frame);
    unsigned char result = get_function_code(frame, frame_size);
    printf("WRITE_MULTIPLE_COILS: %d\n", result);
    assert(result == 15);
    return 0;
}

int test_get_function_code_16()
{
    unsigned char frame[2] ={ 247, 16 };
    unsigned char frame_size = sizeof(frame);
    unsigned char result = get_function_code(frame, frame_size);
    printf("WRITE_MULTIPLE_HOLDING_REGISTERS: %d\n", result);
    assert(result == 16);
    return 0;
}

int test_verify_slave_id_matches_true()
{
    unsigned char frame[1] ={ 247 };
    unsigned char frame_size = sizeof(frame);
    bool result = verify_slave_id_matches(frame, frame_size, 247);
    printf("Slave ID Matches 247: %s\n", result ? "true" : "false");
    assert(result);
    return 0;
}

int test_verify_slave_id_matches_false()
{
    unsigned char frame[1] ={ 1 };
    unsigned char frame_size = sizeof(frame);
    bool result = verify_slave_id_matches(frame, frame_size, 247);
    printf("Slave ID Matches 247: %s\n", result ? "true" : "false");
    assert(!result);
    return 0;
}

int test_is_broadcast_message_true()
{
    unsigned char frame[1] ={ 0 };
    unsigned char frame_size = sizeof(frame);
    bool result = is_broadcast_message(frame, frame_size);
    printf("Broadcast Message: %s\n", result ? "true" : "false");
    assert(result);
    return 0;
}

int test_is_broadcast_message_false()
{
    unsigned char frame[1] ={ 1 };
    unsigned char frame_size = sizeof(frame);
    bool result = is_broadcast_message(frame, frame_size);
    printf("Broadcast Message: %s\n", result ? "true" : "false");
    assert(!result);
    return 0;
}

int test_verify_broadcast_and_function_code_broadcast_and_read_coils()
{
    unsigned char frame[2] ={ 0, 1 };
    unsigned char frame_size = sizeof(frame);
    bool result = verify_broadcast_and_function_code(frame, frame_size);
    printf("Broadcast and READ_COILS: %s\n", result ? "true" : "false");
    assert(!result);
    return 0;
}

int test_verify_broadcast_and_function_code_broadcast_and_read_holding_registers()
{
    unsigned char frame[2] ={ 0, 3 };
    unsigned char frame_size = sizeof(frame);
    bool result = verify_broadcast_and_function_code(frame, frame_size);
    printf("Broadcast and READ_HOLDING_REGISTERS: %s\n", result ? "true" : "false");
    assert(!result);
    return 0;
}

int test_verify_broadcast_and_function_code_broadcast_and_write_coil()
{
    unsigned char frame[2] ={ 0, 5 };
    unsigned char frame_size = sizeof(frame);
    bool result = verify_broadcast_and_function_code(frame, frame_size);
    printf("Broadcast and WRITE_SINGLE_COIL: %s\n", result ? "true" : "false");
    assert(result);
    return 0;
}

int test_verify_broadcast_and_function_code_broadcast_and_write_holding_register()
{
    unsigned char frame[2] ={ 0, 6 };
    unsigned char frame_size = sizeof(frame);
    bool result = verify_broadcast_and_function_code(frame, frame_size);
    printf("Broadcast WRITE_SINGLE_HOLDING_REGISTER: %s\n", result ? "true" : "false");
    assert(result);
    return 0;
}

int test_verify_broadcast_and_function_code_broadcast_and_write_coils()
{
    unsigned char frame[2] ={ 0, 15 };
    unsigned char frame_size = sizeof(frame);
    bool result = verify_broadcast_and_function_code(frame, frame_size);
    printf("Broadcast WRITE_MUTLTIPLE_COILS: %s\n", result ? "true" : "false");
    assert(result);
    return 0;
}

int test_verify_broadcast_and_function_code_broadcast_and_write_holding_registers()
{
    unsigned char frame[2] ={ 0, 16 };
    unsigned char frame_size = sizeof(frame);
    bool result = verify_broadcast_and_function_code(frame, frame_size);
    printf("Broadcast WRITE_MULTIPLE_HOLDING_REGISTERS: %s\n", result ? "true" : "false");
    assert(result);
    return 0;
}

int test_verify_broadcast_and_function_code_not_broadcast()
{
    unsigned char frame[2] ={ 2, 3 };
    unsigned char frame_size = sizeof(frame);
    bool result = verify_broadcast_and_function_code(frame, frame_size);
    printf("Not Broadcast: %s\n", result ? "true" : "false");
    assert(result);
    return 0;
}

int test_destined_for_me_broadcast()
{
    unsigned char frame[1] ={ 0 };
    unsigned char frame_size = sizeof(frame);
    bool result = destined_for_me(frame, frame_size, 2);
    printf("Destined for me (Broadcast): %s\n", result ? "true" : "false");
    assert(result);
    return 0;
}

int test_destined_for_me_id_match()
{
    unsigned char frame[1] ={ 2 };
    unsigned char frame_size = sizeof(frame);
    bool result = destined_for_me(frame, frame_size, 2);
    printf("Destined for me (ID match of 2): %s\n", result ? "true" : "false");
    assert(result);
    return 0;
}

int test_destined_for_me_id_mismatch()
{
    unsigned char frame[1] ={ 1 };
    unsigned char frame_size = sizeof(frame);
    bool result = destined_for_me(frame, frame_size, 2);
    printf("Destined for me (ID mismatch 1 != 2): %s\n", result ? "true" : "false");
    assert(!result);
    return 0;
}

int test_calc_crc_read_coils()
{
    unsigned char frame[6] ={ 1, 1, 0, 0, 0, 1 };
    unsigned char frame_size = sizeof(frame);
    unsigned int crc = calculate_crc(frame, frame_size);
    printf("Coils CRC: 0x%x\n", crc);
    assert(crc == 0xfdca);
    return 0;
}

int test_calc_crc_read_holding_registers()
{
    unsigned char frame[6] ={ 1, 3, 0, 0, 0, 1 };
    unsigned char frame_size = sizeof(frame);
    unsigned int crc = calculate_crc(frame, frame_size);
    printf("Holding Register CRC: 0x%x\n", crc);
    assert(crc == 0x840a);
    return 0;
}

int test_verify_crc_read_coils()
{
    unsigned char frame[8] ={ 1, 1, 0, 0, 0, 1, 0xfd, 0xca };
    unsigned char frame_size = sizeof(frame);
    bool result = verify_crc(frame, frame_size);
    printf("Coils CRC verified: %s\n", result ? "true" : "false");
    assert(result);
    return 0;
}

int test_verify_crc_read_holding_registers()
{
    unsigned char frame[8] ={ 1, 3, 0, 0, 0, 1, 0x84, 0x0a };
    unsigned char frame_size = sizeof(frame);
    bool result = verify_crc(frame, frame_size);
    printf("Holding Register CRC verified: %s\n", result ? "true" : "false");
    assert(result);
    return 0;
}

int test_exception_response_2()
{
    unsigned char *response;
    response = illegal_data_address_exception(1, 3);  
    printf("Illegal Data Address Response: 0x%x, 0x%x, 0x%x\n", response[0], response[1], response[2]);
    assert(response[0] == 1 && response[1] == 0x83 && response[2] == 2);
    free(response);
    return 0;
}

int test_read_holding_register_starting_address_larger_than_number_of_holding_registers()
{
    unsigned int holding_registers[2] = { 23, 65535 };
    unsigned char holding_registers_count = sizeof(holding_registers) / sizeof(int);
    unsigned char *response;
    response = read_holding_registers(1, 3, 1, holding_registers, holding_registers_count);
    printf("Starting address outside of the holding register address range: 0x%x, 0x%x, 0x%x\n", response[0], response[1], response[2]);
    assert(response[0] == 1 && response[1] == 0x83 && response[2] == 2);
    free(response);
    return 0;
}


int test_read_holding_register_too_many_registers_requested()
{
    unsigned int holding_registers[2] = { 23, 65535 };
    unsigned char holding_registers_count = sizeof(holding_registers) / sizeof(int);
    unsigned char *response;
    response = read_holding_registers(1, 0, 3, holding_registers, holding_registers_count);
    printf("Too Many Holding Registers Requested Response: 0x%x, 0x%x, 0x%x\n", response[0], response[1], response[2]);
    assert(response[0] == 1 && response[1] == 0x83 && response[2] == 2);
    free(response);
    return 0;
}

int main(int argc, char *argv[])
{
    printf("Running test_ModbusSlave.exe\n");
    test_verify_frame_size();
    test_get_slave_id();
    test_verify_slave_id_matches();
    test_is_broadcast_message();
    test_get_function_code();
    test_verify_broadcast_and_function_code();
    test_destined_for_me();
    test_calc_crc();
    test_verify_crc();
    test_exception_response();
    test_read_holding_registers();

    return 0;
}

int test_verify_frame_size()
{
    test_verify_frame_size_1();
    test_verify_frame_size_unknown_function_code();
    test_verify_frame_size_function_code_1_too_small();
    test_verify_frame_size_function_code_1_equal_8();
    test_verify_frame_size_function_code_3_equal_8();
    test_verify_frame_size_function_code_5_equal_8();
    test_verify_frame_size_function_code_6_equal_8();
    test_verify_frame_size_function_code_15_too_small();
    test_verify_frame_size_function_code_15_equal_10();
    test_verify_frame_size_function_code_16_too_small();
    test_verify_frame_size_function_code_16_equal_10();
    return 0;
}

int test_get_slave_id()
{
    test_get_slave_id_0();
    test_get_slave_id_1();
    test_get_slave_id_247();
    return 0;
}

int test_get_function_code()
{
    printf("Testing get_function_code()\n");
    test_get_function_code_1();
    test_get_function_code_3();
    test_get_function_code_5();
    test_get_function_code_6();
    test_get_function_code_15();
    test_get_function_code_16();
    return 0;
}

int test_verify_broadcast_and_function_code()
{
    test_verify_broadcast_and_function_code_not_broadcast();
    test_verify_broadcast_and_function_code_broadcast_and_read_coils();
    test_verify_broadcast_and_function_code_broadcast_and_read_holding_registers();
    test_verify_broadcast_and_function_code_broadcast_and_write_coil();
    test_verify_broadcast_and_function_code_broadcast_and_write_coils();
    test_verify_broadcast_and_function_code_broadcast_and_write_holding_register();
    test_verify_broadcast_and_function_code_broadcast_and_write_holding_registers();
    return 0;
}

int test_verify_slave_id_matches()
{
    test_verify_slave_id_matches_true();
    test_verify_slave_id_matches_false();
    return 0;
}

int test_is_broadcast_message()
{
    test_is_broadcast_message_true();
    test_is_broadcast_message_false();
    return 0;
}

int test_destined_for_me()
{
    test_destined_for_me_broadcast();
    test_destined_for_me_id_match();
    test_destined_for_me_id_mismatch();
    return 0;
}

int test_calc_crc()
{
    test_calc_crc_read_coils();
    test_calc_crc_read_holding_registers();
    return 0;
}

int test_verify_crc()
{
    test_verify_crc_read_coils();
    test_verify_crc_read_holding_registers();
    return 0;
}

int test_read_holding_registers()
{
    test_read_holding_register_starting_address_larger_than_number_of_holding_registers();
    test_read_holding_register_too_many_registers_requested();
    return 0;
}

int test_exception_response()
{
    test_exception_response_2();
    return 0;
}
