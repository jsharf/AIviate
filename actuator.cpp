#include "actuator.h"
#include "Servo/Servo.h"
#include <iostream>
#include <time.h>
#include <string>

Servo* servos[ACTUATOR_NUM_AXES];
float backoff_time;


void usage()
{
    std::cerr << "actuator <in_port> [OPTIONS]" << std::endl;
    std::cerr << "\tOPTIONS:\n" <<
         "\t  -d:\n\t\tEnable debugging\n" <<
         "\t  -b <backoff time>:\n\t\tBackoff time; if multiple command packets are received within the backoff time,\n" <<
         "\t\ttheir values are averaged and applied at the end of that time. (A backoff time of 0 means no backoff.)" <<
         std::endl;
}

int main(int argc, char *argv[])
{
    time_t prev_time;
    backoff_time = 0;
    int myDebug = 0;
    if (argc < 2 || argc > 5)
    {
        std::cerr << "Wrong number of arguments" << std::endl;
        usage();
        return 1;
    }
    for(int i = 0; i < argc-1; i++)
    {
        if(strcmp(argv[i+1], "-d") == 0)
        {
            myDebug = 1;
        }

        if(strcmp(argv[i+1], "-b") == 0)
        {
            if(i+2 < argc)
            {
                backoff_time = strtof(argv[i+2], NULL);
            }
            else
            {
                std::cerr << "An argument must be specified for the -b flag." << std::endl;
                usage();
                return 2;
            }
        }
    }

    // Ensure that the first argument is a port number
    long port_number = strtol(argv[1], NULL, 10);
    if(port_number < 0x0001 || port_number > 0xFFFF)
    {
        std::cerr << "First argument must be a port number." << std::endl;
        return 3;

    }

    // Create the UDPListener on the specified port
    UDPListener lst(argv[1]);

    control in_command = control::zero, average_command = control::zero;
    //average_command.ail = average_command.elev = average_command.rudder = average_command.throttle = 0;
    int average_denominator = 0;

    prev_time = time(NULL);

    // Print debug message
    if (myDebug)
    {
        std::cerr << "Outputting debug info on stderr..." << std::endl;
        std::cerr << "Listener start time: " << std::string(ctime(&prev_time)) << std::endl;
    }

    // Construct the servo instances
    for(int i = 0; i < ACTUATOR_NUM_AXES; i++)
    {
        servos[i] = new Servo(i);
    }

    while (true)
    {
        if(lst.receiveControl(in_command) == -1)
        {
            continue;
        }
        if (myDebug)
            std::cerr << in_command << std::endl;
        if(backoff_time > 0)
        {
            if(difftime(prev_time, time(NULL)) < backoff_time)
            {
                average_command.ail += in_command.ail;
                average_command.elev += in_command.elev;
                average_command.rudder += in_command.rudder;
                average_command.throttle += in_command.throttle;
                average_denominator++;
                if(myDebug)
                    std::cerr << "Message backed off, added to average queue; size = " << average_denominator << std::endl;
            }
            else
            {
                average_command.ail /= average_denominator;
                average_command.elev /= average_denominator;
                average_command.rudder /= average_denominator;
                average_command.throttle /= average_denominator;

                if(!executeCommand(average_command))
                    break;

                prev_time = time(NULL);
                average_denominator = 0;
                average_command = control::zero;
            }
        }
        else
        {
            if(!executeCommand(in_command))
                break;
        }
    }

    for(int i = 0; i < ACTUATOR_NUM_AXES; i++)
        delete servos[i];
}

bool executeCommand(control &in_command)
{
    if((servos[0]->write((in_command.ail/2) + 0.5)+
            servos[1]->write((in_command.elev/2) + 0.5)+
            servos[2]->write((in_command.rudder/2) + 0.5)+
            servos[3]->write(in_command.throttle)) < 0)
    {
        std::cerr << "Failed to set control surface positions." << std::endl;
        return false;
    }

    return true;
}
