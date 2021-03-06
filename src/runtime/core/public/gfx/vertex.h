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
vertex.h: Vertex2D and Vertex3D
===============================================================================
*/

#pragma once

#include "metacade_types.h"

namespace Arcade
{

struct CVertex2D;
struct METACADE_API CVertex3D
{
	CVertex3D();
	CVertex3D(const CVec3 &pos, const CVec3 &norm, const CVec2 &tc, const CColor &col = CColor(0xFFFFFFFF));
	CVertex3D(float x, float y, float z,
			float nx, float ny, float nz,
			float s, float t,
			const CColor &col = CColor(0xFFFFFFFF) );

	CVertex3D interpolateTo(const CVertex3D &other, float fraction) const;

	CVec3 _position;
	CVec3 _normal;
	CVec2 _texcoord;
	CColor _color;
};

struct METACADE_API CVertex2D
{
	inline CVertex2D();
	inline CVertex2D(const CVec2 &pos, const CVec2 &tc, const CColor &col = CColor(0x000000FF));
	inline CVertex2D(float x, float y,
			float s, float t,
			const CColor &col = CColor(0xFFFFFFFF));

	inline CVertex2D(const CVertex3D &other);

	inline CVertex2D interpolateTo(const CVertex2D &other, float fraction) const;
	static void interpolateTo(const CVertex2D& A, const CVertex2D& B, CVertex2D& result, float fraction);

	inline CVertex2D operator * (const CMatrix3& matrix) const
	{
		return CVertex2D(_position * matrix, _texcoord, _color);
	}

	CVertex3D to3D() const;

	CVec2 _position;
	CVec2 _texcoord;
	CColor _color;
};

}