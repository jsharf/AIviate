#include "../Servo/Servo.h"
#include <iostream>
#include <cmath>

using namespace std;

void usage(void)
{
	cerr<<"Usage:\n\tservo_test (servo_number) (position)"<<endl;
}

int main(int argc, char **argv)
{
	if(argc == 3)
	{
		long int servo_num = 0;
		float servo_pos = 0;
		servo_num = strtol(argv[1], NULL, 10);
		servo_pos = strtof(argv[2], NULL);
		
		cerr<<servo_num<<", "<<servo_pos<<endl;

		Servo serv(servo_num);
		serv = min(max(servo_pos, 0.0f), 1.0f);
	}
	else
	{
		usage();
	}

	return 0;
}
