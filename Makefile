
DEBUG="-Wall -pedantic -Wextra -Werror"

FLAGS="-std=c++0x"

all: sensor control actuator

clean:
	rm -f control source *.o

sensor: sensor.o
	g++ $(FLAGS) $(DEBUG) -o sensor sensor.o

control: control.o
	g++ $(FLAGS) $(DEBUG) -o control control.o

actuator: actuator.o
	g++ $(FLAGS) $(DEBUG) -o actuator actuator.o

sensor.o: sensor.cpp Comm.cpp
	g++ $(FLAGS) $(DEBUG) -c -o sensor.o sensor.cpp Comm.cpp

control.o: control.cpp Comm.cpp
	g++ $(FLAGS) $(DEBUG) -Wall -c -o control.o control.cpp Comm.cpp

actuator.o: actuator.cpp Comm.cpp
	g++ $(FLAGS) $(DEBUG) -Wall -c -o actuator.o actuator.cpp Comm.cpp
