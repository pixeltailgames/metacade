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
renderlist.cpp:
===============================================================================
*/

#include "render_private.h"

CRenderList::CRenderList()
{
	_elements.reserve(8192);
	_elements.resize(8192);
	_count = 0;
}

CRenderElement& CRenderList::emplaceRenderElement()
{
	if ( _count == _elements.size() )
		_elements.resize(_count * 2);

	return _elements[_count++];
	//_elements.emplace_back();
	//return _elements.back();
}

void CRenderList::clear()
{
	//_elements.clear();
	//_elements.resize(8192);
	_count = 0;
}

CRenderElement& CRenderList::top()
{
	return _elements[_count-1];
	//return _elements.back();
}

void CRenderList::pop()
{
	_count--;
	//_elements.pop_back();
}

bool CRenderList::empty()
{
	return _count == 0;
	//return _elements.empty();
}

void CRenderList::sort()
{
	std::reverse(_elements.begin(), _elements.begin() + _count);
	std::sort(_elements.begin(), _elements.begin() + _count, [](const CRenderElement &a, const CRenderElement &b)
	{
		int32 layerA = a.getLayer();
		int32 layerB = b.getLayer();

		if ( layerA != layerB ) return layerA > layerB;

		uint64 hashA = a.getRenderState().getHash();
		uint64 hashB = b.getRenderState().getHash();
		return hashA > hashB;
	});
}
