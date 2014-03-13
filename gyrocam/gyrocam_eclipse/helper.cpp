#include "helper.h"


int writeI2C(int device, byte address, int num, byte buff[]) {
    int eCode = 0;

    for (int i=0; i<num; i++) {
        eCode |= I2c.write(device, (int)(address) + i, (int)buff[i]);
    }

    return eCode;
}

int readI2C(int device, byte address, int num, byte buff[]) {
    I2c.read(device, (int)address, num);

    for (int i=0; i<num; i++)
    {
        buff[i] = I2c.receive();   // receive a uint8_t
        i++;
    }

    return 0;
}
