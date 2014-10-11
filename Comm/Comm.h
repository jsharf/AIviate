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

#include "Control.h"
#include "../Sensor/Sensor.h"
#include "../types.h"
#include "../Vector/Vector.h"

using namespace std;
extern const int PACKETSIZE;

struct control;

class UDPListener
{
    public:
        UDPListener(string port, int debug=false);
        result_t listen(string &ret) const;
        result_t receiveSensor(sensorf &in_data) const;
        result_t receiveControl(control &ctrl) const;
        result_t receivePlaneState(PlaneState &p) const;
    private:
        struct sockaddr_in remaddr;
        int in_sockfd;
        bool mDebug;
};

class UDPSender
{
    public:
        UDPSender(string url, string port, int debug=false);
        result_t send(const string msg) const;
        result_t sendSensor(const sensor &data) const;
        result_t sendControl(const control &ctrl) const;
        result_t sendPlaneState(const PlaneState &p) const;
        result_t sendTwoVectors(const Vector3d &a, const Vector3d &b) const;
    private:
        sockaddr_in out_addr;
        int out_sockfd;
        bool mDebug;
};

#endif
