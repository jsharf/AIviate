#include "SensorManager.h"
#include "Sensor.h"

void usage()
{
    cerr << "sensor <out_addr> <out_port>" << endl;
}

// The current altitude
// may temporarily be an invalid
// reading because
static struct
{
    char isValid;
    unsigned long int timestamp;
    float altitude;
} currAltitude;

// Barometer calibration values
int ac1;
int ac2;
int ac3;
unsigned int ac4;
unsigned int ac5;
unsigned int ac6;
int b1;
int b2;
int mb;
int mc;
int md;
long b5;

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

    currAltitude.isValid = 0;
    // create a separate thread to measure altitude
    pthread_t altitudeThread;
    if (pthread_create(&altitudeThread, NULL, sensor_read_barometer, NULL) != 0)
    {
        cerr << "Could not initiate pthread for reading altitude values" << \
        endl;
    }

    while (true)
    {
        // Get sensor data from I2C
        sensor_read_accelerometer(&out_data);
        sensor_read_gyro(&out_data);
        sensor_read_compass(&out_data);
        if (currAltitude.isValid)
        {
            out_data.altitude = currAltitude.altitude;
        }
        else
            out_data.altitude = NAN;
        snd.sendSensor(out_data);
    }
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
    char* buf2 = (char*)malloc((n+1)*sizeof(char));
    memcpy(buf2+1, buf, n);
    buf2[0] = reg;
    int i = I2CBus::getInstance().i2c_write(addr, buf2, n+1);
    if (i != (n+1))
    {
        if (DEBUG)
            std::cerr << "Only sent " << i << "/" << n << " bytes (write)" << std::endl;
        return i;
    }
    free(buf2);
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
    power_ctl &= 0xF7;
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

float bmp085GetTemperature(unsigned int16_t ut)
{
    long x1, x2;

    x1 = (((long)ut - (long)ac6)*(long)ac5) >> 15;
    x2 = ((long)mc << 11)/(x1 + md);
    b5 = x1 + x2;

    float temp = ((b5 + 8)>>4);
    temp = temp /10;

    return temp;
}

// Read 2 bytes from the BMP085
// First byte will be from 'address'
// Second byte will be from 'address'+1
int16_t bmp085ReadInt(unsigned char address)
{
    char buf[2];
    sensor_read(BMP085_ADDRESS, address, buf, 2);
    unsigned char msb = (unsigned) buf[0];
    unsigned char lsb = (unsigned) buf[1];
    return (unsigned int16_t) msb<<8 | lsb;
}

// Read 1 byte from the BMP085 at 'address'
char bmp085Read(unsigned char address)
{
    char data;
    sensor_read(BMP085_ADDRESS, address, &data, 1);
    return data;
}

int bmp085GetPressure(unsigned int up)
{
    int x1, x2, x3, b3, b6, p;
    unsigned int b4, b7;

    b6 = b5 - 4000;
    // Calculate B3
    x1 = (b2 * (b6 * b6)>>12)>>11;
    x2 = (ac2 * b6)>>11;
    x3 = x1 + x2;
    b3 = (((((int)ac1)*4 + x3)<<OSS) + 2)>>2;

    // Calculate B4
    x1 = (ac3 * b6)>>13;
    x2 = (b1 * ((b6 * b6)>>12))>>16;
    x3 = ((x1 + x2) + 2)>>2;
    b4 = (ac4 * (unsigned int)(x3 + 32768))>>15;

    b7 = ((unsigned int)(up - b3) * (50000>>OSS));
    if (b7 < 0x80000000)
        p = (b7<<1)/b4;
    else
        p = (b7/b4)<<1;

    x1 = (p>>8) * (p>>8);
    x1 = (x1 * 3038)>>16;
    x2 = (-7357 * p)>>16;
    p += (x1 + x2 + 3791)>>4;

    return p;
}

unsigned int16_t bmp085ReadUT(){
    unsigned int16_t ut;

    char data = 0x2E;
    // Write 0x2E into Register 0xF4
    // This requests a temperature reading
    if (sensor_write(BMP085_ADDRESS, 0xF4, &data, 1) < 0)
    {
        if (DEBUG)
            std::cerr << "Error requesting UT from BMP085" << std::endl;
        return 0;
    }

    // wait 5 milliseconds
    struct timespec delay = { 0,0};
    delay.tv_sec = 0;
    delay.tv_nsec = (5) * (1000000); // 5 milliseconds = 5 nanoseconds *
                                       // 10^6 nanoseconds/millisecond.
    nanosleep(&delay, NULL);

    // Read two bytes from registers 0xF6 and 0xF7
    ut = bmp085ReadInt(0xF6);
    return ut;
}

// Read the uncompensated pressure value
unsigned int bmp085ReadUP()
{
    unsigned char msb, lsb, xlsb;
    unsigned int up = 0;

    // Write 0x34+(OSS<<6) into register 0xF4
    // Request a pressure reading w/ oversampling setting
    char data = 0x34 + (OSS << 6);;
    // Write 0x2E into Register 0xF4
    // This requests a temperature reading
    if (sensor_write(BMP085_ADDRESS, 0xF4, &data, 1) < 0)
    {
        if (DEBUG)
            std::cerr << "Error requesting UT from BMP085" << std::endl;
        return 0;
    }


    // wait 5 milliseconds
    struct timespec delay = { 0,0};
    delay.tv_sec = 0;
    // 5 milliseconds = 5 nanoseconds * 10^6 nanoseconds/millisecond.
    delay.tv_nsec = (2 + (3<<OSS)) * (1000000);
    nanosleep(&delay, NULL);

    // Read register 0xF6 (MSB), 0xF7 (LSB), and 0xF8 (XLSB)
    msb = bmp085Read(0xF6);
    lsb = bmp085Read(0xF7);
    xlsb = bmp085Read(0xF8);

    up = (((unsigned int) msb << 16) | ((unsigned int) lsb << 8) | (unsigned int) xlsb) >> (8-OSS);

    return up;
}

float calcAltitude(float pressure)
{
    float A = pressure/101325;
    float B = 1/5.25588;
    float C = pow(A,B);
    C = 1 - C;
    C = C /0.0000225577;

    return C;
}

void* sensor_read_barometer(void *ignore)
{
    #pragma SUPPRESS
    //int nSteps;
    while (true)
    {
        // check temperature
        //float temperature = bmp085GetTemperature(bmp085ReadUT()); //MUST be called first
        // check pressure
        float pressure = bmp085GetPressure(bmp085ReadUP());
        //float atm = pressure / 101325; // "standard atmosphere"
        float altitude = calcAltitude(pressure); //Uncompensated caculation - in Meters
        // update currAltitude
        currAltitude.altitude = altitude;
        currAltitude.timestamp = time(NULL);
        currAltitude.isValid = 1;
    }

    return NULL;
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
    ac1 = bmp085ReadInt(0xAA);
    ac2 = bmp085ReadInt(0xAC);
    ac3 = bmp085ReadInt(0xAE);
    ac4 = bmp085ReadInt(0xB0);
    ac5 = bmp085ReadInt(0xB2);
    ac6 = bmp085ReadInt(0xB4);
    b1 = bmp085ReadInt(0xB6);
    b2 = bmp085ReadInt(0xB8);
    mb = bmp085ReadInt(0xBA);
    mc = bmp085ReadInt(0xBC);
    md = bmp085ReadInt(0xBE);
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
