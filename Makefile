
DEBUG="-Wall -pedantic -Wextra -Werror"

FLAGS="-std=c++0x"

all: sensor control

clean:
	rm -f control source *.o

sensor: sensor.o
	g++ $(FLAGS) $(DEBUG) -o sensor sensor.o

sensor.o: sensor.cpp comms.cpp
	g++ $(FLAGS) $(DEBUG) -c -o sensor.o sensor.cpp comms.cpp

control.o: control.cpp comms.cpp
	g++ $(FLAGS) $(DEBUG) -Wall -c -o control.o control.cpp comms.cpp
