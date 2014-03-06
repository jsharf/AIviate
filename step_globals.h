#ifndef STEP_GLOBALS_H
#define STEP_GLOBALS_H

#include "Servo/Servo.h"

// control surface pins
Servo AILERON_CONTROL(p25);
Servo ELEVATOR_CONTROL(p26);

// pins to take control over aircraft
DigitalOut ENABLE_FLIGHT(p8);

#endif //STEP_GLOBALS_H
