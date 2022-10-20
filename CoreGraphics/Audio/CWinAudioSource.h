//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Audio/CWinAudioSource.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CWINAUDIOSOURCE_H
#define CWINAUDIOSOURCE_H

#include "CAudioSource.h"
#include <Math/CSIMDVector.h>
#include <xaudio2.h>
#include <x3daudio.h>

namespace Audio
{
	class CWinAudioSource : public CAudioSource
	{
	public:
		CWinAudioSource(const CVObject* pObject);
		~CWinAudioSource();
		CWinAudioSource(const CWinAudioSource&) = delete;
		CWinAudioSource(CWinAudioSource&&) = delete;
		CWinAudioSource& operator = (const CWinAudioSource&) = delete;
		CWinAudioSource& operator = (CWinAudioSource&&) = delete;

		void Initialize() final;
		void Release() final;

		void Play() final;
		void Resume() final;
		void Pause() final;
		void Stop() final;

	private:
		void UpdateEmmitterSettings();
		void UpdateEmmitter();

		float EvaluateVolumeCurve(float dist) const;

		void PlayAPI();
		void ResumeAPI();
		void PauseAPI();
		void StopAPI();

	private:
		Math::SIMDVector m_lastPosition;

		X3DAUDIO_DSP_SETTINGS m_dspSettings;
		X3DAUDIO_EMITTER m_emitter;
		X3DAUDIO_CONE m_cone;

		WAVEFORMATEX m_wfx;
		XAUDIO2_BUFFER m_buffer;

		IXAudio2SourceVoice* m_pVoice;
		class CWinAudio* m_pAudio;
	};
};

#endif
