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
js_private.h:
===============================================================================
*/

#include "js_public.h"

#include "libplatform/libplatform.h"
#include "v8.h"

#include "engine/engine_private.h"

#include "private/js_userdata.h"
#include "private/js_asset.h"
#include "private/js_draw.h"
#include "private/js_sound.h"
#include "private/js_class.h"
#include "private/js_instance.h"
#include "private/js_vm.h"

using namespace Arcade;