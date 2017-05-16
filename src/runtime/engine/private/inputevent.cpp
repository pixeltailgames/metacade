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
inputevent.cpp:
===============================================================================
*/

#include "engine_private.h"

CInputEvent::CInputEvent()
{

}

CInputEvent CInputEvent::generateKeyEvent(uint8 keycode, bool wasPressed)
{
	return CInputEvent();
}

CInputEvent CInputEvent::generateMouseButtonEvent(uint8 button, bool wasPressed)
{
	return CInputEvent();
}

CInputEvent CInputEvent::generateMouseMovementEvent(float oldX, float oldY, float x, float y)
{
	return CInputEvent();
}

CInputEvent CInputEvent::generateFocusEvent(EFocusElement element, bool hasFocus)
{
	return CInputEvent();
}
