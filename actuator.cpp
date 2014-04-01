#include "actuator.h"
using namespace std;

void usage()
{
    cerr << "actuator <in_port> [DEBUG]" << endl;
}

int main(int argc, char *argv[])
{
    int DEBUG = 0;
    if (argc < 2 || argc > 3)
    {
        cerr << "Wrong number of arguments" << endl;
        usage();
        return 1;
    }
    if (argc == 3)
        DEBUG = 1;

    UDPListenever lst(argv[1]);
    control in_command;

    if (DEBUG)
        cerr << "Outputting debug info on stderr..." << endl;

    while (true)
    {
        receiveCommand(in_command);
        if (DEBUG)
            cerr << in_command;
        executeCommand(in_command); 
    }
}

void executeCommand(control &in_command)
{
    return;
}
