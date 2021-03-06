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
iimage.h: Abstraction of an image resource
===============================================================================
*/

#pragma once

#include "metacade_types.h"

namespace Arcade
{

enum EImageFlags
{
	IF_WRAP_X = 1 << 0,
	IF_WRAP_Y = 1 << 1,
	IF_SMOOTH = 1 << 2,
};

class IImage
{
public:
	virtual int32 getWidth() const = 0;
	virtual int32 getHeight() const = 0;
	virtual int32 getBytesPerPixel() const = 0;
	virtual EImagePixelFormat getPixelFormat() const = 0;
	virtual uint8* getPixels() const = 0;
	virtual uint32 getID() const = 0;
	virtual uint32 getFlags() const = 0;
	virtual ~IImage() = 0;
};

}