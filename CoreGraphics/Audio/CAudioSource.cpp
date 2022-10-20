//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Audio/CAudioSource.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CAudioSource.h"

namespace Audio
{
	CAudioSource::CAudioSource(const CVObject* pObject) :
		CVComponent(pObject),
		m_bActive(false) {
	}

	CAudioSource::~CAudioSource() { }

	void CAudioSource::Play()
	{
		m_bActive = true;
	}

	void CAudioSource::Resume() { }
	void CAudioSource::Pause() { }

	void CAudioSource::Stop()
	{
		m_bActive = false;
	}
};
