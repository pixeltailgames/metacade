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
compiler.cpp: Compiles IAssets
===============================================================================
*/

#include "metacade_amal.h"

using namespace Arcade;

#include "IL/il.h"
#include "compiler.h"
#include "SDL.h"
#include <iostream>
#include <fstream>

static const uint8 bitReversalTable[] = 
{
	0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0, 0x10, 0x90, 0x50, 0xD0, 0x30, 0xB0, 0x70, 0xF0,
	0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8, 0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8,
	0x04, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4, 0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4,
	0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC, 0x1C, 0x9C, 0x5C, 0xDC, 0x3C, 0xBC, 0x7C, 0xFC,
	0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2, 0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2,
	0x0A, 0x8A, 0x4A, 0xCA, 0x2A, 0xAA, 0x6A, 0xEA, 0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,
	0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6, 0x16, 0x96, 0x56, 0xD6, 0x36, 0xB6, 0x76, 0xF6,
	0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE, 0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE,
	0x01, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61, 0xE1, 0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1,
	0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9, 0x19, 0x99, 0x59, 0xD9, 0x39, 0xB9, 0x79, 0xF9,
	0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5, 0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5,
	0x0D, 0x8D, 0x4D, 0xCD, 0x2D, 0xAD, 0x6D, 0xED, 0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,
	0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3, 0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3,
	0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB, 0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB,
	0x07, 0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7, 0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7,
	0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF, 0x1F, 0x9F, 0x5F, 0xDF, 0x3F, 0xBF, 0x7F, 0xFF
};

static bool buildImage(CImageAsset* asset, IMetaData* params)
{
	ILuint test = ilGenImage();
	ilBindImage(test);
	if ( !ilLoadImage(*params->getValue("file").toString()) ) return false;

	CString flags = params->getValue("params").toString();

	if ( flags.contains("-wrap-x") ) asset->setFlag(IF_WRAP_X);
	if ( flags.contains("-wrap-y") ) asset->setFlag(IF_WRAP_Y);
	if ( flags.contains("-smooth") ) asset->setFlag(IF_SMOOTH);

	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

	int width = (int) ilGetInteger(IL_IMAGE_WIDTH);
	int height = (int) ilGetInteger(IL_IMAGE_HEIGHT);
	
	uint8* data = (uint8*) ilGetData();
	asset->setImagePixels(PFM_RGBA8, 4, width, height, data);
	ilDeleteImage(test);

	return true;
}

static bool buildSound(CSoundAsset* sound, IMetaData* params)
{
	SDL_AudioSpec sndFormat;
	uint8* waveBuffer;
	uint32 waveSize;

	if ( SDL_LoadWAV(*params->getValue("file").toString(), &sndFormat, &waveBuffer, &waveSize) == NULL ) 
	{
		std::cout << "Unable to open file: " << *params->getValue("file").toString() << std::endl;
		return false;
	}

	if ( sndFormat.channels > 2 )
	{
		std::cout << "Too many audio channels" << std::endl;
		SDL_FreeWAV(waveBuffer);
		return false;
	}

	int32 isFloatingPoint = sndFormat.format & SDL_AUDIO_MASK_DATATYPE;
	int32 bitCount = sndFormat.format & SDL_AUDIO_MASK_BITSIZE;
	int32 convertEndian = sndFormat.format & SDL_AUDIO_MASK_ENDIAN;
	int32 convertSign = !(sndFormat.format & SDL_AUDIO_MASK_SIGNED);

	if ( isFloatingPoint )
	{
		std::cout << "Floating point not supported" << std::endl;
		SDL_FreeWAV(waveBuffer);
		return false;	
	}

	if ( bitCount != 8 && bitCount != 16 )
	{
		std::cout << "Unsupported sample size (" << bitCount << " bits)" << std::endl;
		SDL_FreeWAV(waveBuffer);
		return false;	
	}

	if ( bitCount == 8 )
	{
		if ( convertEndian )
		{
			std::cout << "Performing endianness conversion..." << std::endl;
			for ( uint32 i=0; i<waveSize; ++i )
				waveBuffer[i] = bitReversalTable[ waveBuffer[i] ];
		}

		if ( convertSign )
		{
			std::cout << "Performing sign conversion..." << std::endl;
			const uint8 flip = 0x80;
			for ( uint32 i=0; i<waveSize; ++i )
				waveBuffer[i] ^= flip;
		}
	}
	else if ( bitCount == 16 )
	{
		if ( convertEndian )
		{
			std::cout << "Performing endianness conversion..." << std::endl;
			uint16* sample16 = (uint16* ) waveBuffer;
			for ( uint32 i=0; i<waveSize/2; ++i )
				sample16[i] = (bitReversalTable[ sample16[i] & 0xFF ] << 8) | (bitReversalTable[ sample16[i] >> 8 ]);
		}

		if ( convertSign )
		{
			std::cout << "Performing sign conversion..." << std::endl;
			uint16* sample16 = (uint16* ) waveBuffer;
			const uint16 flip = 0x8000;
			for ( uint32 i=0; i<waveSize/2; ++i )
				sample16[i] ^= flip;
		}
	}

	CSampleInfo info;
	memset(&info, 0, sizeof(CSampleInfo));

	info.numChannels = sndFormat.channels;
	info.numFrames = (waveSize / (bitCount >> 3)) / info.numChannels;
	info.width = bitCount;
	info.sampleRate = sndFormat.freq;

	sound->setSampleInfo(info);
	sound->setWaveData(waveBuffer, waveSize);

	SDL_FreeWAV(waveBuffer);

	return true;
}

static bool buildCode(CCodeAsset* code, IMetaData* params)
{
	std::ifstream input(*params->getValue("file").toString(), std::ios::binary | std::ios::ate);

	if ( !input.is_open() ) return false;

	uint32 size = (uint32) input.tellg();
	input.seekg(std::ios::beg);

	uint8* buffer = new uint8[size];
	input.read((char*)buffer, size);

	code->setCodeBuffer((const char*)buffer, size);
	delete [] buffer;

	return code->validate();
}

CCompiler::CCompiler()
{
	ilInit();
}

CCompiler::~CCompiler()
{
	ilShutDown();
}

bool CCompiler::compile(IAsset* asset, IMetaData* buildParameters)
{
	switch (asset->getType())
	{
	case ASSET_NONE:
	break;
	case ASSET_CODE:
	return buildCode((CCodeAsset*)asset, buildParameters);
	break;
	case ASSET_TEXTURE:
	return buildImage((CImageAsset*)asset, buildParameters);
	break;
	case ASSET_SOUND:
	return buildSound((CSoundAsset*)asset, buildParameters);
	break;
	default:
	break;
	}

	return false;
}
