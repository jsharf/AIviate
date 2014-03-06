#ifndef STEPS_H
#define STEPS_H
#include "sensor.h"
#include "control.h"
#include "database.h"
#include "approx.h"

#define KP 1.0f
#define KD 0.01f

#define OPTIMIZE

// OPT_STATIC assumes that a variable's initial
// value is always the same for each invocation of a 
// function.
#ifdef OPTIMIZE
  #define OPT_STATIC static
#else
  #define OPT_STATIC 
#endif

const float rollFK = 1.0f;
const float pitchFK = 1.0f;
const float rollKP = 0.5f;
const float rollKI = 0.01f;
const float rollKD = 0.1f;
const float pitchKP = 0.5f;
const float pitchKI = 0.01f;
const float pitchKD = 0.1f;

// number of cycles to use in measuring average zero-level gyro values
#define ZERO_MEASURE_PERIOD 10
// whether or not to enable usb debugging
#define USBDEBUG 1

extern Serial pc;

int init_sensors(int);
int get_sensor_data(int);
int pid_control(int);

int sensor_fusion(int pid);

#endif //STEPS_H
