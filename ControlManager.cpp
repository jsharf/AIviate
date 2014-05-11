//#include "ControlManager.h"
#include "Comm/Control.h"
#include "Vector/Vector.h"
#include "Comm/Comm.h"
#include "Comm/Sensor.h"
#include "Filters/Filters.h"
#include "PIDControl/PIDControl.h"
#include <limits>
using namespace std;

void pid_control(PlaneState &data, control &ctrl, float delta);

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
    //sensorf in_data;
    PlaneState in_state;

    clock_t prev_time = clock(), cur_time;
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
            lst.receivePlaneState(in_state);

            // Write debug output
            /*cerr << "SENSORF: \n{\n\t ax: " \
            << in_data.ax << "\n\t ay: " << in_data.ay << "\n\t az: " << in_data.az \
            << "\n\t gx: " << in_data.gx << "\n\t gy: " << in_data.gy << "\n\t gz: " << in_data.gz \
            << "\n\t mx: " << in_data.mx << "\n\t my: " << in_data.my << "\n\t mz: " << in_data.mz \
            << "\n\t altitude: " << in_data.altitude \
            << "\n}" << endl;
*/
            // Recalculate delta
            // TODO: put timestamp on sensor packets
            //       so we can get true delta
            cur_time = clock();
            delta = cur_time-prev_time;
            delta /= CLOCKS_PER_SEC;
            prev_time = cur_time;

            // Run the PID loop
	    //cout << "DT: " << delta << endl;
            pid_control(in_state, out_control, delta);

            // Send the control structure
            snd.sendControl(out_control);

	//	snd.sendPlaneState(in_state);
        //    cout << "X: " << in_state.orientation.x << "Y: " << in_state.orientation.y << "Z: " << in_state.orientation.z << "W: " << in_state.orientation.w << endl; 
	}
        else
        {
            ;//	cerr << "FAILING TO READ!!! Is ai-sensor started?" << endl;
        }
    }
}

/// write the euler angles into the references
// zRad is yaw
// yRad is pitch
// xRad is roll
// as per simgear/flightgear conventions
void getEulerRad(const Quaternion& o, fp_type& zRad, fp_type& yRad, fp_type& xRad)
{
    double w = o.w;
    double x = o.x;
    double y = o.y;
    double z = o.z;
    
    fp_type sqrQW = w*w;
    fp_type sqrQX = x*x;
    fp_type sqrQY = y*y;
    fp_type sqrQZ = z*z;

    fp_type num = 2*(y*z + w*x);
    fp_type den = sqrQW - sqrQX - sqrQY + sqrQZ;
    if (fabs(den) <= std::numeric_limits<fp_type>::min() &&
        fabs(num) <= std::numeric_limits<fp_type>::min())
      xRad = 0;
    else
      xRad = atan2(num, den);

    fp_type tmp = 2*(x*z - w*y);
    if (tmp <= -1)
      yRad = fp_type(0.5)*PI;
    else if (1 <= tmp)
      yRad = -fp_type(0.5)*PI;
    else
      yRad = -asin(tmp);

    num = 2*(x*y + w*z);
    den = sqrQW + sqrQX - sqrQY - sqrQZ;
    if (fabs(den) <= std::numeric_limits<fp_type>::min() &&
        fabs(num) <= std::numeric_limits<fp_type>::min())
      zRad = 0;
    else {
      fp_type psi = atan2(num, den);
      if (psi < 0)
        psi += 2*PI;
      zRad = psi;
    }
}

/// get the euler angles in degrees
void getEulerDeg(const Quaternion& o, fp_type& yaw, fp_type& pitch, fp_type& roll)
{
    getEulerRad(o, yaw, pitch, roll);
    yaw = RAD2DEG(yaw);
    pitch = RAD2DEG(pitch);
    roll = RAD2DEG(roll);
}

// Values should be handed over in m/s/s, rad/s
void pid_control(PlaneState &data, control &ctrl, float delta)
{
    // Filter Constants
    static float rollFK = 0.5;
    static float pitchFK = 0.5;
    static float yawFK = 0.5;

    // PID Roll Constants
    static float rollKP = 1;
    static float rollKI = 0;
    static float rollKD = 0;

    // PID Pitch Constants
    static float pitchKP = 1;
    static float pitchKI = 0;
    static float pitchKD = 0;

    // PID Yaw Constants
    static float yawKP = 1;
    static float yawKI = 0;
    static float yawKD = 0;

    static ComplementaryFilter rollFilter(rollFK), pitchFilter(pitchFK),
    yawFilter(yawFK);
    static PIDController rollPID(rollKP, rollKI, rollKD), pitchPID(pitchKP,
    pitchKI, pitchKD), yawPID(yawKP, yawKI, yawKD);

/*    Vector3d accelVec(data.ax, data.ay, data.az), magVec(data.mx, data.my, data.mz);

    //double accelVecMag = accelVec.magnitude();
    accelVec = accelVec.unit();

    //double magVecMag = magVec.magnitude();
    magVec = magVec.unit();

    // Project the acceleration vector into the xz and yz planes
    Vector2d xzAccelVec(accelVec.x, accelVec.z);
    Vector2d yzAccelVec(accelVec.y, accelVec.z);

    // Compute roll and pitch by comparing with the vertical (j vector)
    double accelRoll = yzAccelVec.angleTo(Vector2d::j);
    double accelPitch = xzAccelVec.angleTo(Vector2d::j);
    double accelAngZ = 0;

    // Calculate roll pitch and yaw with filters
    float pitch = pitchFilter.calculate(accelPitch, data.gy, delta);
    float roll = -rollFilter.calculate(accelRoll, -data.gx, delta);
    //float yaw = yawFilter.calculate(accelAngZ, data.gz, delta);
*/
    /*
    double angle = 0;
    Vector3d gravityVec = data.orientation.getAxisAngle(angle);
    
    Vector2d gravityVecXY = Vector2d(gravityVec.x, gravityVec.y);
    Vector2d gravityVecXZ = Vector2d(gravityVec.x, gravityVec.z);
    Vector2d gravityVecYZ = Vector2d(gravityVec.y, gravityVec.z);
    double roll = 
    double pitch = gravityVecXZ.angleTo(Vector2d::j)/PI;
    double yaw = gravityVecXY.angleTo(Vector2d::i)/PI;
    */

    double roll, pitch, yaw;
    roll = pitch = yaw = 0;

    getEulerDeg(data.orientation, yaw, pitch, roll);

    cout << "Yaw: " << yaw << "\t Pitch: " << pitch << "\t Roll: " << roll <<
    std::endl;

    //pc.printf("Roll: %+8f | Pitch: %+8f | AccelAngX: %+8f | AccelAngY: %+8f | GyroAngX: %+8f | GyroAngY: %+8f\r\n",roll,pitch,accelAngX,accelAngY,-gx,gy);

    //std::cout << "Angles: " << accelPitch << "\t" << data.gy << "\t" << accelRoll << "\t" << -data.gx << "\t" << pitch << "\t" << roll << std::endl;

    // X-axis control via ailerons
    float aileron_out = rollPID.calculate(roll, 0, delta);
    ctrl.ail = aileron_out; //+ AileronCenter;

    // Y-axis control via elevators
    float elevator_out = pitchPID.calculate(pitch, 0, delta);
    ctrl.elev = elevator_out; //+ ElevatorCenter;

    //float rudder_out = yawPID.calculate(yaw, 0, delta);
    //ctrl.rudder = rudder_out; // default value (to be changed)

    // default value, change this later when we get throttling figured out
    ctrl.throttle = 1.0f;
}

