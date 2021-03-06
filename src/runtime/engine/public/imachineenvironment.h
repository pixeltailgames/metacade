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
imachineenvironment.h: Interface for the environment a single machine runs in
===============================================================================
*/

#pragma once

#include "render/render_public.h"
#include "sound/sound_public.h"

namespace Arcade
{

class IMachineEnvironment
{
public:
	virtual class IRenderer* getRenderer() = 0;
	virtual class IAudioSystem* getAudioSystem() = 0;
	virtual ~IMachineEnvironment() {};
};

}