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
Quaternion fromEulerRad(fptype z, fptype y, fptype x)
{
    fptype zd2 = fptype(0.5)*z; 
    fptype yd2 = fptype(0.5)*y; 
    fptype xd2 = fptype(0.5)*x;
    fptype Szd2 = sin(zd2); fptype Syd2 = sin(yd2); fptype Sxd2 = sin(xd2);
    fptype Czd2 = cos(zd2); fptype Cyd2 = cos(yd2); fptype Cxd2 = cos(xd2);
    fptype Cxd2Czd2 = Cxd2*Czd2; fptype Cxd2Szd2 = Cxd2*Szd2;
    fptype Sxd2Szd2 = Sxd2*Szd2; fptype Sxd2Czd2 = Sxd2*Czd2;
    
    Quaternion q;
    q.w = Cxd2Czd2*Cyd2 + Sxd2Szd2*Syd2;
    q.x = Sxd2Czd2*Cyd2 - Cxd2Szd2*Syd2;
    q.y = Cxd2Czd2*Syd2 + Sxd2Szd2*Cyd2;
    q.z = Cxd2Szd2*Cyd2 - Sxd2Czd2*Syd2;
    return q;
}

Quaternion fromEulerDeg(fptype z, fptype y, fptype x)
{
    return fromEulerRad(DEG2RAD(z), DEG2RAD(y), DEG2RAD(x));
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

    while (true)
    {
        lst.receiveSensor(s);
        
        // convert sensorf stuff to orientation in variable p
        fptype roll, yaw, pitch;
        roll = s.mx;
        pitch = s.my;
        yaw = s.mz;
        
        PlaneState p;
        p.orientation = fromEulerDeg(yaw, roll, pitch);

        Vector3d a = Vector3d::i.rotate(p.orientation);
        cout << std::setw(8) <<  a.x << "\t";
        cout << std::setw(8) << a.y << "\t";
        cout << std::setw(8) <<  a.z << "\t" << endl;

        snd.sendPlaneState(p);
    }
}
