#ifndef STEPS_H
#define STEPS_H
#include "sensor.h"
#include "control.h"
#include "database.h"
#include "approx.h"

#define KP 1.0f
#define KD 0.01f

#define OPTIMIZE
#ifdef OPTIMIZE
 #define OSTATIC static
#else
 #define OSTATIC 
#endif

// number of cycles to use in measuring average zero-level gyro values
#define ZERO_MEASURE_PERIOD 1000
// whether or not to enable usb debugging
#define USBDEBUG 1

const float rollFK = 0.95f;
const float pitchFK = 0.95f;
const float rollKP = 0.6f;
const float rollKI = 0.0f;
const float rollKD = 0.02f;
const float pitchKP = 0.6f;
const float pitchKI = 0.0f;
const float pitchKD = 0.02f;

const float AileronCenter = 0.5f;
const float ElevatorCenter = 0.65f;

extern Serial pc;

int init_sensors(int);
int get_sensor_data(int);
int pid_control(int);

int sensor_fusion(int pid);

#endif //STEPS_H