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
channel.h:
===============================================================================
*/

#pragma once

#include "engine/private/indexallocator.h"

namespace Arcade
{

struct CChannelState
{
	CChannelState()
		: _loop(false)
		, _time(0.0)
		, _pitch(1.0)
		, _volume(0.5)
		, _duration(0.0)
	{
	
	}

	bool _loop;
	float _pitch;
	float _volume;
	double _time;
	double _duration;
};

class CSoundMixer;
class CSoundChannel : public ISoundChannel
{
public:
	CSoundChannel(CSoundMixer *mixer, CIndex index, EChannelMode mode = CHANNELMODE_DEFAULT);

	void play(shared_ptr<class ISoundSample> sample);

	bool generatePCM(float* buffer, uint32 offset, uint32 size);

	void update();

	uint32 getIndex() const;

	EChannelMode getMode() const;

	CChannelState& getState();

private:
	friend class CSoundMixer;
	friend class CRuntimeObject;

	weak_ptr<class ISoundSample> _sample;
	CSoundMixer *_mixer;
	CIndex _index;
	EChannelMode _mode;
	CChannelState _state;
	CSampleInfo _sampleInfo;

	float _rate;
	float _invRate;
	int32 _stride;

	bool generateSinglePCMSample(float* buffer, uint32 offset, ISoundSample* sample, const CMixerSettings& mixerSettings);
};

}