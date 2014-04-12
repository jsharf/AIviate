#ifndef control_h
#define control_h

#include <iostream>
#include <ostream>
//#include <unistd.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <ctime>
#include "Comm.h"
#include <cmath>
#include <ctime>
#include "Vector/Vector.h"
#include "Filters/Filters.h"
#include "PIDControl/PIDControl.h"
using namespace std;

#define OPTIMIZE
#ifdef OPTIMIZE
 #define OSTATIC static
#else
  #define OSTATIC 
#endif

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
