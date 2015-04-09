#include <assert.h>
#include <stdio.h>
#include "SimpleModbusSlaveSoftwareSerial.h"


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

int test_verify_slave_id_matches_true()
{
    unsigned char frame[1] ={ 247 };
    unsigned char frame_size = sizeof(frame);
    bool result = verify_slave_id_matches(frame, frame_size, 247);
    printf("Slave ID Matches 247: %d\n", result);
    assert(result);
    return 0;
}

int test_verify_slave_id_matches_false()
{
    unsigned char frame[1] ={ 1 };
    unsigned char frame_size = sizeof(frame);
    bool result = verify_slave_id_matches(frame, frame_size, 247);
    printf("Slave ID Matches 247: %d\n", result);
    assert(!result);
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

int test_verify_slave_id_matches()
{
    test_verify_slave_id_matches_true();
    test_verify_slave_id_matches_false();
}

int main(int argc, char *argv[])
{
    printf("Running test_ModbusSlave.exe\n");
    test_verify_frame_size();
    test_get_slave_id();
    test_verify_slave_id_matches();

    return 0;
}
