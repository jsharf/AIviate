
DEBUG=-Wall -pedantic -Wextra

FLAGS=-std=c++0x

INCLUDES=./,./LinuxI2C/,./Servo/

all: ai-sensor ai-control ai-actuator

clean:
	rm -f ai-control ai-source ai-actuator *.o

####################
#                  #
#    UNIT TESTS    #
#                  #
####################

sensor_test: sensor_test.o SensorManager.o linux_i2c.o
	g++ $(FLAGS) $(DEBUG) -I $(INCLUDES) -o sensor_test ./Test/sensor_test.o SensorManager.o ./LinuxI2C/linux_i2c.o

sensor_test.o: ./Test/SensorTest.cpp
	g++ $(FLAGS) $(DEBUG) -I $(INCLUDES) -c -o ./Test/sensor_test.o ./Test/SensorTest.cpp

servo_test: servo_test.o Servo.o linux_i2c.o
	g++ $(FLAGS) $(DEBUG) -I $(INCLUDES) -o servo_test ./Test/servo_test.o ./Servo/Servo.o ./LinuxI2C/linux_i2c.o

servo_test.o: ./Test/ServoTest.cpp
	g++ $(FLAGS) $(DEBUG) -I $(INCLUDES) -c -o ./Test/servo_test.o ./Test/ServoTest.cpp

####################
#                  #
#    CODE FILES    #
#                  #
####################

ai-sensor: SensorManager.o ./LinuxI2C/linux_i2c.o Sensor.o
	g++ $(FLAGS) $(DEBUG) -I $(INCLUDES) -o ai-sensor SensorManager.o \
	./LinuxI2C/linux_i2c.o Sensor.o

ai-control: ControlManager.o control.o  Comm.o Sensor.o
	g++ $(FLAGS) $(DEBUG) -I $(INCLUDES) -o ai-control control.o Comm.o Sensor.o ControlManager.o

ai-actuator: actuator.o ./Servo/Servo.o Comm.o ./LinuxI2C/linux_i2c.o control.o
	g++ $(FLAGS) $(DEBUG) -I $(INCLUDES) -o ai-actuator actuator.o Comm.o ./Servo/Servo.o ./LinuxI2C/linux_i2c.o control.o

SensorManager.o: SensorManager.cpp
	g++ $(FLAGS) $(DEBUG) -c -o SensorManager.o SensorManager.cpp

Sensor.o: Sensor.cpp
	g++ $(FLAGS) $(DEBUG) -c -o Sensor.o Sensor.cpp

Comm.o: Comm.cpp
	g++ $(FLAGS) $(DEBUG) -c -o Comm.o Comm.cpp

control.o: control.cpp
	g++ $(FLAGS) $(DEBUG) -c -o control.o control.cpp

ControlManager.o: ControlManager.cpp
	g++ $(FLAGS) $(DEBUG) -c -o ControlManager.o ControlManager.cpp

actuator.o: actuator.cpp
	g++ $(FLAGS) $(DEBUG) -I $(INCLUDES) -c -o actuator.o actuator.cpp

linux_i2c.o: ./LinuxI2C/linux_i2c.cpp
	g++ $(FLAGS) $(DEBUG) -I $(INCLUDES) -c -o ./LinuxI2C/linux_i2c.o ./LinuxI2C/linux_i2c.cpp

Servo.o: ./Servo/Servo.cpp
	g++ $(FLAGS) $(DEBUG) -I $(INCLUDES) -c -o ./Servo/Servo.o ./Servo/Servo.cpp
