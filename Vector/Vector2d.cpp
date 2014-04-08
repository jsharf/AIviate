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
void printVector3d(Vector2d arg)
{
    Serial.print("<");
    Serial.print(arg.x);
    Serial.print(", ");
    Serial.print(arg.y);
    Serial.print(">");
}
#endif

//! Construct new Vector2d
Vector2d::Vector2d(fp_type _x, fp_type _y)
{
    this->x = _x;
    this->y = _y;
}

//! Construct new Vector2d (with zeros)
Vector2d::Vector2d()
{
    this->x = 0.0f;
    this->y = 0.0f;
}

//! Adds two Vector2d's together (component-wise)
Vector2d Vector2d::operator+(Vector2d const &other) const
{
    Vector2d temp(this->x + other.x, this->y + other.y);
    return temp;
}

//! Adds two Vector2d's together (component-wise)
void Vector2d::operator+=(Vector2d const &other)
{
    this->x += other.x;
    this->y += other.y;
}

//! Subtracts a Vector2d from another Vector2d (component-wise)
Vector2d Vector2d::operator-(Vector2d const &other) const
{
    Vector2d temp(this->x - other.x, this->y - other.y);
    return temp;
}

//! Subtracts a Vector2d from another Vector2d (component-wise)
void Vector2d::operator-=(Vector2d const &other)
{
    this->x -= other.x;
    this->y -= other.y;
}

//! Scalar multiply
Vector2d Vector2d::operator*(fp_type scalar) const
{
    Vector2d temp(this->x * scalar, this->y * scalar);
    return temp;
}

//! Scalar multiply
void Vector2d::operator*=(fp_type scalar)
{
    this->x *= scalar;
    this->y *= scalar;
}

//! Scalar divide
Vector2d Vector2d::operator/(fp_type scalar) const
{
    Vector2d temp(this->x / scalar, this->y / scalar);
    return temp;
}

//! Scalar divide
void Vector2d::operator/=(fp_type scalar)
{
    this->x /= scalar;
    this->y /= scalar;
}

//! Boolean compare
bool Vector2d::operator==(const Vector2d& other) const
{
#if(COMPARE_ACCURACY > 0)
    return ((this->x==other.x)&&(this->y==other.y));
#else
    return ((abs(this->x-other.x)<=COMPARE_ACCURACY)&&(abs(this->y-other.y)<=COMPARE_ACCURACY));
#endif
}

//! Compute the dot product of two Vector2d's
fp_type Vector2d::dot(Vector2d const &other) const
{
    return ((this->x * other.x) + (this->y * other.y));
}

//! Compute the magnitude of the Vector2d
fp_type Vector2d::magnitude() const
{
    return sqrt((this->x * this->x) + (this->y * this->y));
}

//! Compute the normalized form of the Vector2d
Vector2d Vector2d::unit() const
{
    fp_type mag = this->magnitude();
    Vector2d temp(this->x / mag, this->y / mag);
    return temp;
}

//! Compute the angle the Vector2d points along (polar theta)
fp_type Vector2d::theta() const
{
    return atan2(y, x);
}

//! Compute the angle between two Vector2d's
fp_type Vector2d::angleTo(Vector2d const &other) const
{
    return LIMIT_RAD_RANGE(other.theta() - theta());
}

//! Rotates the vector around the origin by an angle theta
Vector2d Vector2d::rotate(fp_type theta) const
{
    Vector2d temp((this->x * cos(theta)) - (this->y * sin(theta)), (this->x * sin(theta)) + (this->y * cos(theta)));
    return temp;
}

Vector2d Vector2d::project(const Vector2d& axis) const
{
    Vector2d uaxis = axis.unit();
    return uaxis*dot(uaxis);
}

Vector2d Vector2d::lerp(const Vector2d& endpt, fp_type t) const
{
    return (((*this)*(1 - t)) + (endpt*t));
}

std::ostream& operator<<(std::ostream& os, Vector2d& vec)
{
    os << "<" << vec.x << ", " << vec.y << ">";
    return os;
}

const Vector2d Vector2d::i = Vector2d(1,0);
const Vector2d Vector2d::j = Vector2d(0,1);
const Vector2d Vector2d::one = Vector2d(1,1);
const Vector2d Vector2d::zero = Vector2d(0,0);
