
DEBUG=-Wall -pedantic -Wextra

FLAGS=-std=c++0x

INCLUDES=./,./LinuxI2C/,./Servo/

all: sensor control actuator

clean:
	rm -f control source *.o

####################
#                  #
#    UNIT TESTS    #
#                  #
####################

sensor_test: sensor_test.o sensor.o linux_i2c.o
	g++ $(FLAGS) $(DEBUG) -I $(INCLUDES) -o sensor_test ./Test/sensor_test.o sensor.o ./LinuxI2C/linux_i2c.o

sensor_test.o:
	g++ $(FLAGS) $(DEBUG) -I $(INCLUDES) -c -o ./Test/sensor_test.o ./Test/SensorTest.cpp

####################
#                  #
#    CODE FILES    #
#                  #
####################

sensor: sensor.o linux_i2c.o
	g++ $(FLAGS) $(DEBUG) -I $(INCLUDES) -o sensor sensor.o ./LinuxI2C/linux_i2c.o

control: control.o Comm.o
	g++ $(FLAGS) $(DEBUG) -I $(INCLUDES) -o control control.o Comm.o

actuator: actuator.o Servo.o
	g++ $(FLAGS) $(DEBUG) -I $(INCLUDES) -o actuator actuator.o

sensor.o: sensor.cpp
	g++ $(FLAGS) $(DEBUG) -c -o sensor.o sensor.cpp

Comm.o: Comm.cpp
	g++ $(FLAGS) $(DEBUG) -c -o Comm.o Comm.cpp

control.o: control.cpp
	g++ $(FLAGS) $(DEBUG) -c -o control.o  control.cpp

actuator.o: actuator.cpp
	g++ $(FLAGS) $(DEBUG) -c -o actuator.o actuator.cpp

linux_i2c.o: ./LinuxI2C/linux_i2c.cpp
	g++ $(FLAGS) $(DEBUG) -I $(INCLUDES) -c -o ./LinuxI2C/linux_i2c.o ./LinuxI2C/linux_i2c.cpp

Servo.o: ./Servo/Servo.cpp
	g++ $(FLAGS) $(DEBUG) -I $(INCLUDES) -c -o ./Servo/Servo.o ./Servo/Servo.cpp
