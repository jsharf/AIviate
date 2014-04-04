#include "sensor.h"
#include "Comm.cpp"

void usage()
{
    cerr << "sensor <out_addr> <out_port>" << endl;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cerr << "Wrong number of arguments" << endl;
        usage();
        return 1;
    }
    int sensor_config_ret = sensor_config_gy80(NULL);
    if(sensor_config_ret != 0xF)
    {
        cerr << "GY80 module failed initialization with code " << sensor_config_ret << endl;
    }
    UDPSender snd(argv[1], argv[2]);
    sensor out_data;
    while (true)
    {
        // Get sensor data from I2C
        sensor_read_accelerometer(&out_data);
        sensor_read_gyro(&out_data);
        sensor_read_compass(&out_data);
        // Send sensor data over UDP to target
        snd.sendSensor(out_data);
    }
}

// Helps debugging with sensor structs
ostream& operator<<(ostream &out, sensorf &rhs)
{
    out << "SENSORF: \n{\n\t ax: " \
    << rhs.ax << "\n\t ay: " << rhs.ay << "\n\t az: " << rhs.az \
    << "\n\t gx: " << rhs.gx << "\n\t gy: " << rhs.gy << "\n\t gz: " << rhs.gz \
    << "\n\t mx: " << rhs.mx << "\n\t my: " << rhs.my << "\n\t mz: " << rhs.mz \
    << "\n\t temp: " << rhs.temp << "\n\t pressure: " << rhs.pressure \
    << "\n}" << endl;
    return out;
}

int sensor_read(char addr, char reg, char *buf, int n)
{
    if (I2CBus::getInstance().i2c_write(addr, &reg, 1) != 1)
    {
        if (DEBUG)
            std::cerr << "Could not set register address (set_i2c_pointer)" << std::endl;
        return 0;
    }
    int ret = I2CBus::getInstance().i2c_read(buf, n);
    if (ret != n)
    {
        if (DEBUG)
            std::cerr << "I2C read function failed in sensor_read" << std::endl;
        return 0;
    }
    return n;
}

int sensor_write(char addr, char reg, char *buf, int n)
{
    char buf2[n+1];
    memcpy(buf2+1, buf, n);
    buf2[0] = reg;
    int i = I2CBus::getInstance().i2c_write(addr, buf2, n+1);
    if (i != (n+1))
    {
        if (DEBUG)
            std::cerr << "Only sent " << i << "/" << n << " bytes (write)" << std::endl;
        return i;
    }
    return n;
}

int sensor_read_accelerometer(struct sensor* s)
{
    int ret = sensor_read(accel_addr, ACCEL_X, s->raw_data, 6);
    if (ret != 6)
    {
        std::cerr << "Error, could not read (read_accelerometer)" << std::endl;
        return 0;
    }
    int16_t axlsb = (int16_t) s->raw_data[0];
    int16_t axmsb = (int16_t) s->raw_data[1];
    int16_t aylsb = (int16_t) s->raw_data[2];
    int16_t aymsb = (int16_t) s->raw_data[3];
    int16_t azlsb = (int16_t) s->raw_data[4];
    int16_t azmsb = (int16_t) s->raw_data[5];
    s->ax = ((axmsb << 8) + axlsb);
    s->ay = ((aymsb << 8) + aylsb);
    s->az = ((azmsb << 8) + azlsb);
    return 1;
}

// disable accelerometer to save power
int sensor_accelerometer_standby()
{
    char power_ctl;
    int ret = sensor_read(accel_addr, ACCEL_POWER_CTL, &power_ctl, 1);
    if (ret != 1)
    {
        if (DEBUG)
            std::cerr << "Error putting accelerometer in standby (accelerometer_standby)" << std::endl;
        return 0;
    }
    power_ctl &= 0xF7 ;
    ret = sensor_write(accel_addr, ACCEL_POWER_CTL, &power_ctl, 1);
    if (ret != 1)
    {
        if (DEBUG)
            std::cerr << "Error putting accelerometer in standby (accelerometer_standby)" << std::endl;
        return 0;
    }
    return 1;
}

// enable accelerometer for measurements
int sensor_accelerometer_measure()
{
    char power_ctl;
    int ret = sensor_read(accel_addr, ACCEL_POWER_CTL, &power_ctl, 1);
    if (ret != 1)
    {
        if (DEBUG)
            std::cerr << "Error putting accelerometer in measure mode (accelerometer_measure)" << std::endl;
        return 0;
    }
    power_ctl |= 0x8;
    ret = sensor_write(accel_addr, ACCEL_POWER_CTL, &power_ctl, 1);
    if (ret != 1)
    {
        if (DEBUG)
            std::cerr << "Error putting accelerometer in measure mode (accelerometer_measure)" << std::endl;
        return 0;
    }
    return 1;
}

int sensor_gyro_turnon()
{
    char power_ctl;
    int ret = sensor_read(gyro_addr, GYRO_CTRL_REG1, &power_ctl, 1);
    if (DEBUG)
        std::cerr << "Gyro REG1 read: " << std::hex << power_ctl << std::dec << std::endl;
    if (ret != 1)
    {
        if (DEBUG)
            std::cerr << "Error turning on gyro (gyro_turnon)" << std::endl;
        return 0;
    }
    power_ctl |= 0x8;
    if (DEBUG)
        std::cerr << "Gyro REG1 write: " << std::hex << power_ctl << std::dec << std::endl;
    ret = sensor_write(gyro_addr, GYRO_CTRL_REG1, &power_ctl, 1);
    if (ret != 1)
    {
        if (DEBUG)
            std::cerr << "Error turning on gyro (gyro_turnon)" << std::endl;
        return 0;
    }
    return 1;
}

int sensor_gyro_turnoff()
{
    char power_ctl;
    int ret = sensor_read(gyro_addr, GYRO_CTRL_REG1, &power_ctl, 1);
    if (ret != 1)
    {
        if (DEBUG)
            std::cerr << "Error turning off gyro (gyro_turnoff)" << std::endl;
        return 0;
    }
    power_ctl &= 0xF7 ;
    ret = sensor_write(gyro_addr, GYRO_CTRL_REG1, &power_ctl, 1);
    if (ret != 1)
    {
        if (DEBUG)
            std::cerr << "Error turning off gyro (gyro_turnoff)" << std::endl;
        return 0;
    }
    return 1;
}

int sensor_read_gyro(struct sensor* s)
{
    int ret = sensor_read(gyro_addr, GYRO_X, s->raw_data, 6);
    if (ret != 6)
    {
        std::cerr << "Error, could not read (sensor_read_gyro)" << std::endl;
        return 0;
    }
    int16_t gxlsb = (int16_t) s->raw_data[0];
    int16_t gxmsb = (int16_t) s->raw_data[1];
    int16_t gylsb = (int16_t) s->raw_data[2];
    int16_t gymsb = (int16_t) s->raw_data[3];
    int16_t gzlsb = (int16_t) s->raw_data[4];
    int16_t gzmsb = (int16_t) s->raw_data[5];
    #ifdef USE_PREDETERMINED_ZERO_VALS
        s->gx = (int16_t)((gxmsb << 8) | gxlsb) - GX_0;
        s->gy = (int16_t)((gymsb << 8) | gylsb) - GY_0;
        s->gz = (int16_t)((gzmsb << 8) | gzlsb) - GZ_0;
    #else
        s->gx = (int16_t)((gxmsb << 8) | gxlsb) - s->gx0;
        s->gy = (int16_t)((gymsb << 8) | gylsb) - s->gy0;
        s->gz = (int16_t)((gzmsb << 8) | gzlsb) - s->gz0;
    #endif
    return 1;
}

int sensor_read_compass(struct sensor* s)
{
    int ret = sensor_read(compass_addr, compass_x, s->raw_data, 6);
    if (ret != 6)
    {
        std::cerr << "Error, could not read (read_compass)" << std::endl;
        return 0;
    }
    int16_t mxmsb = (int16_t) s->raw_data[0];
    int16_t mxlsb = (int16_t) s->raw_data[1];
    int16_t mymsb = (int16_t) s->raw_data[2];
    int16_t mylsb = (int16_t) s->raw_data[3];
    int16_t mzmsb = (int16_t) s->raw_data[4];
    int16_t mzlsb = (int16_t) s->raw_data[5];
    s->mx = ((mxmsb << 8) + mxlsb);
    s->my = ((mymsb << 8) + mylsb);
    s->mz = ((mzmsb << 8) + mzlsb);
    return 1;
}

int sensor_read_barometer(struct sensor* s)
{
    //int ret = sensor_read(barometer_w, bar
    return 0;
}

int sensor_config_accelerometer(void)
{
    // take accelerometer out of standby mode
    int ret = sensor_accelerometer_measure();
    if (ret == 0)
    {
        if (DEBUG)
            std::cerr << "Error starting up accelerometer" << std::endl;
        return 0;
    }
    return 1<<3;
}
int sensor_config_gyro()
{
    // turn on the gyro via i2c
    int ret = sensor_gyro_turnon();
    if (ret == 0)
    {
        if (DEBUG)
            std::cerr << "Error starting up gyro" << std::endl;
        return 0;
    }
    return 1<<2;
}

int sensor_compass_setmode(void)
{
    char mode = 0;
    int ret = sensor_write(compass_addr, compass_mode, &mode, 1);
    if (ret == 0)
    {
        if (DEBUG)
            std::cerr << "Error setting compass to continuous measurement mode" << std::endl;
        return 0;
    }

    char cra = 0x18;
    ret = sensor_write(compass_addr, compass_cra, &cra, 1);
    if (ret == 0)
    {
        if (DEBUG)
            std::cerr << "Error setting compass sampling rate (in compass_cfa)" << std::endl;
        return 0;
    }
    return 1;
}

int sensor_compass_setidle(void)
{
    char mode = 2;
    int ret = sensor_write(compass_addr, compass_mode, &mode, 1);
    if (ret == 0)
    {
        if (DEBUG)
            std::cerr << "Error setting compass to continuous measurement mode" << std::endl;
        return 0;
    }
    return 1;
}

int sensor_config_compass(void)
{
    int ret = sensor_compass_setmode();
    if (ret == 0)
    {
        if (DEBUG)
            std::cerr << "Error setting up compass" << std::endl;
        return 0;
    }
    return 1<<1;
}

int sensor_config_barometer(void)
{
    return 1<<0;
}

int sensor_config_gy80(struct config *c)
{
    // return value is a 4-bit number: AGCB, indicating
    // the return values of accel, gyro, compass, and barometer
    int ret = sensor_config_accelerometer();
    ret |= sensor_config_gyro();
    ret |= sensor_config_compass();
    ret |= sensor_config_barometer();
    return ret;
}
