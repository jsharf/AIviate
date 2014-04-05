#include "control.h"
using namespace std;

/*
   Control Loop
   ============

Usage:
control <in_port> <out_addr> <out_port>

This program listens on port <in_port> for incoming UDP packets with sensor
data. It applies control logic to the data and outputs commands to
<out_addr> at port <out_port>. In almost all cases, out_addr will be
localhost (127.0.0.1). But we left it an option so that you can simulate
testing on an external computer. This is useful for running the program
physically on the Raspberry Pi, but testing it on a more powerful computer
over the network.
 */

/*
   CONSTANTS
   =========

   Ideally, this should be put in a header file
   These are sort of self-explanatory
 */

void usage()
{
    cerr << "control <in_port> <out_addr> <out_port>" << endl;
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        cerr << "Wrong number of arguments" << endl;
        usage();
        return 1;
    }

    UDPListener lst(argv[1]);
    UDPSender snd(argv[2], argv[3]);
    control out_control;

    sensorf in_data;

    while (true)
    {
        string a = lst.listen();
        if (a != "FAIL")
        {
            // note that this is a horrible mistake to confuse
            // the heading for the yaw
            lst.receiveSensor(in_data);
	    cerr << "SENSORF: \n{\n\t ax: " \
	    << in_data.ax << "\n\t ay: " << in_data.ay << "\n\t az: " << in_data.az \
	    << "\n\t gx: " << in_data.gx << "\n\t gy: " << in_data.gy << "\n\t gz: " << in_data.gz \
	    << "\n\t mx: " << in_data.mx << "\n\t my: " << in_data.my << "\n\t mz: " << in_data.mz \
	    << "\n\t altitude: " << in_data.altitude \
	    << "\n}" << endl;
            out_control.ail = 2*in_data.ax/512;
            out_control.elev = 2*in_data.ay/512 + 0.5;
            out_control.rudder = atan2(in_data.my, in_data.mx)/3.14159265359f;
            out_control.throttle = 1.0f;
            snd.sendControl(out_control);
            //cout << in_data << endl;
            //pid_control(in_data, ctrl);
            //snd.sendControl(ctrl);
        }
    }
}

/*int pid_control(sensorf &data, control &ctrl)
{
    static ComplementaryFilter rollFilter(rollFK), pitchFilter(pitchFK);
    static PIDController rollPID(rollKP, rollKI, rollKD), pitchPID(pitchKP, pitchKI, pitchKD);

    float pid_dt = pid_timer.read_us();
    pid_timer.reset();
    pid_dt /= 1000000;

    // get database of PID process
    OSTATIC db* pid_db = get_db(pid);

    OSTATIC int ax_i = search_db(pid_db, "ax");
    OSTATIC int ay_i = search_db(pid_db, "ay");
    OSTATIC int az_i = search_db(pid_db, "az");

    OSTATIC int gx_i = search_db(pid_db, "gx");
    OSTATIC int gy_i = search_db(pid_db, "gy");
    OSTATIC int gz_i = search_db(pid_db, "gz");

    // if no accelerometer data present, fail
    if (ax_i == -1 || ay_i == -1 || az_i == -1 || gx_i == -1 || gy_i == -1 || gz_i == -1)
    {
        if(USBDEBUG)
            pc.printf("Could not find ax and ay in getdata's db\r\n");
        return PROC_BREAK;
    }
    float ax = (float) pid_db->records[ax_i].val;
    float ay = (float) pid_db->records[ay_i].val;
    float az = (float) pid_db->records[az_i].val;

    float gx = (float) pid_db->records[gx_i].val;
    float gy = (float) pid_db->records[gy_i].val;
    float gz = (float) pid_db->records[gz_i].val;

    ax /= ACCEL_MAGNITUDE;
    ay /= ACCEL_MAGNITUDE;
    az /= ACCEL_MAGNITUDE;

    gx /= GYRO_MAGNITUDE;
    gy /= GYRO_MAGNITUDE;
    gz /= GYRO_MAGNITUDE;

    //float pitch = 1000*atan2(az, ax);
    //float roll = 1000*atan2(az, ay);

    float accelAngY = atan2(az, ax);
    float accelAngX = atan2(az, ay);

    accelAngX -= PI/2;
    if(accelAngX < -PI)
    {
        accelAngX += 2*PI;
    }
    accelAngY -= PI/2;
    if(accelAngY < -PI)
    {
        accelAngY += 2*PI;
    }

    float pitch = pitchFilter.calculate(accelAngY, gy, pid_dt);
    float roll = -rollFilter.calculate(accelAngX, -gx, pid_dt);

    //set(pid_db, "pitch", (int)pitch);
    //set(pid_db, "roll", (int)roll);

    //pc.printf("Roll: %+8f | Pitch: %+8f | AccelAngX: %+8f | AccelAngY: %+8f | GyroAngX: %+8f | GyroAngY: %+8f\r\n",roll,pitch,accelAngX,accelAngY,-gx,gy);

    //print_db(pid_db);

    //pc.printf("Angles: %f %f\r\n", pitch, roll);

    // X-axis control via ailerons
    float aileron_out = rollPID.calculate(roll, 0, pid_dt);

    // big fancy line that controls the PWM to ailerons
    AILERON_CONTROL = aileron_out + AileronCenter;

    float elevator_out = pitchPID.calculate(pitch, 0, pid_dt);

    //pc.printf("Results: %f %f %f\r\n", elevator_out, aileron_out, pid_dt);

    // big fancy line that controls the PWM to elevators
    ELEVATOR_CONTROL = elevator_out + ElevatorCenter;

    return PROC_CONTINUE;
}*/

