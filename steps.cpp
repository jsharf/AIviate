#include "control.h"
#include "steps.h"
#include "step_globals.h"
#include "Filters.h"
#include "PIDControl.h"
#include "mbed.h"

extern Serial pc;
Timer pid_timer;

int main_proc(int pid)
{
    // schedule process to initialize i2c sensors
    schedule_proc("INITSENSE", &init_sensors);
    
    return PROC_BREAK;
}

// in the future, change get_sensor_data to append the sensor data to a rolling list 
int get_sensor_data(int pid)
{
    OSTATIC struct sensor s;
    int control_pid = find_proc("CONTROL"); // the process identifier of the processes which handles the control algorithm
    if (control_pid == -1)
    {
        if (schedule_proc("CONTROL", pid_control) < 0)
        {
            if(USBDEBUG)
                pc.printf("Couldn't start control process\r\n");    
        }   
        if(USBDEBUG)
            pc.printf("Added new process control\r\n");
        
        control_pid = find_proc("CONTROL");
    }

    
    // retrieve process database
    db *m_db = get_db(pid);
    
    OSTATIC int xzero_i = search_db(m_db, "gx0");
    OSTATIC int yzero_i = search_db(m_db, "gy0");
    OSTATIC int zzero_i = search_db(m_db, "gz0");
    if (xzero_i == -1 || yzero_i == -1 || zzero_i == -1)
    {
        schedule_proc("INITSENSE", &init_sensors);
        return PROC_BREAK;
    }
    
    //print_db(m_db);
    
    s.gx0 = m_db->records[xzero_i].val;
    s.gy0 = m_db->records[yzero_i].val;
    s.gz0 = m_db->records[zzero_i].val; 
    
    if (sensor_read_accelerometer(&s) == 0 || sensor_read_gyro(&s) == 0)
    {
        if (USBDEBUG)
            pc.printf("Error in get_sensor_data while reading from accel/gyro!\r\n");
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
    
    
    
    OSTATIC db* pid_db = get_db(control_pid);

    set(pid_db, "ax", s.ax);
    set(pid_db, "ay", s.ay);
    set(pid_db, "az", s.az);
    
    set(pid_db, "gx", s.gx);
    set(pid_db, "gy", s.gy);
    set(pid_db, "gz", s.gz);
    
    //print_db(pid_db);

    // TESTY
    //pc.printf("set sensor values in database\r\n");

    // pc.printf("Done
    return PROC_CONTINUE;
}

int init_sensors(int pid)
{
    pid_timer.start();
    
    //set_time(0);
    
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
    int32_t gx = 0, gy = 0, gz = 0;
    struct sensor s;
    
    s.gx0 = s.gy0 = s.gz0 = 0;
    
    int32_t numzerotrials = ZERO_MEASURE_PERIOD;
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

    schedule_proc("GETDATA", &get_sensor_data);    
    int dpid = find_proc("GETDATA");
    if (dpid == -1)
    {
        if (USBDEBUG)
            pc.printf("Critical Err: Getdata scheduled but find_proc can't find it (wtf?!) -- maybe proc overflow good luck\r\n");
        return PROC_BREAK;    
    }
    db *my_db = get_db(dpid);
    set(my_db, "gx0", gx);
    set(my_db, "gy0", gy);
    set(my_db, "gz0", gz);
    if (USBDEBUG)
    {
        pc.printf("init: %i %i %i\r\n", gx, gy, gz);
        print_db(my_db);
    }
    
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

