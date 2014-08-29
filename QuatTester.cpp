#include "Vector/Vector.h"
#include "Comm/Comm.h"
#include <time.h>

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cerr << "Wrong number of arguments" << endl;
        return 1;
    }
    
    // UDP sender object
    UDPSender snd(argv[1], argv[2]);

    PlaneState p;

    usleep(1000000);

    while(true)
    {
        double i = 0, j=0, k=0;
        //for (double j=0; j<360; j++)
            for (k=0; k<360; k++)
            {
                double yaw=DEG2RAD(i), pitch=DEG2RAD(j), roll=DEG2RAD(k);
                p.orientation  = Quaternion(yaw, pitch, roll);
                snd.sendPlaneState(p);
                usleep(5000);
            }
        usleep(1000000);
        i=0, k = 0;
        for (j=0; j<360; j++)
            {
                double yaw=DEG2RAD(i), pitch=DEG2RAD(j), roll=DEG2RAD(k);
                p.orientation  = Quaternion(yaw, pitch, roll);
                snd.sendPlaneState(p);
                usleep(5000);
            }
        usleep(1000000);
        j=0, k = 0;
        for (i=0; i<360; i++)
            {
                double yaw=DEG2RAD(i), pitch=DEG2RAD(j), roll=DEG2RAD(k);
                p.orientation  = Quaternion(yaw, pitch, roll);
                snd.sendPlaneState(p);
                usleep(5000);
            }
            usleep(1000000);
    }
	
}
