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
render_public.h:
===============================================================================
*/

#define MAX_TEXTURE_BITS 10
#define MAX_TEXTURES 1024
#define MAX_VERTICES 65535
#define MAX_INDICES 200000

#include "public/itexture.h"
#include "public/irenderbuffer.h"
#include "public/idrawbuffer.h"
#include "public/irenderer.h"
#include "public/iimage.h"
#include "public/material.h"
#include "public/renderstate.h"
#include "public/renderelement.h"
#include "public/renderbatch.h"