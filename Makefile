
DEBUG=-Wall -pedantic -Wextra

FLAGS=-std=c++0x

INCLUDES=./,./LinuxI2C/,./Servo/,./PIDControl,./Filters,./Vector

all: ai-sensor ai-control ai-actuator compat_fg

run: all
	./run.sh
terminate:
	pkill ai-sensor && pkill ai-control && pkill ai-actuator

clean:
	rm -f ai-control ai-sensor ai-actuator *.o Comm/*.o Filters/*.o PIDControl/*.o vector/*.o fg_simu/compat_fg

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

ai-sensor: SensorManager.o ./LinuxI2C/linux_i2c.o Sensor/Sensor.o Comm/Comm.o Filters/Filters.o \
           Vector/Vector3d.o Vector/Quaternion.o Vector/Vector2d.o
	g++ $(FLAGS) $(DEBUG) -I $(INCLUDES) -o ai-sensor SensorManager.o \
	./LinuxI2C/linux_i2c.o Sensor/Sensor.o Comm/Comm.o Filters/Filters.o Vector/Vector3d.o \
	Vector/Quaternion.o -pthread

ai-control: ControlManager.o Comm/Control.o  Comm/Comm.o Sensor/Sensor.o Filters/Filters.o\
PIDControl/PIDControl.o Vector/Vector3d.o Vector/Vector2d.o Vector/Quaternion.o
	g++ $(FLAGS) $(DEBUG) -I $(INCLUDES) -o ai-control Comm/Control.o Comm/Comm.o \
    Sensor/Sensor.o  ControlManager.o Filters/Filters.o PIDControl/PIDControl.o Vector/Vector3d.o Vector/Vector2d.o Vector/Quaternion.o 

ai-actuator: actuator.o ./Servo/Servo.o Comm/Comm.o ./LinuxI2C/linux_i2c.o \
             Comm/Control.o Vector/Vector3d.o Vector/Quaternion.o Vector/Vector2d.o
	g++ $(FLAGS) $(DEBUG) -I $(INCLUDES) -o ai-actuator actuator.o Comm/Comm.o \
    ./Servo/Servo.o ./LinuxI2C/linux_i2c.o Comm/Control.o Vector/Vector3d.o Vector/Quaternion.o Vector/Vector2d.o

SensorManager.o: SensorManager.cpp
	g++ $(FLAGS) $(DEBUG) -I $(INCLUDES) -c -pthread -o SensorManager.o SensorManager.cpp

Sensor/Sensor.o: Sensor/Sensor.cpp
	g++ $(FLAGS) $(DEBUG) -c -o Sensor/Sensor.o Sensor/Sensor.cpp

Comm/Comm.o: Comm/Comm.cpp
	g++ $(FLAGS) $(DEBUG) -c -o Comm/Comm.o Comm/Comm.cpp

Comm/Control.o: Comm/Control.cpp
	g++ $(FLAGS) $(DEBUG) -c -o Comm/Control.o Comm/Control.cpp

ControlManager.o: ControlManager.cpp
	g++ $(FLAGS) $(DEBUG) -I $(INCLUDES) -c -o ControlManager.o ControlManager.cpp

actuator.o: actuator.cpp
	g++ $(FLAGS) $(DEBUG) -I $(INCLUDES) -c -o actuator.o actuator.cpp

linux_i2c.o: ./LinuxI2C/linux_i2c.cpp
	g++ $(FLAGS) $(DEBUG) -I $(INCLUDES) -c -o ./LinuxI2C/linux_i2c.o ./LinuxI2C/linux_i2c.cpp

Servo.o: ./Servo/Servo.cpp
	g++ $(FLAGS) $(DEBUG) -I $(INCLUDES) -c -o ./Servo/Servo.o ./Servo/Servo.cpp
Filters/Filters.o: Filters/Filters.cpp
	g++ $(FLAGS) $(DEBUG) -I $(INCLUDES) -c -o ./Filters/Filters.o \
./Filters/Filters.cpp

PIDControl/PIDControl.o: PIDControl/PIDControl.cpp
	g++ $(FLAGS) $(DEBUG) -I $(INCLUDES) -c -o ./PIDControl/PIDControl.o \
./PIDControl/PIDControl.cpp

Vector/Vector3d.o: Vector/Vector3d.cpp
	g++ $(FLAGS) $(DEBUG) -I $(INCLUDES) -c -o ./Vector/Vector3d.o ./Vector/Vector3d.cpp

Vector/Quaternion.o: Vector/Quaternion.cpp
	g++ $(FLAGS) $(DEBUG) -I $(INCLUDES) -c -o ./Vector/Quaternion.o ./Vector/Quaternion.cpp

Vector/Vector2d.o: Vector/Vector2d.cpp
	g++ $(FLAGS) $(DEBUG) -I $(INCLUDES) -c -o ./Vector/Vector2d.o ./Vector/Vector2d.cpp

compat_fg: Comm/Comm.o Sensor/Sensor.o Vector/Vector3d.o Vector/Quaternion.o
	g++ fg_simu/compat_fg.cpp -o fg_simu/compat_fg \
    $(FLAGS) $(DEBUG) -I $(INCLUDES) Comm/Comm.o \
    Sensor/Sensor.o Vector/Vector3d.o Vector/Quaternion.o 
