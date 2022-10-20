//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Audio/CAudioOgg.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CAUDIOOGG_H
#define CAUDIOOGG_H

#include "CAudioData.h"
#include <string>

#pragma comment(lib, "ogg.lib")
#pragma comment(lib, "vorbis.lib")
#pragma comment(lib, "vorbisfile.lib")

namespace Audio
{
	class CAudioOgg
	{
	public:
		static void Load(const std::wstring filename, AudioInfo& audioInfo);
	};
};

#endif
