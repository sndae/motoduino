/*! \file i2c.h
 *  \author Soo-Hyun Yoo
 *  \brief Functions to make I2C communication easier.
 *
 *  Details.
 */

#ifndef I2C_H
#define I2C_H

#include "I2C.h"

int writeI2C(int device, byte address, int num, byte buff[]) {
    int eCode;

    for (int i=0; i<num; i++) {
        eCode |= I2c.write(device, (int)(address + i), (int)buff[i]);
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

#endif // I2C_H

