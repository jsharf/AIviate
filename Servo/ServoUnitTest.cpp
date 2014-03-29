#include "Servo.h"
#include <ctime>

int main(void)
{
	Servo s1(0);

	s1 = 0.25;

	sleep(1);

	s1 = 0.5;

	sleep(1);

	s1 = 0.75;

	return 0;
}
