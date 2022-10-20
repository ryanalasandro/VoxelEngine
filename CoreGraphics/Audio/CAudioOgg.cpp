//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Audio/CAudioOgg.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CAudioOgg.h"

#include <fstream>
#include <stdio.h>
#include <cassert>

#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

namespace Audio
{
	void CAudioOgg::Load(const std::wstring filename, AudioInfo& audioInfo)
	{
		int err;
		OggVorbis_File vf;

		FILE* pFile;
		_wfopen_s(&pFile, filename.c_str(), L"rb");
		assert(pFile);

		err = ov_open_callbacks(pFile, &vf, nullptr, 0, OV_CALLBACKS_NOCLOSE);
		assert(err == 0);

		{ // Gather file info.
			vorbis_info* vi = ov_info(&vf, -1);
			audioInfo.sampleRate = vi->rate;
			audioInfo.numChannels = vi->channels;
			audioInfo.bitsPerSample = 16;
			audioInfo.audioFormat = 1;
			audioInfo.blockAlign = audioInfo.numChannels * 2;
			audioInfo.byteRate = audioInfo.sampleRate * audioInfo.blockAlign;
			audioInfo.dataOffset = 0;
			audioInfo.dataChunkSize = static_cast<u32>(ov_pcm_total(&vf, -1)) * audioInfo.blockAlign;
		}

		if(!audioInfo.bStream)
		{ // Read audio data.
			audioInfo.pBuffer = new char[audioInfo.dataChunkSize];

			u32 dataOffset = 0;
			char buffer[4096];
			int read;
			int currentSection;
			do
			{
				read = ov_read(&vf, buffer, 4096, 0, 2, 1, &currentSection);
				if(read > 0)
				{
					// Simply copy the PCM data into the audio buffer.
					//	There might be need for more care taken for property changes in the stream down the line.
					memcpy(audioInfo.pBuffer + dataOffset, buffer, read);
					dataOffset += read;
				}
			} while(read > 0);
		}

		err = ov_clear(&vf);
		assert(err == 0);

		fclose(pFile);
	}
};
