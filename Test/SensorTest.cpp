//#include "LinuxI2C/linux_i2c.h"//#include "Servo/Servo.h"#include "sensor.h"#include <iostream>#include <ctime>int main(void){    sensor sens;    std::cout << sensor_config_gy80(nullptr) << std::endl;    while(1)    {        sensor_read_accelerometer(sens);        sensor_read_gyro(sens);        std::cout <<    sens.ax << ", " << sens.ay << ", " << sens.az << ", " <<                  sens.gx << ", " << sens.gy << ", " << sens.gz << std::endl;        sleep(1);    }    return 0;}