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

    if (bind(in_sockfd, (struct sockaddr *) &in_addr,
                sizeof(in_addr)) < 0) 
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

int UDPSender::send(string msg) const
{
    if (sendto(out_sockfd, msg.c_str(), msg.size(), 0, 
            (struct sockaddr *)&out_addr, sizeof(out_addr)) < 0) {
        perror("sendto failed");
        return -1;
    }
    return 1;
}
