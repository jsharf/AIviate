#include "../Vector/Vector.h"
#include "../Comm/Comm.h"
#include "../Comm/Control.h"
#include "../Comm/Sensor.h"
#include <iostream>
#include <iomanip>

typedef double fptype;

void usage()
{
    std::cerr << "Usage: compat_fg <inport> <out addr> <out port>" << std::endl;
}
/// Return a quaternion from euler angles
Quaternion fromEulerRad(fptype yaw, fptype pitch, fptype roll)
{
    Quaternion q;
    fptype zd2 = fptype(0.5)*yaw; fptype yd2 = fptype(0.5)*pitch; fptype xd2 = fptype(0.5)*roll;
    fptype Szd2 = sin(zd2); fptype Syd2 = sin(yd2); fptype Sxd2 = sin(xd2);
    fptype Czd2 = cos(zd2); fptype Cyd2 = cos(yd2); fptype Cxd2 = cos(xd2);
    fptype Cxd2Czd2 = Cxd2*Czd2; fptype Cxd2Szd2 = Cxd2*Szd2;
    fptype Sxd2Szd2 = Sxd2*Szd2; fptype Sxd2Czd2 = Sxd2*Czd2;
    q.w = Cxd2Czd2*Cyd2 + Sxd2Szd2*Syd2;
    q.x = Sxd2Czd2*Cyd2 - Cxd2Szd2*Syd2;
    q.y = Cxd2Czd2*Syd2 + Sxd2Szd2*Cyd2;
    q.z = Cxd2Szd2*Cyd2 - Sxd2Czd2*Syd2;
    return q;
}
int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        std::cerr << "Error: Incorrect number of arguments" << std::endl;
        usage();
        return 1;
    }

    UDPListener lst(argv[1]);
    UDPSender snd(argv[2], argv[3]);

    sensorf s;
    PlaneState p;
/*
    Vector3d rotate(const Vector3d& axis, fp_type theta) const;// Rotate the vector around the axis axis by the angle theta (+theta is CCW if vector points towards observer)
*/
    Vector3d axis;
    fptype angle = 0;
    while (true)
    {
        lst.receiveSensor(s);
        
        // convert sensorf stuff to orientation in variable p
        fptype phi, theta, psi;
        phi = s.mx;
        theta = s.my;
        psi = s.mz;
      
        fptype roll, pitch, yaw;
        roll = phi;
        pitch = theta;
        yaw = psi;
       
        PlaneState p;

        Quaternion orientation = fromEulerRad(yaw, pitch, roll);

        Vector3d axis = Vector3d::i.rotate(orientation);

        p.orientation = orientation;
/*
        fptype lastangle = angle;
        Vector3d lastaxis = axis;
        axis = p.orientation.getAxisAngle(angle);
*/
        cout << "yaw: " << std::setw(8) <<  yaw << "\t";
        cout << "pitch: " << std::setw(8) << pitch << "\t";
        cout << "roll: " << std::setw(8) <<  roll << "\t";
        cout << "i: " << std::setw(8) << axis.x << "\t";
        cout << "j: " << std::setw(8) << axis.y << "\t";
        cout << "k: " << std::setw(8) << axis.z << "\t";

        //cout << "angle: " << std::setw(8) << angle << "\t"; 
        cout << std::endl;
        snd.sendPlaneState(p);
    }
}
