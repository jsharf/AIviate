/* RasPi R/C Servo Library
 *
 * Copyright (c) 2007-2014 sford, cstyles, fughilli
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef MBED_SERVO_H
#define MBED_SERVO_H

#include "linux_i2c.h"

    const int UPPER_PW_LIMIT = 928;
    const int LOWER_PW_LIMIT = 0;
    const int CENTER_PW = (UPPER_PW_LIMIT+LOWER_PW_LIMIT)/2;
    const int PW_RANGE = UPPER_PW_LIMIT-LOWER_PW_LIMIT;

class Servo {

public:
    /** Create a servo object connected to the specified pin on the specified i2c servo controller
     *
     * @param pin on the controller to connect to
     */
    Servo(int pin, unsigned char sc_addr = 0x39);

    /** Set the servo position, normalised to its full range
     *
     * @param percent A normalised number 0.0-1.0 to represent the position within the  full range.
     */
    void write(float percent);

    /**  Read the servo motor's current position
     *
     * @param returns a normalised number 0.0-1.0 representing the position within the full range.
     */
    float read();

    /**  Shorthand for the write and read functions */
    Servo& operator= (float percent);
    Servo& operator= (Servo& rhs);
    operator float();

protected:
    unsigned char _addr;
    int _pin;
    float _range;
    float _degrees;
    float _p;
    bool setPos(int s_num, unsigned int pos, unsigned char dev_addr);
};

#endif
