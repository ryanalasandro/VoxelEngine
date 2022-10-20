//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Audio/CAudioClip.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CAudioClip.h"
#include "CAudioLoader.h"

namespace Audio
{
	CAudioClip::CAudioClip() { }
	CAudioClip::~CAudioClip() { }

	void CAudioClip::Initialize()
	{
		memset(&m_info, 0, sizeof(m_info));
		CAudioLoader::Load(m_data.filename, m_info);
	}

	void CAudioClip::Release() { }
};
