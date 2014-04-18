#include "Sensor.h"

#define K_comp (0.9)
#define GyroFullRange (250) //Degrees per second
#define GyroConversionFactor (TWO_PI/GyroFullRange)


// Helps debugging with sensor structs
std::ostream& operator<<(std::ostream &out, sensorf &rhs)
{
    out << "SENSORF: \n{\n\t ax: " \
    << rhs.ax << "\n\t ay: " << rhs.ay << "\n\t az: " << rhs.az \
    << "\n\t gx: " << rhs.gx << "\n\t gy: " << rhs.gy << "\n\t gz: " << rhs.gz \
    << "\n\t mx: " << rhs.mx << "\n\t my: " << rhs.my << "\n\t mz: " << rhs.mz \
    << "\n\t altitude: " << rhs.altitude \
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
//    static float k_accel = 0.01, \
//                 k_gyro = 0.0003, \
//                 k_r = 0.01; // Kalman filter constants
    // Create kalman filters to clean noisy sensor data
//    static KalmanFilter filterXY(k_accel, k_gyro, k_r);
    static Quaternion oldOrientation = Quaternion::identity;

    Vector3d gravity_vector(data.ax, data.ay, data.az);
    double ITheta = gravity_vector.magnitude() * dt;
    Quaternion IGyroQuat = gravity_vector.rotationAroundAxis(ITheta);
    Quaternion AccOrientation = Vector3d::i.quaternionTo(gravity_vector);
    p.orientation = Quaternion::identity.slerp(oldOrientation * IGyroQuat, K_comp) *
                    Quaternion::identity.slerp(AccOrientation, 1-K_comp);
    // Create
    

    // Complimentary filter on orientation

    
    // Create a quaternion which rotates Vector3d::i to the gravity vector
    //Quaternion attitude = Vector3d::i.quaternionTo(gvector);
    //p.orientation = attitude;
    oldOrientation = p.orientation;
}
