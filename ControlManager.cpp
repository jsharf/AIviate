//#include "ControlManager.h"
#include "Comm/Control.h"
#include "Vector/Vector.h"
#include "Comm/Comm.h"
#include "Sensor/Sensor.h"
#include "Filters/Filters.h"
#include "PIDControl/PIDControl.h"
#include <limits>
#include <string>
using namespace std;

void pid_control(PlaneState &data, control &ctrl, float delta);

static int SENSOR_SIMU = 0;

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
    if (argc != 4 && argc != 5)
    {
        cerr << "Wrong number of arguments" << endl;
        usage();
        return 1;
    }

    if (argc == 5 && string(argv[4]) == "-d")
        SENSOR_SIMU = 1;

    UDPListener lst(argv[1]);
    UDPSender snd(argv[2], argv[3]);

    control out_control;
    sensorf in_data;
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
            // and simulate the filtering algorithms on it
            if (SENSOR_SIMU)
            {
                lst.receiveSensor(in_data);
                fp_type phi, theta, psi;
                phi = in_data.mx;
                theta = in_data.my;
                psi = in_data.mz;
              
                fp_type roll, pitch, yaw;
                roll = phi;
                pitch = theta;
                yaw = psi;
               
                Quaternion orientation(yaw, pitch, roll);
                Vector3d mag = Vector3d::i.rotate(orientation);
                Quaternion opp = mag.quaternionTo(Vector3d::i);
                mag = Vector3d::i.rotate(opp);
                in_data.mx = mag.x; in_data.my = mag.y; in_data.mz = mag.z;
                
                // delta isn't implemented properly, use timesteps of 1
                
                sensorf_to_planestate(in_data, in_state, 1);
                cout << "DEBUG" << endl;
            } 
            else
            {
                // else just receive clean orientation data
                lst.receivePlaneState(in_state);
            }
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
            // delta isn't properly implemented, use timesteps of 1
            pid_control(in_state, out_control, 1);
            
            // Send the control structure
            snd.sendControl(out_control);


        //    cout << "X: " << in_state.orientation.x << "Y: " << in_state.orientation.y << "Z: " << in_state.orientation.z << "W: " << in_state.orientation.w << endl; 
        }
        else
        {
            ;//	cerr << "FAILING TO READ!!! Is ai-sensor started?" << endl;
        }
    }
}

/// get the euler angles in degrees
void getEulerDeg(const Quaternion& o, fp_type& yaw, fp_type& pitch, fp_type& roll)
{
    o.getEulerRad(yaw, pitch, roll);
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
    static float rollKP = 5;
    static float rollKI = 0;
    static float rollKD = -1;

    // PID Pitch Constants
    static float pitchKP = 5;
    static float pitchKI = 0;
    static float pitchKD = -1;

    // PID Yaw Constants
    static float yawKP = 10;
    static float yawKI = 0;
    static float yawKD = -1;

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

    //pc.printf("Roll: %+8f | Pitch: %+8f | AccelAngX: %+8f | AccelAngY: %+8f | GyroAngX: %+8f | GyroAngY: %+8f\r\n",roll,pitch,accelAngX,accelAngY,-gx,gy);

    //std::cout << "Angles: " << accelPitch << "\t" << data.gy << "\t" << accelRoll << "\t" << -data.gx << "\t" << pitch << "\t" << roll << std::endl;

    // X-axis control via ailerons
    float aileron_out = rollPID.calculate(roll, 0, delta);
    if (aileron_out > 90 && aileron_out < -90)
        aileron_out = 180-aileron_out;
    ctrl.ail = aileron_out/90.0; //+ AileronCenter;
    if (ctrl.ail > 1)
        ctrl.ail = 1;
    if (ctrl.ail < -1)
        ctrl.ail = -1;

    // Y-axis control via elevators
    float elevator_out = pitchPID.calculate(pitch, 0, delta);
    ctrl.elev = -1.0*elevator_out/90.0f; //+ ElevatorCenter;
    if (ctrl.elev > 1)
        ctrl.elev = 1;
    if (ctrl.elev < -1)
        ctrl.elev = -1;

    double yaw_target = 0;
    if (yaw > 180)
        yaw_target = 360;
    float rudder_out = yawPID.calculate(yaw, yaw_target, delta);
    ctrl.rudder = (rudder_out)/180.0; // default value (to be changed)
    if (ctrl.rudder > 1)
        ctrl.rudder = 1;
    if (ctrl.rudder < -1)
        ctrl.rudder = -1;

    // default value, change this later when we get throttling figured out
    ctrl.throttle = 1.0f;
    
    cout << "Yaw: " << yaw << "\t Pitch: " << pitch << "\t Roll: " << roll <<
    "\t ail: " << ctrl.ail << "\t elev: " << ctrl.elev << "\t rudder: " <<
    ctrl.rudder << std::endl;

}
