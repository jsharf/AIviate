#include "actuator.h"#include "Servo/Servo.h"using namespace std;Servo* servos[ACTUATOR_NUM_AXES];float backoff_time;void usage(){    cerr << "actuator <in_port> [OPTIONS]" << endl;    cerr << "\tOPTIONS:\n" <<         "\t  -d:\n\t\tEnable debugging\n" <<         "\t  -b <backoff time>:\n\t\tBackoff time; if multiple command packets are received within the backoff time,\n" <<         "\t\ttheir values are averaged and applied at the end of that time. (A backoff time of 0 means no backoff.)" <<         endl;}int main(int argc, char *argv[]){    backoff_time = 0;    int myDebug = 0;    if (argc < 2 || argc > 5)    {        cerr << "Wrong number of arguments" << endl;        usage();        return 1;    }    for(int i = 0; i < argc-1; i++)    {        if(strcmp(argv[i+1], "-d") == 0)        {            myDebug = 1;        }        if(strcmp(argv[i+1], "-b") == 0)        {            if(i+2 < argc-1)            {                backoff_time = strtof(argv[i+2], NULL);            }            else            {                cerr << "An argument must be specified for the -b flag." << endl;                usage();
                return 2;            }        }    }

    if(strtol(argv[1], NULL, 10) == 0)
    {
        cerr << "First argument must be a port number." << endl;
        return 3;

    }    UDPListener lst(argv[1]);
    control in_command;    if (myDebug)        cerr << "Outputting debug info on stderr..." << endl;    for(int i = 0; i < ACTUATOR_NUM_AXES; i++)    {        servos[i] = new Servo(i);    }    while (true)    {        lst.receiveControl(in_command);        //if (myDebug)        //    cerr << in_command;        if(!executeCommand(in_command))
            break;    }

    for(int i = 0; i < ACTUATOR_NUM_AXES; i++)
        delete servos[i];}bool executeCommand(control &in_command){    if((servos[0]->write(in_command.ail)+    servos[1]->write(in_command.elev)+    servos[2]->write(in_command.rudder)+    servos[3]->write(in_command.throttle)) < 0)
    {
        cerr << "Failed to set control surface positions." << endl;
        return false;
    }    return true;}
