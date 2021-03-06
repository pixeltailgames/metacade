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
drawbuffer.h:
===============================================================================
*/

#pragma once

#include "render/private/renderbuffer.h"

namespace Arcade
{

class CDrawBuffer : public IDrawBuffer
{
public:
	CDrawBuffer(shared_ptr<CRenderBuffer> renderBuffer);

	void addRenderBatch(CRenderBatch batch);
	void clearRenderBatches();

	virtual const CRenderBatch* getRenderBatches() const override;
	virtual uint32 getNumRenderBatches() const override;
	virtual const IRenderBuffer* getRenderBuffer() const override;

private:

	shared_ptr<CRenderBuffer> _renderBuffer;
	vector<CRenderBatch, CRuntimeAllocator<CRenderBatch>> _renderBatches;
};

}