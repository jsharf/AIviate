#include <Comm.h>
using namespace std;

UDPListener::UDPListener(string port, int debug=false) : mDebug(debug)
{
    sockaddr_in in_addr;
    struct sockaddr_in remaddr; /* remote address */
    socklen_t addrlen = sizeof(remaddr); /* length of addresses */
    in_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (in_sockfd < 0) 
    {
        cerr << "ERROR opening socket" << endl;
        return;
    }
    // set nonblocking mode
    int flags = fcntl(in_sockfd, F_GETFL);
    flags |= O_NONBLOCK;
    fcntl(in_sockfd, F_SETFL, flags);

    bzero((char *) &in_addr, sizeof(in_addr));
    int portno = atoi(port);
    in_addr.sin_family = AF_INET;
    in_addr.sin_addr.s_addr = INADDR_ANY;
    in_addr.sin_port = htons(portno);

    if (bind(in_sockfd, (struct sockaddr *) &in_addr,
                sizeof(in_addr)) < 0) 
    {
        error("ERROR on binding"); 
        return;
    }
    IFDEBUG cerr << "Listening on port " << portno << endl;
}

string UDPListener::listen() const
{
    int recvlen = recvfrom(in_sockfd, buf, PACKETSIZE, 0, 
            (struct sockaddr*)&remaddr, &addrlen);
    if (recvlen > 0) 
    {
        unsigned char buf[recvlen];
        buf[recvlen] = 0;
        string packet((char *)buf);
        cerr << "Received Message:" << packet << endl;
        return packet;
    }
    else
    {
        return "FAIL";
    }
}

UDPSender::UDPSender(string port, string url, int debug = false) : mDebug(debug)
{
    sockaddr_in out_addr;
    out_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (out_sockfd < 0) 
    {
        cerr << "ERROR opening socket" << endl;
        return;
    }

    int flags = fcntl(out_sockfd, F_GETFL); 
    flags |= O_NONBLOCK;
    fcntl(out_sockfd, F_GETFL); 

    struct hostent *hp;   
    hp = gethostbyname(url.c_str());
    if (!hp) {
        cerr << "could not obtain address of " << url << endl;
        return;
    }

    bzero((char *) &out_addr, sizeof(out_addr));
    int outport = atoi(port.c_str());
    out_addr.sin_family = AF_INET;
    out_addr.sin_port = htons(outport);
    /* put the host's address into the server address structure */
    memcpy((void *)&out_addr.sin_addr, hp->h_addr_list[0], hp->h_length);

    if (!hp) {
        cerr << "could not obtain address of " << outport << endl;
        return;
    }

    /* put the host's address into the server address structure */
    memcpy((void *)&out_addr.sin_addr, hp->h_addr_list[0], hp->h_length);
    if (bind(out_sockfd, (struct sockaddr *) &out_addr,
                sizeof(out_addr)) < 0) 
    {
        error("ERROR on binding"); 
        return 1;
    }
}

int UDPSender::send(string msg) const
{
    if (sendto(out_sockfd, msg.c_str(), msg.size(), 0, 
            (struct sockaddr *)&out_addr, sizeof(out_addr)) < 0) {
        perror("sendto failed");
        return 1;
    }
}
