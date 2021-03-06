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
core_public.h: Inter-modular headers
===============================================================================
*/

#define EPSILON 0.00001f

#ifndef M_PI
#define M_PI       3.14159265358979323846f
#endif

#define M_FPI	   3.1415926f

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#include "public/math/matrix3.h"
#include "public/math/matrix4.h"
#include "public/math/vec2.h"
#include "public/math/vec3.h"
#include "public/math/halfplane.h"
#include "public/math/random.h"

#include "public/gfx/color.h"
#include "public/gfx/image.h"
#include "public/gfx/vertex.h"

#include "public/util/refcounter.h"
#include "public/util/variant.h"
#include "public/util/guid.h"
#include "public/util/string.h"
#include "public/util/sha.h"

#include "public/iallocator.h"