#ifndef LINUX_I2C_H
#define LINUX_I2C_H

#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>

#define I2C_FILENAME "/dev/i2c-0"

class I2CBus
{
public:
    static I2CBus& getInstance()
    {
        static I2CBus instance;
        return instance;
    }
    int write(char i2c_addr, const char * buf, unsigned int len);
    int read(char i2c_addr, char * buf, unsigned int len);
private:
    I2CBus();

    bool setSlave(char i2c_addr);

    I2CBus(I2CBus const&);
    void operator=(I2CBus const&);

    int fd;
    const char *fileName = I2C_FILENAME;
};

#endif // LINUX_I2C_H
