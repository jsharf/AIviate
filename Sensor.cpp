#include "Sensor.h"

// Helps debugging with sensor structs
std::ostream& operator<<(std::ostream &out, sensorf &rhs)
{
    out << "SENSORF: \n{\n\t ax: " \
    << rhs.ax << "\n\t ay: " << rhs.ay << "\n\t az: " << rhs.az \
    << "\n\t gx: " << rhs.gx << "\n\t gy: " << rhs.gy << "\n\t gz: " << rhs.gz \
    << "\n\t mx: " << rhs.mx << "\n\t my: " << rhs.my << "\n\t mz: " << rhs.mz \
    << "\n\t altitude: " << rhs.altitude \
    << "\n}" << std::endl;
    return out;
}

