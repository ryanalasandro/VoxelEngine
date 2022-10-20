//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Audio/CAudioMixer.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CAudioMixer.h"
#include <Utilities/CMemoryFree.h>

#ifdef PLATFORM_WINDOWS
#include "CWinAudio.h"
#endif
#ifdef PLATFORM_LINUX
#endif

namespace Audio
{
	CAudioMixer::CAudioMixer() :
		m_pAudio(nullptr) {
	}

	CAudioMixer::~CAudioMixer() { }

	void CAudioMixer::Intialize()
	{
#ifdef PLATFORM_WINDOWS
		m_pAudio = new CWinAudio();
#endif
#ifdef PLATFORM_LINUX
		m_pAudio = new CLinuxAudio();
#endif

		m_pAudio->Initialize();
	}

	void CAudioMixer::Release()
	{
		SAFE_RELEASE_DELETE(m_pAudio);
	}
};
