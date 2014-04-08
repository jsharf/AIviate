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

Quaternion::Quaternion(fp_type _x, fp_type _y, fp_type _z, fp_type _w)
{
    this->x = _x;
    this->y = _y;
    this->z = _z;
    this->w = _w;
}

Quaternion::Quaternion()
{
    Quaternion(0,0,0,0);
}

Quaternion Quaternion::operator+(const Quaternion& other) const
{
    Quaternion ret(other.x + this->x, other.y + this->y, other.z + this->z, other.w + this->w);
    return ret;
}

void Quaternion::operator+=(const Quaternion& other)
{
    this->x += other.x;
    this->y += other.y;
    this->z += other.z;
    this->w += other.w;
}

Quaternion Quaternion::operator-(const Quaternion& other) const
{
    Quaternion ret(other.x - this->x, other.y - this->y, other.z - this->z, other.w - this->w);
    return ret;
}

void Quaternion::operator-=(const Quaternion& other)
{
    this->x -= other.x;
    this->y -= other.y;
    this->z -= other.z;
    this->w -= other.w;
}

Quaternion Quaternion::operator*(const Quaternion& other) const
{
    Quaternion ret(
        this->w*other.x + this->x*other.w + this->y*other.z - this->z*other.y,
        this->w*other.y - this->x*other.z + this->y*other.w + this->z*other.x,
        this->w*other.z + this->x*other.y - this->y*other.x + this->z*other.w,
        this->w*other.w - this->x*other.x - this->y*other.y - this->z*other.z);
    return ret;
}

void Quaternion::operator*=(const Quaternion& other)
{
    this->x = this->w*other.x + this->x*other.w + this->y*other.z - this->z*other.y;
    this->y = this->w*other.y - this->x*other.z + this->y*other.w + this->z*other.x;
    this->z = this->w*other.z + this->x*other.y - this->y*other.x + this->z*other.w;
    this->w = this->w*other.w - this->x*other.x - this->y*other.y - this->z*other.z;
}

Quaternion Quaternion::operator*(fp_type scalar) const
{
    Quaternion ret(
        this->x * scalar,
        this->y * scalar,
        this->z * scalar,
        this->w * scalar
    );
    return ret;
}

void Quaternion::operator*=(fp_type scalar)
{
    this->x *= scalar;
    this->y *= scalar;
    this->z *= scalar;
    this->w *= scalar;
}

Quaternion Quaternion::operator/(fp_type scalar) const
{
    Quaternion ret(
        this->x / scalar,
        this->y / scalar,
        this->z / scalar,
        this->w / scalar
    );
    return ret;
}

void Quaternion::operator/=(fp_type scalar)
{
    this->x /= scalar;
    this->y /= scalar;
    this->z /= scalar;
    this->w /= scalar;
}

bool Quaternion::operator==(const Quaternion& other) const
{
#if(COMPARE_ACCURACY > 0)
    return ((this->x==other.x)&&(this->y==other.y)&&(this->z==other.z)&&(this->w==other.w));
#else
    return ((abs(this->x-other.x)<=COMPARE_ACCURACY)&&(abs(this->y-other.y)<=COMPARE_ACCURACY)&&(abs(this->z-other.z)<=COMPARE_ACCURACY)&&(abs(this->w-other.w)<=COMPARE_ACCURACY));
#endif
}

Quaternion Quaternion::conjugate() const
{
    return Quaternion(-x, -y, -z, w);
}

fp_type Quaternion::norm()
{
    return sqrt(x*x + y*y + z*z + w*w);
}

Quaternion Quaternion::unit()
{
    return (*this)/norm();
}

fp_type Quaternion::distance(const Quaternion& other) const
{
    return ((*this) - other).norm();
}

fp_type Quaternion::dot(const Quaternion& other) const
{
    return (x*other.x + y*other.y + z*other.z + w*other.w);
}

Quaternion Quaternion::slerp(const Quaternion& endpt, fp_type t) const
{
    Quaternion qret;
    fp_type cosHalfTheta = dot(endpt);

    if(abs(cosHalfTheta) >= 1.0f)
    {
        qret.x = x;
        qret.y = y;
        qret.z = z;
        qret.w = w;
        return qret;
    }

    fp_type halfTheta = acos(cosHalfTheta);
    fp_type sinHalfTheta = sqrt(1.0f - cosHalfTheta*cosHalfTheta);

    if(abs(sinHalfTheta) < 0.001f)
    {
        qret.x = (x*0.5f + endpt.x*0.5f);
        qret.y = (y*0.5f + endpt.y*0.5f);
        qret.z = (z*0.5f + endpt.z*0.5f);
        qret.w = (w*0.5f + endpt.w*0.5f);
        return qret;
    }

    fp_type ratA = sin((1 - t) * halfTheta) / sinHalfTheta;
    fp_type ratB = sin(t * halfTheta) / sinHalfTheta;

    qret.x = (x*ratA + endpt.x*ratB);
    qret.y = (y*ratA + endpt.y*ratB);
    qret.z = (z*ratA + endpt.z*ratB);
    qret.w = (w*ratA + endpt.w*ratB);
    return qret;
}

const Quaternion Quaternion::zero = Quaternion(0,0,0,0);
const Quaternion Quaternion::identity = Quaternion(0,0,0,1);
const Quaternion Quaternion::i = Quaternion(1,0,0,0);
const Quaternion Quaternion::j = Quaternion(0,1,0,0);
const Quaternion Quaternion::k = Quaternion(0,0,1,0);
