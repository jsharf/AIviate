#ifndef control_h
#define control_h

#include <iostream>
#include <ostream>
//#include <unistd.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
/*#include <fcntl.h>
#include <netinet/in.h>
#include <netdb.h>*/
#include "Comm.h"
using namespace std;

struct control
{
    float ail; // ailerons
    float elev; // elevators
    float rudder; // duh
    float throttle;
    friend ostream& operator<<(ostream &out, control &rhs);
    static const control zero;
};

#endif
