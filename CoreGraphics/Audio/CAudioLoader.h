//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Audio/CAudioLoader.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CAUDIOLOADER_H
#define CAUDIOLOADER_H

#include "CAudioData.h"
#include <string>

namespace Audio
{
	class CAudioLoader
	{
	public:
		static void Load(const std::wstring& filename, AudioInfo& audioInfo);

	private:
		static void LoadMetaData(const std::wstring& filename, AudioInfo& audioInfo);
	};
};

#endif
