#include "linux_i2c.h"

I2CBus::I2CBus()
{
    if((fd = open(fileName, O_RDWR)) < 0)
    {
        std::cerr << "Failed to open I2C bus for read/write!\n";
    }
    else
    {
        std::cerr << "I2C bus opened successfully.\n";
    }
}

int I2CBus::write(char i2c_addr, const char* buf, unsigned int len)
{
    int ret_len = 0;
    if(setSlave(i2c_addr))
    {
        ret_len = write(fd, buf, len);
        if(ret_len != len)
        {
            std::cerr << "Error writing to I2C slave.\n";
        }
    }
    return ret_len;
}

int I2CBus::read(char i2c_addr, char* buf, unsigned int len)
{
    int ret_len = 0;
    if(setSlave(i2c_addr))
    {
        ret_len = read(fd, buf, len);
        if(ret_len != len)
        {
            std::cerr << "Error reading from I2C slave.\n";
        }
    }
    return ret_len;
}

bool I2CBus::setSlave(char i2c_addr)
{
    if((ioctl(fd, I2C_SLAVE, address)) < 0)
    {
        std::cerr << "Unable to get bus access to talk to slave\n";
    }
}

