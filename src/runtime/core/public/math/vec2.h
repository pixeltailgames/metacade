/*
Copyright 2017 Zachary Blystone <zakblystone@gmail.com>

This file is part of Metacade.

Metacade is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Metacade is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Metacade.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
===============================================================================
vec2.h:
===============================================================================
*/

#include "metacade_private.h"

#pragma once

class CMatrix3;
class CVec2 
{
public:
	union
	{
		struct
		{
			float x,y;
		};
		float _xy[2];
	};

	//Constructor
	CVec2();
	CVec2(float x, float y);
	CVec2(const CVec2 &other);

	//Accessors and Mutators
	void set(float x, float y);
	void set(float xy[2]);
	void set(const CVec2 &other);
	float getX() const;
	float getY() const;
	void get(float *ptr) const;

	//Utility Functions
	float lengthSquared() const;
	float length() const;
	float distanceSquared(const CVec2 &other) const;
	float distance(const CVec2 &other) const;
	float dot(const CVec2 &other) const;
	float cross(const CVec2 &other) const;
	float getAngle() const;
	float angleTo(const CVec2 &other) const;
	bool inRect(const CVec2 &pos, const CVec2 &size) const;
	bool inBox(const CVec2 &min, const CVec2 &max) const;
	CVec2 vmin(const CVec2 &b) const;
	CVec2 vmax(const CVec2 &b) const;
	CVec2 interpolateTo(const CVec2 &other, float fraction) const;
	CVec2 &normalize();

	//Operator Overloads
	friend ostream& operator << (ostream &os, const CVec2 &v)
	{
		os << "[";
		os << v._xy[0] << ", ";
		os << v._xy[1] << "]";
		return os;
	}

	friend istream& operator >> (istream &is, CVec2 &v)
	{
		is >> v._xy[0];
		is >> v._xy[1];
		return is;
	}

	float operator[](int i) const;
	float& operator[](int i);

	CVec2 operator--();
	CVec2& operator+=(const CVec2 &other);
	CVec2& operator-=(const CVec2 &other);
	CVec2& operator*=(const CMatrix3 &other);
	CVec2& operator*=(const CVec2 &other);
	CVec2& operator*=(float scalar);
	CVec2& operator/=(const CVec2 &other);
	CVec2& operator/=(float scalar);
	CVec2 operator+(const CVec2 &other) const;
	CVec2 operator-(const CVec2 &other) const;
	CVec2 operator*(const CMatrix3 &other) const;
	CVec2 operator*(const CVec2 &other) const;
	CVec2 operator*(float scalar) const;
	CVec2 operator/(const CVec2 &other) const;
	CVec2 operator/(float scalar) const;
	bool operator==(const CVec2 &other) const;
	bool operator!=(const CVec2 &other) const;
};

CVec2 operator/(float scalar, CVec2 vec);

static const CVec2 Vec2Zero(0,0);