#include "control.h"\
#include "Vector/Vector.h"
using namespace std;

void pid_control(sensorf &data, control &ctrl, float delta);

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

    time_t prev_time = time(NULL), cur_time;
    float delta;

    // TODO: add a check for initial recepion of data to prevent first delta
    // from being very large (i.e., if ai-sensor is started after ai-control).
    // COMMENT: not really necessary if it's just once. Also not 100% sure I
    // like this behavior
    while (true)
    {
        string a = lst.listen();
        if (a != "FAIL")
        {
            // Listen for sensor data
            lst.receiveSensor(in_data);

            // Write debug output
            cerr << "SENSORF: \n{\n\t ax: " \
            << in_data.ax << "\n\t ay: " << in_data.ay << "\n\t az: " << in_data.az \
            << "\n\t gx: " << in_data.gx << "\n\t gy: " << in_data.gy << "\n\t gz: " << in_data.gz \
            << "\n\t mx: " << in_data.mx << "\n\t my: " << in_data.my << "\n\t mz: " << in_data.mz \
            << "\n\t altitude: " << in_data.altitude \
            << "\n}" << endl;

            // Recalculate delta
            // TODO: put timestamp on sensor packets
            //       so we can get true delta
            cur_time = time(NULL);
            delta = difftime(cur_time, prev_time);
            prev_time = cur_time;

            // Run the PID loop
            pid_control(in_data, out_control, delta);

            // Send the control structure
            snd.sendControl(out_control);
        }
    }
}


// Values should be handed over in m/s/s, rad/s
void pid_control(sensorf &data, control &ctrl, float delta)
{
    // Filter Constants
    static float rollFK = 0.5;
    static float pitchFK = 0.5;
    static float yawFK = 0.5;

    // PID Roll Constants
    static float rollKP = 1;
    static float rollKI = 0;
    static float rollKD = 0.1;

    // PID Pitch Constants
    static float pitchKP = 1;
    static float pitchKI = 0;
    static float pitchKD = 0.1;

    // PID Yaw Constants
    static float yawKP = 1;
    static float yawKI = 0;
    static float yawKD = 0.1;

    static ComplementaryFilter rollFilter(rollFK), pitchFilter(pitchFK),
    yawFilter(yawFK);
    static PIDController rollPID(rollKP, rollKI, rollKD), pitchPID(pitchKP,
    pitchKI, pitchKD), yawPID(yawKP, yawKI, yawKD);

    Vector3d accelVec(data.ax, data.ay, data.az), magVec(data.mx, data.my, data.mz);

    accelVecMag = accelVec.magnitude();
    accelVec = accelVec.unit();

    magVecMag = magVec.magnitude();
    magVec = magVec.unit();

    // Project the acceleration vector into the xz and yz planes
    Vector2d xzAccelVec(accelVec.x, accelVec.z);
    Vector2d yzAccelVec(accelVec.y, accelVec.z);

    // Compute roll and pitch by comparing with the vertical (j vector)
    accelRoll = yzAccelVec.angleTo(Vector2d::j);
    accelPitch = xzAccelVec.angleTo(Vector2d::j);

    // Calculate roll pitch and yaw with filters
    float pitch = pitchFilter.calculate(accelPitch, gy, delta);
    float roll = -rollFilter.calculate(accelRoll, -gx, delta);
    float yaw = yawFilter.calculate(accelAngZ, gz, delta);


    //pc.printf("Roll: %+8f | Pitch: %+8f | AccelAngX: %+8f | AccelAngY: %+8f | GyroAngX: %+8f | GyroAngY: %+8f\r\n",roll,pitch,accelAngX,accelAngY,-gx,gy);

    std::cout << "Angles: " << pitch << "\t" << roll << std::endl;

    // X-axis control via ailerons
    float aileron_out = rollPID.calculate(roll, 0, delta);
    ctrl.ail = aileron_out; //+ AileronCenter;

    // Y-axis control via elevators
    float elevator_out = pitchPID.calculate(pitch, 0, delta);
    ctrl.elev = elevator_out; //+ ElevatorCenter;

    ctrl.rudder = 0; // default value (to be changed)

    // default value, change this later when we get throttling figured out
    ctrl.throttle = 1.0f;
}

