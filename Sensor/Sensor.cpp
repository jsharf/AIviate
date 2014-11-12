#include "Sensor.h"
#include "../Comm/Comm.h"

#define K_comp (0.9)
#define GyroFullRange (250) //Degrees per second
#define GyroConversionFactor (TWO_PI/GyroFullRange)

// Helps debugging with sensor structs
std::ostream& operator<<(std::ostream &out, sensorf &rhs)
{
    out << "SENSORF: \n{\n\t ax: " 
        << rhs.ax << "\n\t ay: " << rhs.ay << "\n\t az: " << rhs.az 
        << "\n\t gx: " << rhs.gx << "\n\t gy: " << rhs.gy << "\n\t gz: " << rhs.gz 
        << "\n\t mx: " << rhs.mx << "\n\t my: " << rhs.my << "\n\t mz: " << rhs.mz 
        << "\n\t altitude: " << rhs.altitude 
        << "\n}" << std::endl;
    return out;
}

void sensor_to_float(const sensor &a, sensorf &f)
{
    f.mx = (float) a.mx;
    f.my = (float) a.my;
    f.mz = (float) a.mz;
    f.gx = (float) a.gx*GyroConversionFactor;
    f.gy = (float) a.gy*GyroConversionFactor;
    f.gz = (float) a.gz*GyroConversionFactor;
    f.ax = (float) a.ax;
    f.ay = (float) a.ay;
    f.az = (float) a.az;
    f.altitude = (float) a.altitude;
}

void sensorf_to_planestate(const sensorf &data, PlaneState &p, float dt)
{
    Vector3d gravity_vector(data.ax, data.ay, data.az);
    Vector3d gyro_vector(data.gx, data.gy, data.gz);
    Vector3d mag_vector(data.mx, data.my, data.mz);

    static Quaternion q, qDot;
    static float norm, 
                 Beta = 3.0, 
                 target_Beta = 0.015,
                 SamplePeriod = 0.01;

    // Auxiliary variables to avoid repeated arithmetic
    float _2q2mx;
    float hx, hy;

    Vector3d _2q1m, _2b, _4b, _8b;
    Quaternion _2q, q1, q2, q3, q4, s;

    // normalize magnetometer
    mag_vector = mag_vector.unit();

    gyro_vector *= 0.00875; // convert to dps

    gyro_vector *= 180.0/PI; // convert to radians/sec

        //invert accelometer, so positive is down
        gravity_vector *= -1;

    // Auxiliary variables to avoid repeated arithmetic
    _2q1m.x = 2.0f * q.x * mag_vector.x;
    _2q1m.y = 2.0f * q.x * mag_vector.y;
    _2q1m.z = 2.0f * q.x * mag_vector.z;
    _2q2mx = 2.0f * q.y * mag_vector.x;

    _2q.x = 2.0f * q.x;
    _2q.y = 2.0f * q.y;
    _2q.z = 2.0f * q.z;
    _2q.w = 2.0f * q.w;

    q1.x = q.x * q.x;
    q1.y = q.x * q.y;
    q1.z = q.x * q.z;
    q1.w = q.x * q.w;
    q2.y = q.y * q.y;
    q2.z = q.y * q.z;
    q2.w = q.y * q.w;
    q3.z = q.z * q.z;
    q3.w = q.z * q.w;
    q4.w = q.w * q.w;

    // Normalise accelerometer measurement (what if all zero? whatever!)
    gravity_vector = gravity_vector.unit();

    // Reference direction of Earth's magnetic field
    _2q1m.x = 2.0f * q.x * mag_vector.x;
    _2q1m.y = 2.0f * q.x * mag_vector.y;
    _2q1m.z = 2.0f * q.x * mag_vector.z;
    _2q2mx = 2.0f * q.y * mag_vector.x;
    hx = mag_vector.x * q1.x - _2q1m.y * q.w + _2q1m.z * q.z + mag_vector.x * q2.y + _2q.y * mag_vector.y * q.z + _2q.y * mag_vector.z * q.w - mag_vector.x * q3.z - mag_vector.x * q4.w;
    hy = _2q1m.x * q.z + mag_vector.y * q1.x - _2q1m.z * q.y + _2q2mx * q.z - mag_vector.y * q2.y + mag_vector.y * q3.z + _2q.z * mag_vector.z * q.w - mag_vector.y * q4.w;
    _2b.x = (float)sqrt(hx * hx + hy * hy);
    _2b.z = -_2q1m.x * q.z + _2q1m.y * q.y + mag_vector.z * q1.x + _2q2mx * q.w - mag_vector.z * q2.y + _2q.z * mag_vector.y * q.w - mag_vector.z * q3.z + mag_vector.z * q4.w;
    _4b.x = 2.0f * _2b.x;
    _4b.z = 2.0f * _2b.z;
    _8b.x = 2.0f * _4b.x;
    _8b.z = 2.0f * _4b.z;

    // Gradient decent algorithm corrective step
    s.x= -_2q.z*(2*(q2.w - q1.z) - gravity_vector.x)    +   _2q.y*(2*(q1.y + q3.w) - gravity_vector.y)   +  -_4b.z*q.z*(_4b.x*(0.5 - q3.z - q4.w) + _4b.z*(q2.w - q1.z) - mag_vector.x)   +   (-_4b.x*q.w+_4b.z*q.y)*(_4b.x*(q2.z - q1.w) + _4b.z*(q1.y + q3.w) - mag_vector.y)    +   _4b.x*q.z*(_4b.x*(q1.z + q2.w) + _4b.z*(0.5 - q2.y - q3.z) - mag_vector.z);
    s.y= _2q.w*(2*(q2.w - q1.z) - gravity_vector.x) +   _2q.x*(2*(q1.y + q3.w) - gravity_vector.y) +   -4*q.y*(2*(0.5 - q2.y - q3.z) - gravity_vector.z)    +   _4b.z*q.w*(_4b.x*(0.5 - q3.z - q4.w) + _4b.z*(q2.w - q1.z) - mag_vector.x)   + (_4b.x*q.z+_4b.z*q.x)*(_4b.x*(q2.z - q1.w) + _4b.z*(q1.y + q3.w) - mag_vector.y)   +   (_4b.x*q.w-_8b.z*q.y)*(_4b.x*(q1.z + q2.w) + _4b.z*(0.5 - q2.y - q3.z) - mag_vector.z);             
    s.z= -_2q.x*(2*(q2.w - q1.z) - gravity_vector.x)    +     _2q.w*(2*(q1.y + q3.w) - gravity_vector.y)   +   (-4*q.z)*(2*(0.5 - q2.y - q3.z) - gravity_vector.z) +   (-_8b.x*q.z-_4b.z*q.x)*(_4b.x*(0.5 - q3.z - q4.w) + _4b.z*(q2.w - q1.z) - mag_vector.x)+(_4b.x*q.y+_4b.z*q.w)*(_4b.x*(q2.z - q1.w) + _4b.z*(q1.y + q3.w) - mag_vector.y)+(_4b.x*q.x-_8b.z*q.z)*(_4b.x*(q1.z + q2.w) + _4b.z*(0.5 - q2.y - q3.z) - mag_vector.z);
    s.w= _2q.y*(2*(q2.w - q1.z) - gravity_vector.x) +   _2q.z*(2*(q1.y + q3.w) - gravity_vector.y)+(-_8b.x*q.w+_4b.z*q.y)*(_4b.x*(0.5 - q3.z - q4.w) + _4b.z*(q2.w - q1.z) - mag_vector.x)+(-_4b.x*q.x+_4b.z*q.z)*(_4b.x*(q2.z - q1.w) + _4b.z*(q1.y + q3.w) - mag_vector.y)+(_4b.x*q.y)*(_4b.x*(q1.z + q2.w) + _4b.z*(0.5 - q2.y - q3.z) - mag_vector.z);


    s = s.unit();

    // Compute rate of change of quaternion
    qDot.x = 0.5 * (-q.y * gyro_vector.x - q.z * gyro_vector.y - q.w * gyro_vector.z) - Beta * s.x;
    qDot.y = 0.5 * (q.x * gyro_vector.x + q.z * gyro_vector.z - q.w * gyro_vector.y) - Beta * s.y;
    qDot.z = 0.5 * (q.x * gyro_vector.y - q.y * gyro_vector.z + q.w * gyro_vector.x) - Beta * s.z;
    qDot.w = 0.5 * (q.x * gyro_vector.z + q.y * gyro_vector.y - q.z * gyro_vector.x) - Beta * s.w;

    if(Beta>target_Beta) Beta -= 0.01;

    // Integrate to yield quaternion
    q.x += qDot.x * SamplePeriod;
    q.y += qDot.y * SamplePeriod;
    q.z += qDot.z * SamplePeriod;
    q.w += qDot.w * SamplePeriod;

    q = q.unit();

    // q is resulting orientation quaternion
    p.orientation = q;    
}
