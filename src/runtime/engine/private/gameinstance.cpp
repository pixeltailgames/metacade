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
gameinstance.cpp:
===============================================================================
*/

#include "engine_private.h"
#include "render/render_private.h"

class CWhiteImage : public IImage
{
	virtual int32 getWidth() const override { return 2; }
	virtual int32 getHeight() const override { return 2; }
	virtual int32 getBytesPerPixel() const override { return 4; }
	virtual EImagePixelFormat getPixelFormat() const override
	{
		return EImagePixelFormat::PFM_RGBA8;
	}

	virtual uint8* getPixels() const override
	{
		static const CColor pixels[4] = {
			CColor(0xFFFFFFFF),
			CColor(0xFFFFFFFF),
			CColor(0xFFFFFFFF),
			CColor(0xFFFFFFFF),
		};
		return (uint8 *) pixels;
	}

	virtual uint32 getID() const override
	{
		return 0;
	}
};

CGameInstance::CGameInstance(weak_ptr<CGameClass> klass, shared_ptr<IVMInstance> vmInstance)
	: CRuntimeObject(klass.lock().get())
	, _klass(klass)
	, _vmInstance(vmInstance)
	, _elementRenderer(make_shared<CElementRenderer>(this))
	, _lastTime(0.f)
	, _defaultWhiteImage(make_shared<CWhiteImage>())
{

}

class IGameClass* CGameInstance::getClass()
{
	return _klass.lock().get();
}

void CGameInstance::postInputEvent(const CInputEvent& input)
{

}

void CGameInstance::think(float time)
{
	float DT = time - _lastTime;
	if ( DT > 1.f ) DT = 1.f;
	_lastTime = time;

	_vmInstance->think(time, DT);
}

void CGameInstance::render(IRenderer* renderer, CVec2 viewportSize, uint32 targetID /*= 0*/)
{
	_elementRenderer->setViewSize(viewportSize);
	_elementRenderer->beginFrame();

	CClipShape viewClip;
	viewClip.add(CHalfPlane(CVec2(-1,0), CVec2(0,0)));
	viewClip.add(CHalfPlane(CVec2(0,-1), CVec2(0,0)));
	viewClip.add(CHalfPlane(CVec2(1,0), CVec2(viewportSize.x,0)));
	viewClip.add(CHalfPlane(CVec2(0,1), CVec2(0,viewportSize.y)));

	_elementRenderer->setViewportClip(viewClip);

	_vmInstance->render(_elementRenderer);

	_elementRenderer->endFrame();

	renderer->render(_elementRenderer->getDrawBuffer().get());
}

void CGameInstance::initializeRenderer(IRenderer* renderer)
{
	ITextureProvider* provider = renderer->getTextureProvider();
	if ( provider == nullptr ) return;

	vector<ITexture* > *textureList = getTextureList(renderer, true);
	if ( textureList == nullptr ) return;

	ITexture* base = provider->loadTexture(renderer, _defaultWhiteImage.get());
	if ( base == nullptr ) return;
	textureList->push_back(base);

	shared_ptr<CGameClass> klass = _klass.lock();
	if ( klass == nullptr ) return;
	
	IPackage* pkg = klass->getPackage();
	if ( pkg == nullptr ) return;

	for ( uint32 i=0; i<pkg->getNumAssets(); ++i )
	{
		CImageAsset* image = castAsset<CImageAsset>(pkg->getAsset(i));
		if ( image == nullptr ) continue;

		ITexture* loaded = provider->loadTexture(renderer, image);	
		if ( loaded != nullptr )
		{
			log(LOG_MESSAGE, "Load Texture: %s[%i] (%ix%i)", *image->getName(), loaded->getID(), loaded->getWidth(), loaded->getHeight());
			textureList->push_back(loaded);
		}
	}
}

void CGameInstance::finishRenderer(IRenderer* renderer)
{
	ITextureProvider* provider = renderer->getTextureProvider();
	if ( provider == nullptr ) return;

	vector<ITexture* > *textureList = getTextureList(renderer);
	if ( textureList == nullptr ) return;

	for ( ITexture* tex : *textureList )
	{
		provider->freeTexture(renderer, tex);
	}

	delete textureList;
	_loadedTextures.erase(_loadedTextures.find(renderer));

	//_loadedTextures.clear();
}

bool CGameInstance::callFunction(CFunctionCall call)
{
	return _vmInstance->callFunction(call);
}

vector<ITexture*>* CGameInstance::getTextureList(IRenderer* renderer, bool newOnly)
{
	auto found = _loadedTextures.find(renderer);
	if ( found != _loadedTextures.end() )
	{
		if ( newOnly ) return nullptr;

		return (*found).second;
	}

	vector<ITexture*>* textures = new vector<ITexture*>();
	_loadedTextures.insert(make_pair(renderer, textures));
	return textures;
}
