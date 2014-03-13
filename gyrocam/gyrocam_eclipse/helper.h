/*! \file i2c.h
 *  \author Soo-Hyun Yoo
 *  \brief Functions to make I2C communication easier.
 *
 *  Details.
 */

#ifndef I2C_H
#define I2C_H

#include "I2C.h"

#define sp Serial.print
#define spln Serial.println
#define spn Serial.println
#define sw Serial.write


int writeI2C(int device, byte address, int num, byte buff[]);
int readI2C(int device, byte address, int num, byte buff[]);

#endif // I2C_H

