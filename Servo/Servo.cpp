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

#include "Servo.h"

static float clamp(float value, float min, float max) {
    if(value < min) {
        return min;
    } else if(value > max) {
        return max;
    } else {
        return value;
    }
}

Servo::Servo(int pin, unsigned char sc_addr) {
	_addr = sc_addr;
    _pin = pin;
    write(0.5f);
}

void Servo::write(float percent) {
    setPos(_pin, (int)(LOWER_PW_LIMIT + PW_RANGE*percent), _addr);
    _p = clamp(percent, 0.0, 1.0);
}

float Servo::read() {
    return _p;
}

Servo& Servo::operator= (float percent) {
    write(percent);
    return *this;
}

Servo& Servo::operator= (Servo& rhs) {
    write(rhs.read());
    return *this;
}

Servo::operator float() {
    return read();
}

bool Servo::setPos(int s_num, unsigned int pos, unsigned char dev_addr)
{
    char buf[2];
    buf[0] = ((s_num<<5)&0xE0)|((pos>>8)&0x0F);
    buf[1] = pos&0xFF;
    return (I2CBus::getInstance().i2c_write(dev_addr, buf, 2)==2);    
}
