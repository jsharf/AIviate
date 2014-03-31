#include "sensor.h"

/*extern Serial pc;
I2C i2c(p9, p10);
*/

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

/*
char sensor_set_i2c_pointer(char addr, char reg)
{
    addr &= ~1;
    if (i2c.write(addr) == 0)
    {
        if (DEBUG)
            pc.printf("Could not write device address (set_i2c_pointer)\r\n");
        return 0;
    }
    if (i2c.write(reg) == 0)
    {
        if (DEBUG)
            pc.printf("Could not write reg address (set_i2c_pointer)\r\n");
        return 0;
    }
    return 1;
}

int sensor_read(char addr, char reg, char *buf, int n)
{
    i2c.start();
    if (sensor_set_i2c_pointer(addr, reg) == 0)
    {
        if (DEBUG)
            pc.printf("Could not set i2c pointer (read)\r\n");
        return 0;
    }
    int ret = i2c.read(addr, buf,n);   
    if (ret != 0)
    {
        if (DEBUG)
            pc.printf("I2C read function failed in sensor_read\r\n");
        return 0;
    }
    return n;
}

int sensor_write(char addr, char reg, char *buf, int n)
{
    i2c.start();
    if (sensor_set_i2c_pointer(addr, reg) == 0)
    {
        if (DEBUG)
            pc.printf("Could not set i2c pointer (write)\r\n");
        return 0;
    }
    for (int i=0; i<n; i++)
    {
        if (i2c.write(buf[i]) == 0)
        {   
            i2c.stop();
            if (DEBUG)
                pc.printf("Only sent %i/%i bytes (write)\r\n", i, n);
            return i;
        }
    }
    i2c.stop();
    return n;
    
}

int sensor_read_accelerometer(struct sensor* s)
{
    int ret = sensor_read(accel_w, ACCEL_X, s->raw_data, 6);
    if (ret != 6)
    {
        pc.printf("Error, could not read (read_accelerometer)\r\n");
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
    int ret = sensor_read(accel_w, ACCEL_POWER_CTL, &power_ctl, 1);
    if (ret == 0)
    {
        if (DEBUG)
            pc.printf("Error putting accelerometer in standby (accelerometer_standby)\r\n");
        return 0;
    }
    power_ctl &= 0xF7 ;
    ret = sensor_write(accel_w, ACCEL_POWER_CTL, &power_ctl, 1);
    if (ret == 0)
    {
        if (DEBUG)
            pc.printf("Error putting accelerometer in standby (accelerometer_standby)\r\n");
        return 0;
    }
    return 1;
}

// enable accelerometer for measurements
int sensor_accelerometer_measure()
{
    char power_ctl;
    int ret = sensor_read(accel_w, ACCEL_POWER_CTL, &power_ctl, 1);
    if (ret == 0)
    {
        if (DEBUG)
            pc.printf("Error putting accelerometer in measure mode (accelerometer_measure)\r\n");
        return 0;
    }
    power_ctl |= 0x8 ;
    ret = sensor_write(accel_w, ACCEL_POWER_CTL, &power_ctl, 1);
    if (ret == 0)
    {
        if (DEBUG)
            pc.printf("Error putting accelerometer in measure mode (accelerometer_measure)\r\n");
        return 0;
    }    
    return 1;
}

int sensor_gyro_turnon()
{
    char power_ctl;
    int ret = sensor_read(gyro_w, GYRO_CTRL_REG1, &power_ctl, 1);
    if (DEBUG)
        pc.printf("Gyro REG1 read: %x\r\n",power_ctl);
    if (ret == 0)
    {
        if (DEBUG)
            pc.printf("Error turning on gyro (gyro_turnon)\r\n");
        return 0;
    }
    power_ctl |= 0x8;
    if (DEBUG)
        pc.printf("Gyro REG1 write: %x\r\n",power_ctl);
    ret = sensor_write(gyro_w, GYRO_CTRL_REG1, &power_ctl, 1);
    if (ret == 0)
    {
        if (DEBUG)
            pc.printf("Error turning on gyro (gyro_turnon)\r\n");
        return 0;
    }    
    return 1;
}

int sensor_gyro_turnoff()
{
    char power_ctl;
    int ret = sensor_read(gyro_w, GYRO_CTRL_REG1, &power_ctl, 1);
    if (ret == 0)
    {
        if (DEBUG)
            pc.printf("Error turning off gyro (gyro_turnoff)\r\n");
        return 0;
    }
    power_ctl &= 0xF7 ;
    ret = sensor_write(gyro_w, GYRO_CTRL_REG1, &power_ctl, 1);
    if (ret == 0)
    {
        if (DEBUG)
            pc.printf("Error turning off gyro (gyro_turnoff)\r\n");
        return 0;
    }
    return 1;
}

int sensor_read_gyro(struct sensor* s)
{
    //char buf = GYRO_X;
    int ret = sensor_read(gyro_w|1, GYRO_X, s->raw_data, 6);
    if (ret != 6)
    {
        pc.printf("Error, could not read (sensor_read_gyro)\r\n");
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
    int ret = sensor_read(compass_w, compass_x, s->raw_data, 6);
    if (ret == 0)
    {
        pc.printf("Error, could not read (read_gyro)\r\n");
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
    s->mz = ((mzmsb << 9) + mzlsb);
    return 1;
}

int sensor_read_barometer(void)
{
    return 0;
}

int sensor_config_accelerometer(void)
{
    // take accelerometer out of standby mode
    int ret = sensor_accelerometer_measure();
    if (ret == 0)
    {
        if (DEBUG)
            pc.printf("Error starting up accelerometer\r\n");
        return 0;
    }
    return 8;
}
int sensor_config_gyro()
{
    // turn on the gyro via i2c
    int ret = sensor_gyro_turnon();
    if (ret == 0)
    {
        if (DEBUG)
            pc.printf("Error starting up gyro\r\n");
        return 0;
    }
    return 4;
}

int sensor_compass_setmode(void)
{
    char mode = 0;
    int ret = sensor_write(compass_w, compass_mode, &mode, 1);
    if (ret == 0)
    {
        if (DEBUG)
            pc.printf("Error setting compass to continuous measurement mode\r\n");
        return 0;
    }    
    
    char cra = 0x18;
    ret = sensor_write(compass_w, compass_cra, &cra, 1);
    if (ret == 0)
    {
        if (DEBUG)
            pc.printf("Error setting compass sampling rate (in compass_cfa)\r\n");
        return 0;
    }
    return 1;
}

int sensor_compass_setidle(void)
{
    char mode = 2;
    int ret = sensor_write(compass_w, compass_mode, &mode, 1);
    if (ret == 0)
    {
        if (DEBUG)
            pc.printf("Error setting compass to continuous measurement mode\r\n");
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
            pc.printf("Error setting up compass\r\n");
        return 0;    
    }
    return 2;
}

int sensor_config_barometer(void)
{
    return 1;
}

int sensor_config_gy80(struct config *c)
{
    // return value is a 4-bit number: AGCB, indicating
    // the return values of accel, gyro, compass, and barometer
    i2c.frequency(c->frequency);
    int ret = sensor_config_accelerometer();
    ret |= sensor_config_gyro();
    ret |= sensor_config_compass();
    ret |= sensor_config_barometer();
    return ret;
}
*/
