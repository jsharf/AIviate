#include "Sensor.h"

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
    //    static float k_accel = 0.01, 
    //                 k_gyro = 0.0003, 
    //                 k_r = 0.01; 
    // Kalman filter constants
    // Create kalman filters to clean noisy sensor data
    //    static KalmanFilter filterXY(k_accel, k_gyro, k_r);
    static Vector3d oldUp = Vector3d::k;
    static Vector3d oldFront = Vector3d::i;
    
    Vector3d gravity_vector(data.ax, data.ay, data.az);
    Vector3d gyro_vector(data.gx, data.gy, data.gz);
    Vector3d mag_vector(data.mx, data.my, data.mz);

    // orthonormal eigenbasis (u,v,w) using accelerometer and magnetic compass.
    // gravity and magnetic compass should normally be orthonormal, but the 
    // chips aren't perfectly aligned and the sensors have error. So we build
    // an orthonormal eigenbasis from the gravity and magnetic compass vectors
    // to force them to be orthogonal
    Vector3d ugrav = gravity_vector.unit();

    Vector3d mag_proj = mag_vector.project(ugrav);
    Vector3d mag_perp = mag_vector - mag_proj;
    Vector3d umag_perp = perp.unit();

    Vector3d up_now = Vector3d::k + ugrav;
    Vector3d forward_now = Vector3d::i - umag_perp;
    //Vector3d w = (u.cross(v)).unit();

    // orientation quaternion
    // Quaternion orientation = w.quaternionTo(Vector3d::k);

    double ITheta = gyro_vector.magnitude() * dt;
    
    Quaternion IGyroQuat = gyro_vector.rotationAroundAxis(ITheta);
    //Quaternion AccOrientation = Vector3d::i.quaternionTo(gravity_vector);
    static Vector3d up  = (oldUp.rotate(IGyroQuat))*(K_comp) + up_now*(1-K_comp);
        Quaternion::identity.slerp(orientation, 1-K_comp);
    oldUp = up;

    static Vector3d front = (oldFront.rotate(IGyroQuat))*(K_comp) + up_now*(1-K_comp);
    oldFront = front;

    // implement triad method to determine attitude rotation matrix
    // then determine quaternion from attitude rotation matrix
    // then set p.orientation equal to the attitude quaternion
    // http://en.wikipedia.org/wiki/Triad_method
    
    // upwards is Vector3d::k from world's reference
    Vector3d S = Vector3d::k;

    // upwards from airplane's reference
    Vector3d s = up.unit();

    Vector3d M = Vector3d::k.cross(Vector3d::i).unit();
    Vector3d m = up.cross(front).unit();

    Vector3d SxM = S.cross(M);
    Vector3d sxm = s.cross(m);

    // the following is going to get really dicey. sorry
    // formulate the 3 rows of the matrix A which
    // is the transformation between airplane coordinates
    // and world coordinates.  

    std::vector <Vector3d> A(3);
    A[0].x = S.x*s.x + M.x*m.x + SxM.x*sxm.x;
    A[0].y = S.x*s.y + M.x*m.y + SxM.x*sxm.y;    
    A[0].z = S.x*s.z + M.x*m.z + SxM.x*sxm.z;    

    A[1].x = S.y*s.x + M.y*m.x + SxM.y*sxm.x;
    A[1].y = S.y*s.y + M.y*m.y + SxM.y*sxm.y;    
    A[1].z = S.y*s.z + M.y*m.z + SxM.y*sxm.z;    
    
    A[2].x = S.z*s.x + M.z*m.x + SxM.z*sxm.x;
    A[2].y = S.z*s.y + M.z*m.y + SxM.z*sxm.y;    
    A[2].z = S.z*s.z + M.z*m.z + SxM.z*sxm.z;    

    // now convert the rotation matrix A into a quaternion
    // http://en.wikipedia.org/wiki/Rotation_formalisms_in_three_dimensions#Rotation_matrix_.E2.86.94_quaternion
    Quaternion q;
    q.w = 0.5*sqrt(1 + A[0].x + A[1].y + A[2].z);
    double k = 0.25*(1/q.w);
    q.x = k*(A[2].y - A[1].z);
    q.y = k*(A[0].z - A[2].x);
    q.z = k*(A[1].x - A[0].y);
    
    p.orientation = q;
}
