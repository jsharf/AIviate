#include "Control.h"
using namespace std;

ostream& operator<<(ostream &out, control &rhs)
{
    out << "CTRL: \n{\n\t ail: " << rhs.ail << "\n\t elev: " << rhs.elev \
    << "\n\t rudder: " << rhs.rudder << "\n\t throttle: " << rhs.throttle \
    << "\n}" << endl;
    return out;
}

const control control::zero = {0,0,0,0};
