#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <ostream>
#include <pthread.h>
#include <ctime>
#include <sys/time.h>
#include "LinuxI2C/linux_i2c.h"
#include "Sensor/Sensor.h"
#include "Comm/Comm.h"
#include "Vector/Vector.h"
#include "Filters/Filters.h"
#include <iostream>
using namespace std;

#define DEBUG 1

//#define accel_w 0xA6
#define ACCEL_X 0x32 // x: 0x32,0x33 y: 0x34,0x35 z: 0x36,0x37 Little Endian!!! (x = 0x33<<8|0x22). 2's complement 16bit
#define ACCEL_POWER_CTL 0x2D
//#define gyro_w 0xD2
#define GYRO_X 0xa8 // x: 0x28,0x29 y: 0x2A,0x2B z: 0x2C,0x2D Little Endian! (x = 0x28<<8|0x22). 2's complement 16bit
#define GYRO_CTRL_REG1 0x20
//#define compass_w 0x3C
//#define compass_r 0x3D

// Just use the raw address. (No R/W bit.)

#define accel_addr 0x53
#define gyro_addr 0x69
#define compass_addr 0x1E

#define compass_x 0x03
#define compass_cra 0x00
#define compass_crb 0x01
#define compass_mode 0x02

#define ACCEL_MAGNITUDE 256
#define GYRO_MAGNITUDE (((32768.0/250)*180)/PI)

#define BMP085_ADDRESS 0x77
#define OSS 0

#define USE_PREDETERMINED_ZERO_VALS

#ifdef USE_PREDETERMINED_ZERO_VALS
const int GX_0 = 14;
const int GY_0 = -12;
const int GZ_0 = 3;
#endif

struct config
{
    int frequency;
    int accel_resolution;
};

uint64_t get_time_in_us();

char sensor_set_i2c_pointer(char addr, char reg);

int sensor_read(char addr, char reg, char *buf, int n);

int sensor_write(char addr, char reg, char *buf, int n);

int sensor_read_accelerometer(struct sensor* s);
int sensor_accelerometer_standby();
int sensor_accelerometer_measure();

int sensor_read_gyro(struct sensor* s);
int sensor_gyro_turnon();
int sensor_gyro_turnoff();

int sensor_read_compass(struct sensor *s);
int sensor_compass_setmode(void);


float bmp085GetTemperature(unsigned int ut);
long bmp085GetPressure(unsigned long up);
uint16_t bmp085ReadUT();
float calcAltitude(float pressure);
int16_t bmp085ReadInt(unsigned char address);
unsigned int bmp085ReadUP();
void* sensor_read_barometer(void *ignore);

int sensor_config_accelerometer(void);
int sensor_config_gyro();
int sensor_config_compass(void);
int sensor_config_barometer(void);

int sensor_config_gy80(struct config *c);

#endif //SENSOR_MANAGER_H
