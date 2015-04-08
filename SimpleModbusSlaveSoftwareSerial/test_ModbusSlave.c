#include <assert.h>
#include <stdio.h>
#include "SimpleModbusSlaveSoftwareSerial.h"


int test_get_slave_id()
{
    unsigned char frame[1] ={ 1 };
    unsigned char slave_id = get_slave_id(frame);
    assert(slave_id == 1);
    return 0;
}

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

int test_verify_frame_size_function_code_1_equal_8()
{
    unsigned char frame[8] ={ 1, 1, 2, 3, 4, 5, 6, 7 };
    unsigned char frame_size = sizeof(frame);
    printf("Frame Size %d\n", frame_size);
    bool frame_size_ok = verify_frame_size(frame, frame_size);
    assert(frame_size_ok);
    return 0;
}

int test_verify_frame_size()
{
    test_verify_frame_size_1();
    test_verify_frame_size_unknown_function_code();
    test_verify_frame_size_function_code_1_too_small();
    test_verify_frame_size_function_code_1_equal_8();
    return 0;
}

int main(int argc, char *argv[])
{
    printf("Running test_ModbusSlave.exe\n");
    test_get_slave_id();
    test_verify_frame_size();

    return 0;
}
