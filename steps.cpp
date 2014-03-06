#include "control.h"
#include "steps.h"
#include "step_globals.h"
#include "Filters/Filters.h"
#include "PIDControl/PIDControl.h"
#include "mbed.h"

extern Serial pc;
Timer pid_timer;

int main_proc(int pid)
{
    // schedule process to initialize i2c sensors
    schedule_proc("INITSENSE", &init_sensors);

    set_time(0);

    return PROC_BREAK;
}

// in the future, change get_sensor_data to append the sensor data to a rolling list
int get_sensor_data(int pid)
{
    struct sensor s;

    //pc.printf("GET SENSOR proc running!\r\n");

    // retrieve process database
    db *m_db = get_db(pid);
    /** GYRO VALUES
    int xzero_i = search_db(m_db, "gx0");
    int yzero_i = search_db(m_db, "gy0");
    int zzero_i = search_db(m_db, "gz0");
    if (xzero_i == -1 || yzero_i == -1 || zzero_i == -1)
    {
        // probably first time running process, retrieve data from init_sensors and store in current process output buffer
        int init_pid = find_proc("INITSENSE");
        if (init_pid == -1)
        {
            if (USBDEBUG)
                pc.printf("INITSENSE not found, was init called? (get_sensor_data)\r\n");
            return 1;
        }
        // copy gyro zero-level values from init_sensors to current process db
        db* init_db = get_db(init_pid);
        xzero_i = search_db(init_db, "gx0");
        yzero_i = search_db(init_db, "gy0");
        zzero_i = search_db(init_db, "gz0");
        if (xzero_i == -1 || yzero_i == -1 || zzero_i == -1)
        {
            pc.printf("Critical error: gyro_zero vals not found \r\n");
            print_all_db();
            schedule_proc("INITSENSE", &init_sensors);
            return 0;
        }
        int gx0, gy0, gz0;
        gx0 = init_db->records[xzero_i].val;
        gy0 = init_db->records[yzero_i].val;
        gz0 = init_db->records[zzero_i].val;
        set(m_db, "gx0", gx0);
        set(m_db, "gy0", gy0);
        set(m_db, "gz0", gz0);
        pc.printf("getdata: %i %i %i\r\n", gx0, gy0, gz0);
    }

    s.gx0 = m_db->records[xzero_i].val;
    s.gy0 = m_db->records[yzero_i].val;
    s.gz0 = m_db->records[zzero_i].val;
    */

    // TESTY
    //pc.printf("about to read accel\r\n");

    if (sensor_read_accelerometer(&s) == 0 || sensor_read_gyro(&s) == 0)
    {
        //if (USBDEBUG)
            //pc.printf("Error in get_sensor_data while reading from accel!\r\n");
        return PROC_BREAK;
    }

    // TESTY
    //pc.printf("finished reading accel\r\n");

    /*
    if (read_gyro(&s) == 0)
    {
        if (USBDEBUG)
            pc.printf("Error in get_sensor_data while reading from gyro!\r\n");
        return 1;
    }
    if (read_compass(&s) == 0)
    {
        if (USBDEBUG)
            pc.printf("Error in get_sensor_data while reading from compass!\r\n");
        return 1;
    }
    */

    set(m_db, "ax", s.ax);
    set(m_db, "ay", s.ay);
    set(m_db, "az", s.az);

    set(m_db, "gx", s.gx);
    set(m_db, "gy", s.gy);
    set(m_db, "gz", s.gz);

    print_db(m_db);

    // TESTY
    //pc.printf("set sensor values in database\r\n");


        int control_pid = find_proc("CONTROL"); // the process identifier of the processes which handles the control algorithm
        if (control_pid == -1)
        {
            // TESTY
            //pc.printf("Adding new proc CONTROL\r\n");
            if (schedule_proc("CONTROL", pid_control) < 0)
            {
                //if(USBDEBUG)
                    //pc.printf("Couldn't start control process\r\n");
            }
            //pc.printf("Added new process control\r\n");

        }
        else
        {
            //pc.printf("control already exists\r\n");
        }

    // pc.printf("Done
    return PROC_CONTINUE;
}

// oh god, process identifier (PID) vs proportional integral derivative (PID)
// is gonna be soooooo confusing
// this is the PID control algorithm, and takes as input its process identifier, like any other process.
// a couple globals are used here because it's faster
int pid_control(int pid)
{
    static ComplementaryFilter rollFilter(rollFK), pitchFilter(pitchFK);
    static PIDController rollPID(rollKP, rollKI, rollKD), pitchPID(pitchKP, pitchKI, pitchKD);

    float pid_dt = pid_timer.read_us();
    pid_timer.reset();
    pid_dt /= 1000000;

    //pc.printf("CONTROL proc running!\r\n");
    int sensorpid = find_proc("GETDATA");
    if (sensorpid == -1)
    {
        if(USBDEBUG)
            pc.printf("Could not find proc GETDATA\r\n");
        // no sensor process. failure!!!
        return PROC_BREAK;
    }

    // get database containing sensor data (from get_sensor_data)
    static db* data_db = get_db(sensorpid);

    static int ax_i = search_db(data_db, "ax");
    static int ay_i = search_db(data_db, "ay");
    static int az_i = search_db(data_db, "az");

    static int gx_i = search_db(data_db, "gx");
    static int gy_i = search_db(data_db, "gy");
    static int gz_i = search_db(data_db, "gz");

    // if no accelerometer data present, fail
    if (ax_i == -1 || ay_i == -1 || az_i == -1 || gx_i == -1 || gy_i == -1 || gz_i == -1)
    {
        if(USBDEBUG)
            pc.printf("Could not find proc ax and ay in getdata's db\r\n");
        return PROC_BREAK;
    }
    float ax = (float) data_db->records[ax_i].val;
    float ay = (float) data_db->records[ay_i].val;
    float az = (float) data_db->records[az_i].val;

    float gx = (float) data_db->records[gx_i].val;
    float gy = (float) data_db->records[gy_i].val;
    float gz = (float) data_db->records[gz_i].val;

    ax /= ACCEL_MAGNITUDE;
    ay /= ACCEL_MAGNITUDE;
    az /= ACCEL_MAGNITUDE;

    gx /= GYRO_MAGNITUDE;
    gy /= GYRO_MAGNITUDE;
    gz /= GYRO_MAGNITUDE;

    //float pitch = pitchFilter.calculate(atan2(az, ay), gx, pid_dt);
    //float roll = rollFilter.calculate(atan2(az, ax), gy, pid_dt);
    float pitch = atan2(az, ax);
    float roll = atan2(az, ay);
    pitch -= PI/2;
    if(pitch < -PI)
    {
        pitch += 2*PI;
    }
    roll -= PI/2;
    if(roll < -PI)
    {
        roll += 2*PI;
    }

    //pc.printf("Angles: %f %f\r\n", pitch, roll);

    // X-axis control via ailerons
    float aileron_out = rollPID.calculate(roll, 0, pid_dt);

    // big fancy line that controls the PWM to ailerons
    AILERON_CONTROL = aileron_out + 0.5f;

    float elevator_out = pitchPID.calculate(pitch, 0, pid_dt);

    //pc.printf("Results: %f %f %f\r\n", elevator_out, aileron_out, pid_dt);

    // big fancy line that controls the PWM to elevators
    ELEVATOR_CONTROL = elevator_out + 0.5f;

    return PROC_CONTINUE;
}

int init_sensors(int pid)
{
    pid_timer.start();
    // create config struct
    struct config c;

    // set configurations
    c.frequency = 400000;

    // pass configuration struct to configuration routine
    int ret = sensor_config_gy80(&c);
    if (ret == 0)
    {
        if (USBDEBUG)
            pc.printf("Error configuring sensors\r\n");
        return PROC_BREAK;
    }

    pc.printf("Configuring sensors\r\n");



    // accumulations of gyro values (for zeroing)
    int gx = 0, gy = 0, gz = 0;
    struct sensor s;
    int numzerotrials = ZERO_MEASURE_PERIOD;
    for (int i=0; i<numzerotrials; i++)
    {
        if (sensor_read_gyro(&s) == 0)
        {
            if (USBDEBUG)
                pc.printf("Error in collecting zero-level data from gyro (init_sensors)\r\n");
            return PROC_CONTINUE;
        }
        gx += s.gx;
        gy += s.gy;
        gz += s.gz;
    }

    gx /= numzerotrials;
    gy /= numzerotrials;
    gz /= numzerotrials;

    db *my_db = get_db(pid);
    set(my_db, "gx0", gx);
    set(my_db, "gy0", gy);
    set(my_db, "gz0", gz);
    pc.printf("init: %i %i %i\r\n", gx, gy, gz);
    print_db(my_db);
    schedule_proc("GETDATA", &get_sensor_data);

    return PROC_BREAK;
}

int sensor_fusion(int pid)
{
    int *data;
    int n;
    // get data from output of get_sensor_data

    // this needs to be written!

    //romberg(data, n, &trap);
    return PROC_CONTINUE;
}

