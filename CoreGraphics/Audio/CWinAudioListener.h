//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Audio/CWinAudioListener.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CWINAUDIOLISTENER_H
#define CWINAUDIOLISTENER_H

#include "CAudioListener.h"
#include <Math/CSIMDVector.h>
#include <xaudio2.h>
#include <x3daudio.h>

namespace Audio
{
	class CWinAudioListener : public CAudioListener
	{
	public:
		CWinAudioListener(const CVObject* pObject);
		~CWinAudioListener();
		CWinAudioListener(const CWinAudioListener&) = delete;
		CWinAudioListener(CWinAudioListener&&) = delete;
		CWinAudioListener& operator = (const CWinAudioListener&) = delete;
		CWinAudioListener& operator = (CWinAudioListener&&) = delete;

		void Initialize() final;
		void LateUpdate() final;
		void Release() final;

		inline const X3DAUDIO_LISTENER* Getx3dListener() { return &m_x3dListener; }

	private:
		Math::SIMDVector m_lastPosition;

		X3DAUDIO_CONE m_cone;
		X3DAUDIO_LISTENER m_x3dListener;
	};
};

#endif
