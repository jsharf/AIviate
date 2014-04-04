#ifndef SENSOR_H
#define SENSOR_H

#include <iostream>
#include <ostream>

struct sensor
{
    int16_t ax, ay, az;
    int16_t gx, gy, gz;
    int16_t mx, my, mz;
    int16_t altitude;
    char raw_data[6];
};

struct sensorf
{
    float ax, ay, az;
    float gx, gy, gz;
    float mx, my, mz;
    float altitude;
    friend std::ostream& operator<<(std::ostream &out, sensorf &rhs);
};

#endif //SENSOR_H
