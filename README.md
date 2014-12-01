AIviate
=======

AIviate is an autonomous drone created by modifying a Bixler V1.1 RC Airplane.
The microprocessor involved was previously the MBED LPC1768. The airplane
suffered a crash during testing and as a result the MBED broke. After
reconsidering our microprocessor choice, we decided to switch to the Raspberry
Pi for several reasons. This included cost, a full TCP/IP stack, unix, and a
number of other things (namely, both contributors own Raspberry Pis).

This project was started with the sole purpose of having fun and learning. If
you want to join us in the development of the airplane, contact us.

Code Structure
--------------

Currently AIviate consists of 3 programs:

1. Sensor Manager: Interfaces with the sensors over i2c and reads values
2. Controller: Recieves sensor values and operates the "control logic". It
sends commands to the surface manager
3. Surface Manager:  Receives commands from the controller and operates the
control surfaces.

These programs communicate over UDP using the library written in
Comms.cpp/Comms.h. They are seperate executable binaries.

### UDP Communication

Comms.cpp and Comms.h define a library which greatly simplifies inter-program
communications. Of note are send, listen, sendSensor, sendControl,
receiveSensor, receiveControl.

### Sensors

The sensors include a 10DOF IMU and a GPS module.

The IMU communicates over I2C and the GPS module is connected via a serial uart
connection.

#### IMU Module

The IMU has 4 sensors on it with a total of 10 DOF (Degrees of Freedom):
+ Accelerometer (Ax, Ay, Az)
+ Gyroscope (Gx, Gy, Gz)
+ Magnetic Compass (Mx, My, Mz)
+ Barometer (Pressure + Temperature)

Technically because the Barometer also measures temperature, this is really 11
DOF. But measuring altitude requires
both pressure and temperature, so we consider the altitude reading to be one degree of freedom.

The I2C IMU we are using is sold on Amazon as the "Sunkee 10DOF IMU" for $20.
It is more commonly known as the GY-80.

#### GPS Module

The GPS Module connects to the raspberry pi via a 3.3v TTL UART serial
communication line. The module can be purchased from the Adafruit website.
Sample code and instructions for wiring can also be found on Adafruit.

### Controller

This program should use receiveSensor and sendControl from Comm.h/.cpp. The
Controller receives sensor data from the sensor manager program and uses it to
calculate the appropriate commands to send to the surface manager. This is the
main logic and meat of our program. This program makes extensive use of Kevin's
filtering and control libraries.

Currently the control algorithm is mainly PID.

Debugging
---------

The UDP interface between the control program, the sensor manager, and the
actuator manager mimics the UDP protocol used by Flight Gear for interfacing
with external programs. This allows a simple Hardware In The Loop (HITL) testing
configuration. A powerful computer runs the Flight Gear simulator and configures
the program to send simulated sensor data to the control loop running on the
Raspberry Pi. The control program on the Raspberry Pi generates a set of
response commands and sends them over UDP to the simulator.

Since we can put programs in between Flightgear and the control loop, we can
simulate failures to see how the plane recovers from faulty sensors and faulty
actuators. 
