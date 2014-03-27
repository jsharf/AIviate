#include "linux_i2c.h"

const char *fileName = I2C_FILENAME;

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

I2CBus::~I2CBus()
{
	close(fd);
}

int I2CBus::i2c_write(char i2c_addr, const char* buf, unsigned int len)
{
    int ret_len = 0;
    if(i2c_setSlave(i2c_addr))
    {
        ret_len = write(fd, buf, len);
        if(ret_len != len)
        {
            std::cerr << "Error writing to I2C slave.\n";
        }
    }
    return ret_len;
}

int I2CBus::i2c_read(char i2c_addr, char* buf, unsigned int len)
{
    int ret_len = 0;
    if(i2c_setSlave(i2c_addr))
    {
        ret_len = read(fd, buf, len);
        if(ret_len != len)
        {
            std::cerr << "Error reading from I2C slave.\n";
        }
    }
    return ret_len;
}

bool I2CBus::i2c_setSlave(char i2c_addr)
{
    if(ioctl(fd, I2C_SLAVE, (int)i2c_addr) < 0)
    {
        std::cerr << "Unable to get bus access to talk to slave\n";
        return false;
    }
    return true;
}

