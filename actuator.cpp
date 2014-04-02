#include "actuator.h"
#include "Servo/Servo.h"
using namespace std;

Servo* servos[4];

void usage()
{
    cerr << "actuator <in_port> [DEBUG]" << endl;
}

int main(int argc, char *argv[])
{
    int myDebug = 0;
    if (argc < 2 || argc > 3)
    {
        cerr << "Wrong number of arguments" << endl;
        usage();
        return 1;
    }
    if (argc == 3)
        myDebug = 1;

    UDPListener lst(argv[1]);
    control in_command;

    if (myDebug)
        cerr << "Outputting debug info on stderr..." << endl;

    for(int i = 0; i < 4; i++)
    {
        servos[i] = new Servo(i);
    }

    while (true)
    {
        lst.receiveControl(in_command);
        //if (myDebug)
        //    cerr << in_command;
        executeCommand(in_command);
    }

    delete [] servos;
}

void executeCommand(control &in_command)
{
    servos[0]->write(in_command.ail);
    servos[1]->write(in_command.elev);
    servos[2]->write(in_command.rudder);
    servos[3]->write(in_command.throttle);

    return;
}
