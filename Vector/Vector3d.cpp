//  This file is part of the Vector library.
//
//  The Vector library is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  The Vector library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with the Chronos library.  If not, see <http://www.gnu.org/licenses/>.
//
//  Copyright 2013 Kevin Balke (fughilli@gmail.com)

#include "Vector.h"

#ifdef ARDUINO
void printVector3d(Vector3d arg)
{
    Serial.print("<");
    Serial.print(arg.x);
    Serial.print(", ");
    Serial.print(arg.y);
    Serial.print(", ");
    Serial.print(arg.z);
    Serial.print(">");
}
#endif

//! Construct new Vector3d
Vector3d::Vector3d(fp_type _x, fp_type _y, fp_type _z)
{
    this->x = _x;
    this->y = _y;
    this->z = _z;
}

//! Construct new Vector3d (with zeros)
Vector3d::Vector3d()
{
    this->x = 0.0f;
    this->y = 0.0f;
    this->z = 0.0f;
}


//! Adds two Vector3d's together (component-wise)
Vector3d Vector3d::operator+(const Vector3d& other) const
{
    Vector3d temp(this->x + other.x, this->y + other.y, this->z + other.z);
    return temp;
}

//! Adds two Vector3d's together (component-wise)
void Vector3d::operator+=(const Vector3d& other)
{
    this->x += other.x;
    this->y += other.y;
    this->z += other.z;
}

//! Subtracts a Vector3d from another Vector3d (component-wise)
Vector3d Vector3d::operator-(const Vector3d& other) const
{
    Vector3d temp(this->x - other.x, this->y - other.y, this->z - other.z);
    return temp;
}

//! Subtracts a Vector3d from another Vector3d (component-wise)
void Vector3d::operator-=(const Vector3d& other)
{
    this->x -= other.x;
    this->y -= other.y;
    this->z -= other.z;
}

//! Scalar multiply
Vector3d Vector3d::operator*(fp_type scalar) const
{
    Vector3d temp(this->x * scalar, this->y * scalar, this->z * scalar);
    return temp;
}

//! Scalar multiply
void Vector3d::operator*=(fp_type scalar)
{
    this->x *= scalar;
    this->y *= scalar;
    this->z *= scalar;
}

//! Scalar divide
Vector3d Vector3d::operator/(fp_type scalar) const
{
    Vector3d temp(this->x / scalar, this->y / scalar, this->z / scalar);
    return temp;
}

//! Scalar divide
void Vector3d::operator/=(fp_type scalar)
{
    this->x /= scalar;
    this->y /= scalar;
    this->z /= scalar;
}

//! Boolean compare
bool Vector3d::operator==(const Vector3d& other) const
{
#if(COMPARE_ACCURACY > 0)
    return ((this->x==other.x)&&(this->y==other.y)&&(this->z==other.z));
#else
    return ((abs(this->x-other.x)<=COMPARE_ACCURACY)&&(abs(this->y-other.y)<=COMPARE_ACCURACY)&&(abs(this->z-other.z)<=COMPARE_ACCURACY));
#endif
}

//! Compute the dot product of two Vector3d's
fp_type Vector3d::dot(const Vector3d& other) const
{
    return ((this->x * other.x) + (this->y * other.y) + (this->z * other.z));
}

//! Compute the cross product of two Vector3d's	(orthogonal Vector3d)
Vector3d Vector3d::cross(const Vector3d& other) const
{
    Vector3d temp(
        this->y * other.z - this->z * other.y,
        this->z * other.x - this->x * other.z,
        this->x * other.y - this->y * other.x
    );
    return temp;
}

//! Compute magnitude of the Vector3d
fp_type Vector3d::magnitude() const
{
    return sqrt((this->x * this->x) + (this->y * this->y) + (this->z * this->z));
}

//! Compute the normalized form of the Vector3d
Vector3d Vector3d::unit() const
{
    fp_type mag = this->magnitude();
    Vector3d temp(this->x / mag, this->y / mag, this->z / mag);
    return temp;
}

//! Compute the angle the (x,y) components of the Vector3d point along (spherical theta)
fp_type Vector3d::theta() const
{
    return atan2(this->y, this->x);
}

//! Compute the angle of elevation of the Vector3d out of the (x,y) plane (spherical rho)
fp_type Vector3d::rho() const
{
    return atan2(this->z, sqrt((this->x * this->x) + (this->y * this->y)));
}

//! Compute the angle between two Vector3d's
fp_type Vector3d::angleTo(const Vector3d& other) const
{
//    fp_type dotprod = dot(other);
//    dotprod /= (magnitude() * other.magnitude());
//    return acos(dotprod);
    return LIMIT_RAD_RANGE(atan2(cross(other).magnitude(), dot(other)));
}

Quaternion Vector3d::quaternionTo(const Vector3d& other) const
{
    Vector3d axis = cross(other);
    return axis.rotationAroundAxis(abs(angleTo(other)));
}

Quaternion Vector3d::rotationAroundAxis(fp_type theta) const
{
    Vector3d axis = unit() * sin(theta / 2.0f);
    return Quaternion(axis.x, axis.y, axis.z, cos(theta / 2.0f));
}

//! Rotate the vector around the axis axis by the angle theta (+theta is CCW if vector points towards observer)
Vector3d Vector3d::rotate(const Vector3d& axis, fp_type theta) const
{
//    fp_type cosT = cos(theta);
//    fp_type sinT = sin(theta);
//    Vector3d uaxis = axis.unit();
//    fp_type ux = uaxis.x;
//    fp_type uy = uaxis.y;
//    fp_type uz = uaxis.z;
//    //R = 	[(cosT + ((ux * ux) * (1.0f - cosT))) ; ((ux * uy * (1.0f - cosT)) - (uz * sinT)) ; ((ux * uz * (1.0f - cosT)) + (uz * sinT))]
//    //		[((uy * ux * (1.0f - cosT)) + (uz * sinT)) ; (cosT + ((uy * uy) * (1.0f - cosT))) ; ((uy * uz * (1.0f - cosT)) - (ux * sinT))]
//    //		[((uz * ux * (1.0f - cosT)) - (uy * sinT)) ; ((uz * uy * (1.0f - cosT)) + (ux * sinT)) ; (cosT + ((uz * uz) * (1.0f - cosT)))]
//    fp_type tx = (cosT + ((ux * ux) * (1.0f - cosT))) * this->x + ((ux * uy * (1.0f - cosT)) - (uz * sinT)) * this->y + ((ux * uz * (1.0f - cosT)) + (uz * sinT)) * this->z;
//    fp_type ty = ((uy * ux * (1.0f - cosT)) + (uz * sinT)) * this->x + (cosT + ((uy * uy) * (1.0f - cosT))) * this->y + ((uy * uz * (1.0f - cosT)) - (ux * sinT)) * this->z;
//    fp_type tz = ((uz * ux * (1.0f - cosT)) - (uy * sinT)) * this->x + ((uz * uy * (1.0f - cosT)) + (ux * sinT)) * this->y + (cosT + ((uz * uz) * (1.0f - cosT))) * this->z;
//    Vector3d temp(tx, ty, tz);
//    return temp;
    Vector3d uaxis = axis.unit();
    return (((*this)*cos(theta)) + (cross(uaxis)*sin(theta)) + (uaxis*dot(uaxis)*(1-cos(theta))));
}

Vector3d Vector3d::rotate(const Quaternion& rot_q) const
{
    Quaternion vec_q(this->x, this->y, this->z, 0);
    vec_q = (rot_q*vec_q)*rot_q.conjugate();
    Vector3d ret(vec_q.x, vec_q.y, vec_q.z);
    return ret;
}

Vector3d Vector3d::project(const Vector3d& axis) const
{
    Vector3d uaxis = axis.unit();
    return uaxis*dot(uaxis);
}

Vector3d Vector3d::lerp(const Vector3d& endpt, fp_type t) const
{
    return (((*this)*(1 - t)) + (endpt*t));
}

const Vector3d Vector3d::i = Vector3d(1.0f,0.0f,0.0f);
const Vector3d Vector3d::j = Vector3d(0.0f,1.0f,0.0f);
const Vector3d Vector3d::k = Vector3d(0.0f,0.0f,1.0f);
const Vector3d Vector3d::one = Vector3d(1.0f,1.0f,1.0f);
const Vector3d Vector3d::zero = Vector3d(0.0f,0.0f,0.0f);
