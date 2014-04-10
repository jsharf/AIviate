#include "linux_i2c.h"

const char *fileName = I2C_FILENAME;

I2CBus::I2CBus()
{
    mymutex = PTHREAD_MUTEX_INITIALIZER;
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
    // wait for any pending transactions on the i2c bus to complete
    pthread_mutex_lock(&mymutex);
    // close the i2c bus
    close(fd);
    // release the i2c bus mutex
    pthread_mutex_unlock(&mymutex);
}

int I2CBus::i2c_write(char i2c_addr, const char* buf, unsigned int len)
{
    // acquire a lock on the i2c bus
    pthread_mutex_lock(&mymutex);
    unsigned int ret_len = 0;
    if(i2c_setSlave(i2c_addr))
    {
        ret_len = write(fd, buf, len);
        if(ret_len != len)
        {
            std::cerr << "Error writing to I2C slave.\n";
        }
    }
    pthread_mutex_unlock(&mymutex);
    return ret_len;
}

int I2CBus::i2c_read(char* buf, unsigned int len)
{
    pthread_mutex_lock(&mymutex);
    unsigned int ret_len = read(fd, buf, len);
    if(ret_len != len)
    {
        std::cerr << "Error reading from I2C slave.\n";
    }
    pthread_mutex_unlock(&mymutex);
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

