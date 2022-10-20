//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Audio/CAudioData.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CAUDIODATA_H
#define CAUDIODATA_H

#include <Globals/CGlobals.h>
#include <Utilities/CMemoryFree.h>

namespace Audio
{
	enum class AudioFormatType : u8
	{
		Unknown,
		Wave,
		Ogg,
	};

	struct AudioInfo
	{
		AudioFormatType type;
		bool bStream;

		u16 audioFormat;
		u16 bitsPerSample;
		u16 numChannels;
		u16 blockAlign;
		u32 sampleRate;
		u32 byteRate;
		u32 dataChunkSize;
		u32 dataOffset;

		u32 loopStart;
		u32 loopEnd;

		char* pBuffer;

		~AudioInfo()
		{
			SAFE_DELETE_ARRAY(pBuffer);
		}
	};
};

#endif
