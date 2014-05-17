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

        Quaternion orientation(yaw, pitch, roll);

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
