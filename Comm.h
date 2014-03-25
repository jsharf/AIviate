#ifndef COMM_H
#define COMM_H

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
using namespace std;

extern const int PACKETSIZE;

class UDPListener
{
    public:
        UDPListener(string port, int debug=false);
        string listen() const;
    private:
        struct sockaddr_in remaddr;
        int in_sockfd;
        bool mDebug;
};

class UDPSender
{
    public:
        UDPSender(string url, string port, int debug=false);
        int send(string msg) const;
    private:
        sockaddr_in out_addr;
        int out_sockfd;
        bool mDebug;
};

#endif
