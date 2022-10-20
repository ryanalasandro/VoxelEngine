//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Audio/CAudioWave.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CAudioWave.h"
#include <fstream>
#include <cassert>

namespace Audio
{
	void CAudioWave::Load(const std::wstring filename, AudioInfo& audioInfo)
	{
		std::ifstream file(filename);
		assert(file.is_open());

		HEADER header;

		file.read(reinterpret_cast<char*>(&header.chunkId), sizeof(header.chunkId));
		assert(header.chunkId == WAVE_CHUNK_ID);
		file.read(reinterpret_cast<char*>(&header.chunkSize), sizeof(header.chunkSize));

		file.read(reinterpret_cast<char*>(&header.format), sizeof(header.format));
		file.read(reinterpret_cast<char*>(&header.subChunkId), sizeof(header.subChunkId));
		assert(header.subChunkId == WAVE_SUB_CHUNK_ID);

		file.read(reinterpret_cast<char*>(&header.subChunkSize), sizeof(header.subChunkSize));
		file.read(reinterpret_cast<char*>(&header.audioFormat), sizeof(header.audioFormat));
		assert(header.audioFormat == 1 || header.audioFormat == 3); // PCM is 1, other values mean compression.

		file.read(reinterpret_cast<char*>(&header.numChannels), sizeof(header.numChannels));
		file.read(reinterpret_cast<char*>(&header.sampleRate), sizeof(header.sampleRate));
		file.read(reinterpret_cast<char*>(&header.byteRate), sizeof(header.byteRate));
		file.read(reinterpret_cast<char*>(&header.blockAlign), sizeof(header.blockAlign));
		file.read(reinterpret_cast<char*>(&header.bitsPerSample), sizeof(header.bitsPerSample));

		file.read(reinterpret_cast<char*>(&header.dataChunkId), sizeof(header.dataChunkId));
		if(header.dataChunkId != WAVE_DATA_CHUNK_ID)
		{
			// There is extra header data, though this is still a PCM format.
			u32 listSz;
			file.read(reinterpret_cast<char*>(&listSz), sizeof(listSz));

			file.seekg(listSz, std::ios::cur);
			file.read(reinterpret_cast<char*>(&header.dataChunkId), sizeof(header.dataChunkId));
			assert(header.dataChunkId == WAVE_DATA_CHUNK_ID);
		}

		file.read(reinterpret_cast<char*>(&header.dataChunkSize), sizeof(header.dataChunkSize));

		audioInfo.audioFormat = header.audioFormat;
		audioInfo.sampleRate = header.sampleRate;
		audioInfo.byteRate = header.byteRate;
		audioInfo.bitsPerSample = header.bitsPerSample;
		audioInfo.numChannels = header.numChannels;
		audioInfo.blockAlign = header.blockAlign;
		audioInfo.dataChunkSize = header.dataChunkSize;
		audioInfo.dataOffset = static_cast<u32>(file.tellg());

		if(!audioInfo.bStream)
		{
			audioInfo.pBuffer = new char[header.dataChunkSize];
			file.read(audioInfo.pBuffer, header.dataChunkSize);
		}

		file.close();
	}
};
