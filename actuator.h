#ifndef ACTUATOR_H
#define ACTUATOR_H

#include "LinuxI2C/linux_i2c.h"
#include "Comm/Control.h"
#include <iostream>
using namespace std;

#define ACTUATOR_NUM_AXES (4)

bool executeCommand(control &in_command);

#endif //ACTUATOR_H
