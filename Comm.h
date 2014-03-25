#include <string>
using namespace std;

const int PACKETSIZE = 2048;

class UDPListener
{
    public:
        UDPListener(string port, int debug);
        string listen() const;
    private:
        int in_sockfd;
        bool mDebug;
};

class UDPSender
{
    public:
        UDPSender(string port, string url, int debug);
        int send(string msg) const;
    private:
        int out_sockfd;
        bool mDebug;
};

