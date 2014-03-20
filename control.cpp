#include <iostream>
#include <unistd.h>
#include <string>
#include <sys/types.h> 
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netdb.h>
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
const int PACKETSIZE = 2048;


void usage()
{
    cerr << "control <in_port> <out_addr> <out_port>" << endl;
}

void error(const string msg)
{
    cerr << msg;
}
    
int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        cerr << "Wrong number of arguments" << endl;
        usage();
        return 1;
    }

    sockaddr_in in_addr;
    sockaddr_in out_addr;
   
    struct sockaddr_in remaddr; /* remote address */
    socklen_t addrlen = sizeof(remaddr); /* length of addresses */
    
    int in_sockfd, out_sockfd;

    in_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    out_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
   
    if (in_sockfd < 0) 
    {
        cerr << "ERROR opening socket" << endl;
        return 1;
    }

    if (out_sockfd < 0) 
    {
        cerr << "ERROR opening socket" << endl;
        return 1;
    }
    
    // set nonblocking mode
    int flags = fcntl(in_sockfd, F_GETFL);
    flags |= O_NONBLOCK;
    fcntl(in_sockfd, F_SETFL, flags);
  
    flags = fcntl(out_sockfd, F_GETFL); 
    flags |= O_NONBLOCK;
    fcntl(out_sockfd, F_GETFL); 
    
    bzero((char *) &in_addr, sizeof(in_addr));
    int portno = atoi(argv[1]);
    in_addr.sin_family = AF_INET;
    in_addr.sin_addr.s_addr = INADDR_ANY;
    in_addr.sin_port = htons(portno);

    struct hostent *hp;   
 
    hp = gethostbyname(argv[2]);
    if (!hp) {
        fprintf(stderr, "could not obtain address of %s\n", argv[2]);
        return 0;
    }
    
    bzero((char *) &out_addr, sizeof(out_addr));
    int outport = atoi(argv[3]);
    out_addr.sin_family = AF_INET;
    out_addr.sin_port = htons(outport);
    /* put the host's address into the server address structure */
    memcpy((void *)&out_addr.sin_addr, hp->h_addr_list[0], hp->h_length);

    if (!hp) {
        fprintf(stderr, "could not obtain address of %s\n", argv[3]);
        return 0;
    }

    /* put the host's address into the server address structure */
    memcpy((void *)&out_addr.sin_addr, hp->h_addr_list[0], hp->h_length);

    if (bind(in_sockfd, (struct sockaddr *) &in_addr,
        sizeof(in_addr)) < 0) 
    {
        error("ERROR on binding"); 
        return 1;
    }
    
    unsigned char buf[PACKETSIZE];
    int recvlen;
    
    cout << "Listening on port " << portno << endl;
    cout << "Output on port " << outport << endl;

    // forever is a long time...
    while (true)
    {
        // wait for incoming sensor packets
        recvlen = recvfrom(in_sockfd, buf, PACKETSIZE, 0, 
            (struct sockaddr*)&remaddr, &addrlen);
        if (recvlen > 0) 
        {
            buf[recvlen] = 0;
            string packet((char *)buf);
            cout << "Received Message:" << packet << endl;
        }
        if (sendto(out_sockfd, "derpderpderp", 13, 0, (struct sockaddr *)&out_addr, sizeof(out_addr)) < 0) {
            perror("sendto failed");
            return 1;
        }
    }
    return 0;
}

// oh god, process identifier (PID) vs proportional integral derivative (PID)
// is gonna be soooooo confusing
// this is the PID control algorithm, and takes as input its process identifier, like any other process.
// a couple globals are used here because it's faster
/*
int pid_control(int pid)
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
}
*/
