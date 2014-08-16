#ifndef COMM_H
#define COMM_H

#include <ostream>
#include <iostream>
#include <unistd.h>
#include <string>
#include <sys/types.h> 
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string>
#include <cstring>
#include "Sensor.h"
#include "Control.h"
#include "../Vector/Vector.h"
using namespace std;
extern const int PACKETSIZE;

struct control;

class UDPListener
{
    public:
        UDPListener(string port, int debug=false);
        string listen() const;
        int receiveSensor(sensorf &in_data) const;
        int receiveControl(control &ctrl) const;
        int receivePlaneState(PlaneState &p) const;
    private:
        struct sockaddr_in remaddr;
        int in_sockfd;
        bool mDebug;
};

class UDPSender
{
    public:
        UDPSender(string url, string port, int debug=false);
        int send(const string msg) const;
        int sendSensor(const sensor &data) const;
        int sendControl(const control &ctrl) const;
        int sendPlaneState(const PlaneState &p) const;
        int sendTwoVectors(const Vector3d &a, const Vector3d &b) const;
    private:
        sockaddr_in out_addr;
        int out_sockfd;
        bool mDebug;
};

#endif
