AIviate
=======

AIviate is an autonomous drone created by modifying a Bixler V1.1 RC Airplane.
The microprocessor involved was previously the MBED LPC1768. The airplane
recently suffered a crash, and as a result the MBED broke. After reconsidering
our microprocessor choice, we decided to switch to the Raspberry Pi for several
reasons. This included cost, a full TCP/IP stack, unix, and a number of other
things (namely, both contributors own Raspberry Pis).

This project was started with the sole purpose of having fun and learning. If
you want to join us in the development of the airplane, contact us. 

Code Structure
--------------

Currently, there are a couple processes coordinated by a centralized scheduler.
We're in the process of migrating to a Raspberry Pi. We hope to
separate each process in our code as individual Unix processes, obviating the
need for our own scheduling code (the Unix processes can communicate via TCP).
This will also simplify testing and debugging 

### Sensors

The sensors include a 10DOF IMU and a GPS module.

The IMU communicates over I2C and the GPS module is connected via a serial
uart connection.
   
#### IMU Module

The IMU has 4 sensors on it with a total of 10 DOF (Degrees of Freedom):
+ Accelerometer (Ax, Ay, Az)
+ Gyroscope (Gx, Gy, Gz)
+ Magnetic Compass (Mx, My, Mz)
+ Barometer (Pressure + Temperature)

Technically because the Barometer also measures temperature, this is really 11
DOF. But measuring altitude (what we really want from the barometer) requires
both pressure and temperature (without either, the sensor is useless for our
purposes), we consider it to be only one degree of measurement.

The I2C IMU we are using is sold on Amazon as the "Sunkee 10DOF IMU" for $20.
It is more commonly known as the GY-80.

#### GPS Module

The GPS Module connects to the raspberry pi via a 3.3v TTL UART serial
communication line. The module can be purchased from the Adafruit website.
Sample code and instructions for wiring can also be found on Adafruit.

### Database

This code is actually an extension to the process scheduler, and will likely
be outdated as we switch to the Raspberry Pi. It's not worth writing a
description of this section because the code will likely be replaced by
some form of IPC (probably TCP).

### Control

This code is the main process scheduling and management. See the section on
the Database code above.

### Filtering and Control

Luckily Kevin wrote a really nice PID and Filtering library, so this is mostly
code that has already been developed and used in the past (and thus is less
likely to include errors). I hope to improve on our control algorithm by
incorporating some neat algorithms in the approximation section below.

Currently the control algorithm is a bunch of PID loops. We plan on integrating
all of the different PIDs into one central control algorithm by creating a
mathematical model of the system. (For example, currently the pitch and roll are
simply PID-controlled to appropriate values, rather than modelling the
elevators, ailerons, and rudder as part of a more complex system which is
PID-controlled to it's appropriate state). This effectively does the same thing,
but is more flexible: you don't need to think of individual control surfaces,
and can instead think of "where" you want the plane to be (location and
orientation).

### Approximation

The approx.cpp code hasn't been finished yet and is my attempt to improve the
Kalman filter by increasing the accuracy of extrapolated data. By incorporating
standard quadrature methods (such as Romberg's algorithm) I hope to improve our
airplane's accuracy and response. This code probably won't be written for a
while.
rest of the airplane is done, since it isn't needed.

Debugging
---------

We plan on eventually setting up the control loop to communicate with Xplane so
that we can simulate our code before flying the airplane. We've even modeled the
airplane with kinect fusion (and plan on putting the plane model in Xplane to
improve the simulation).

Since we can put code inbetween Xplane and the control loop, we can simulate
failures to see how the plane recovers
