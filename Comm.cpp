#include "Comm.h"
using namespace std;
const int PACKETSIZE = 2048;

UDPListener::UDPListener(string port, int debug) : mDebug(debug)
{
    sockaddr_in in_addr;
    in_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (in_sockfd < 0) 
    {
        if (mDebug) 
            cerr << "ERROR opening socket" << endl;
        return;
    }
    // set nonblocking mode
    int flags = fcntl(in_sockfd, F_GETFL);
    flags |= O_NONBLOCK;
    fcntl(in_sockfd, F_SETFL, flags);

    bzero((char *) &in_addr, sizeof(in_addr));
    int portno = atoi(port.c_str());
    in_addr.sin_family = AF_INET;
    in_addr.sin_addr.s_addr = INADDR_ANY;
    in_addr.sin_port = htons(portno);

    if (::bind(in_sockfd, (struct sockaddr *) &in_addr, sizeof(in_addr)) < 0) 
    {
        if (mDebug)
            cerr << "ERROR on binding" << endl; 
        return;
    }
    if (mDebug)
        cerr << "Listening on port " << portno << endl;
}

string UDPListener::listen() const
{
    unsigned char buf[PACKETSIZE];
    socklen_t addrlen = sizeof(remaddr);
    int recvlen = recvfrom(in_sockfd, buf, PACKETSIZE, 0, 
            (struct sockaddr*)&remaddr, &addrlen);
    if (recvlen > 0) 
    {
        if (recvlen < PACKETSIZE)
            buf[recvlen] = 0;
        else
            buf[PACKETSIZE-1] = 0;
        string packet((char *)buf);
        if (mDebug)
            cerr << "Received Message:" << packet << endl;
        return packet;
    }
    else
    {
        return "FAIL";
    }
}

int UDPListener::receiveSensor(sensorf &in_data) const
{
    string packet = listen();
    if (packet != "FAIL")
    {
        sscanf(packet.c_str(), "%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n", \
        &(in_data.mx), &(in_data.my), &(in_data.mz), \
        &(in_data.gx), &(in_data.gy), &(in_data.gz), \
        &(in_data.ax), &(in_data.ay), &(in_data.az), \
        &(in_data.altitude));
        return 0;
    }
    else
    {
        return -1;
    }
}

int UDPListener::receiveControl(control &ctrl) const
{
    string packet = listen();
    if (packet != "FAIL")
    {
        sscanf(packet.c_str(), "%f\t%f\t%f\t%f\n", &(ctrl.ail), \
            &(ctrl.elev), &(ctrl.rudder), &(ctrl.throttle));
        return 0;
    }
    else
    {
        return -1;
    }
}

int UDPListener::receivePlaneState(PlaneState &p) const
{
    string packet = listen();
    if (packet != "FAIL")
    {
        float x, y, z, w;
        sscanf(packet.c_str(), "%f\t%f\t%f\t%f\n", \
               &x, &y, &z, &w);
        p.orientation = Quaternion(x, y, z, w);
        return 0;
    }
    else
    {
        return -1;
    }
}

UDPSender::UDPSender(string url, string port, int debug) : mDebug(debug)
{
    out_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (out_sockfd < 0) 
    {
        if (mDebug)
            cerr << "ERROR opening socket" << endl;
        return;
    }
    
    int flags = fcntl(out_sockfd, F_GETFL); 
    flags |= O_NONBLOCK;
    fcntl(out_sockfd, F_GETFL);

    int outport = atoi(port.c_str());
    
    bzero((char *) &out_addr, sizeof(out_addr));
    out_addr.sin_family = AF_INET;
    out_addr.sin_port = htons(outport);
    
    
    struct hostent *hp = gethostbyname(url.c_str());
    if (!hp) {
        if (mDebug)
            cerr << "could not obtain address of " << url << endl;
        return;
    }
    
    /* put the host's address into the server address structure */
    memcpy((void *)&out_addr.sin_addr, hp->h_addr_list[0], hp->h_length);
    

}

int UDPSender::send(const string msg) const
{
    if (sendto(out_sockfd, msg.c_str(), msg.size(), 0, 
            (struct sockaddr *)&out_addr, sizeof(out_addr)) < 0) {
        perror("sendto failed");
        return -1;
    }
    return 1;
}

int UDPSender::sendSensor(const sensor &data) const
{
    char str[256];
    float roll = (float) data.mx;
    float pitch = (float) data.my;
    float heading = (float) data.mz;
    float r_rate = (float) data.gx;
    float p_rate = (float) data.gy;
    float y_rate = (float) data.gz;
    float ax = (float) data.ax;
    float ay = (float) data.ay;
    float az = (float) data.az;
    float altitude = (float) data.altitude;
    snprintf(str, 128, "%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n",\
    roll, pitch, heading, \
    r_rate, p_rate, y_rate, \
    ax, ay, az, \
    altitude);
    string packet((const char *) str);
    return send(packet);
}

int UDPSender::sendControl(const control &ctrl) const
{
    char send_data[256];
    sprintf(send_data, "%f\t%f\t%f\t%f\n", ctrl.ail, ctrl.elev, ctrl.rudder,
    ctrl.throttle);
    string packet(send_data);
    return send(packet);
}

int sendPlaneState(const PlaneState &p) const
{
    char buf[256];
    sprintf(buf, "%f\t%f\t%f\t%f\n", \
            p.orientation.x, p.orientation.y \
            p.orientation.z, p.orientation.w);
    string packet(buf);
    return send(packet);
}
