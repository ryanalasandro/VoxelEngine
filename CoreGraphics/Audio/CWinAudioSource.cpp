//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Audio/CWinAudioSource.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CWinAudioSource.h"
#include "CWinAudio.h"
#include "CWinAudioListener.h"
#include "CAudioClip.h"
#include "../Application/CSceneManager.h"
#include <Logic/CTransform.h>
#include <Utilities/CTimer.h>
#include <Utilities/CDebugError.h>

namespace Audio
{
	static const X3DAUDIO_DISTANCE_CURVE_POINT AUDIO_CURVE_SMOOTHSTEP_POINTS[21] {
		{ 0.0f,  1.0f - Math::SmoothStep(0.0f, 1.0f, 0.0f) },
		{ 0.05f, 1.0f - Math::SmoothStep(0.0f, 1.0f, 0.05f) },
		{ 0.1f,  1.0f - Math::SmoothStep(0.0f, 1.0f, 0.1f) },
		{ 0.15f, 1.0f - Math::SmoothStep(0.0f, 1.0f, 0.15f) },
		{ 0.2f,  1.0f - Math::SmoothStep(0.0f, 1.0f, 0.2f) },
		{ 0.25f, 1.0f - Math::SmoothStep(0.0f, 1.0f, 0.25f) },
		{ 0.3f,  1.0f - Math::SmoothStep(0.0f, 1.0f, 0.3f) },
		{ 0.35f, 1.0f - Math::SmoothStep(0.0f, 1.0f, 0.35f) },
		{ 0.4f,  1.0f - Math::SmoothStep(0.0f, 1.0f, 0.4f) },
		{ 0.45f, 1.0f - Math::SmoothStep(0.0f, 1.0f, 0.45f) },
		{ 0.5f,  1.0f - Math::SmoothStep(0.0f, 1.0f, 0.5f) },
		{ 0.55f, 1.0f - Math::SmoothStep(0.0f, 1.0f, 0.55f) },
		{ 0.6f,  1.0f - Math::SmoothStep(0.0f, 1.0f, 0.6f) },
		{ 0.65f, 1.0f - Math::SmoothStep(0.0f, 1.0f, 0.65f) },
		{ 0.7f,  1.0f - Math::SmoothStep(0.0f, 1.0f, 0.7f) },
		{ 0.75f, 1.0f - Math::SmoothStep(0.0f, 1.0f, 0.75f) },
		{ 0.8f,  1.0f - Math::SmoothStep(0.0f, 1.0f, 0.8f) },
		{ 0.85f, 1.0f - Math::SmoothStep(0.0f, 1.0f, 0.85f) },
		{ 0.9f,  1.0f - Math::SmoothStep(0.0f, 1.0f, 0.9f) },
		{ 0.95f, 1.0f - Math::SmoothStep(0.0f, 1.0f, 0.95f) },
		{ 1.0f,  1.0f - Math::SmoothStep(0.0f, 1.0f, 1.0f) },
	};

	static const X3DAUDIO_DISTANCE_CURVE AUDIO_CURVE_SMOOTHSTEP = { (X3DAUDIO_DISTANCE_CURVE_POINT*)AUDIO_CURVE_SMOOTHSTEP_POINTS, 21 };

	//-----------------------------------------------------------------------------------------------
	// Audio Source.
	//-----------------------------------------------------------------------------------------------

	CWinAudioSource::CWinAudioSource(const CVObject* pObject) :
		CAudioSource(pObject),
		m_pAudio(nullptr)
	{
		memset(&m_dspSettings, 0, sizeof(m_dspSettings));
		memset(&m_emitter, 0, sizeof(m_emitter));
		memset(&m_cone, 0, sizeof(m_cone));

		memset(&m_wfx, 0, sizeof(m_wfx));
		memset(&m_buffer, 0, sizeof(m_buffer));
	}

	CWinAudioSource::~CWinAudioSource() { }

	void CWinAudioSource::Initialize()
	{
		m_pAudio = dynamic_cast<CWinAudio*>(App::CSceneManager::Instance().AudioMixer().Audio());

		// Setup the wave format.
		m_wfx.wFormatTag = m_data.pClip->GetInfo().audioFormat;
		m_wfx.nSamplesPerSec = m_data.pClip->GetInfo().sampleRate;
		m_wfx.wBitsPerSample = (m_data.pClip->GetInfo().bitsPerSample + 0xF) & ~0xF;
		m_wfx.nChannels = m_data.pClip->GetInfo().numChannels;
		m_wfx.nBlockAlign = m_data.pClip->GetInfo().blockAlign;
		m_wfx.nAvgBytesPerSec = m_data.pClip->GetInfo().byteRate;

		// Setup the audio source.
		XAUDIO2_VOICE_DETAILS details;
		m_pAudio->GetMasterVoice()->GetVoiceDetails(&details);

		// Setup the dsp settings.
		m_dspSettings.SrcChannelCount = m_wfx.nChannels;
		m_dspSettings.DstChannelCount = details.InputChannels;
		if(m_data.spatialBlend < 1.0f)
		{
			size_t matrixSz = m_dspSettings.SrcChannelCount * m_dspSettings.DstChannelCount;
			m_dspSettings.pMatrixCoefficients = new float[matrixSz + 2];
			memset(m_dspSettings.pMatrixCoefficients, 0, sizeof(float) * matrixSz);
		}

		// Call emmitter settings updater.
		UpdateEmmitterSettings();

		IXAudio2VoiceCallback* pVoiceCallback = nullptr;
		if(m_data.pClip->GetInfo().bStream)
		{
			//pVoiceCallback = m_data.context;
		}
		else
		{
			m_buffer.LoopCount = m_data.loopCount == ~0 ? XAUDIO2_LOOP_INFINITE : m_data.loopCount;
			m_buffer.LoopBegin = m_data.pClip->GetInfo().loopStart;
			m_buffer.LoopLength = m_data.pClip->GetInfo().loopEnd - m_data.pClip->GetInfo().loopStart;
			m_buffer.AudioBytes = m_data.pClip->GetInfo().dataChunkSize;
			m_buffer.pAudioData = reinterpret_cast<const BYTE*>(m_data.pClip->GetInfo().pBuffer);
			m_buffer.Flags = XAUDIO2_END_OF_STREAM;
		}

		ASSERT_HR_R(m_pAudio->GetAudio()->CreateSourceVoice(&m_pVoice, &m_wfx, 0, 2.0f, pVoiceCallback));

		if(m_data.pTransform)
		{
			m_lastPosition = m_data.pTransform->GetPosition();
		}
		else
		{
			m_lastPosition = Math::SIMD_VEC_ZERO;
		}
	}

	void CWinAudioSource::UpdateEmmitterSettings()
	{
		// Setup the emmitter cone.
		m_cone.InnerAngle = Math::g_2Pi;
		m_cone.InnerVolume = 1.0f;
		m_cone.OuterAngle = Math::g_2Pi;
		m_cone.OuterVolume = 0.0f;
		m_cone.InnerLPF = 0.0f;
		m_cone.OuterLPF = 1.0f;
		m_cone.InnerReverb = 0.0f;
		m_cone.OuterReverb = 1.0f;

		// Setup the emmitter.
		m_emitter.ChannelCount = m_dspSettings.SrcChannelCount;
		if(m_emitter.ChannelCount > 1)
		{
			m_emitter.ChannelRadius = 0.0f;
			m_emitter.pChannelAzimuths = new float[m_dspSettings.SrcChannelCount];
			memset(m_emitter.pChannelAzimuths, 0, sizeof(float) * m_emitter.ChannelCount);
		}

		m_emitter.InnerRadius = m_data.innerRadius;
		m_emitter.InnerRadiusAngle = X3DAUDIO_PI * 0.25f;
		m_emitter.CurveDistanceScaler = m_data.distanceScale;

		switch(m_data.curveType)
		{
			case Audio::AudioCurveType::InvSquare:
				m_emitter.pVolumeCurve = nullptr;
				break;
			case Audio::AudioCurveType::Linear:
				m_emitter.pVolumeCurve = (X3DAUDIO_DISTANCE_CURVE*)(&X3DAudioDefault_LinearCurve);
				break;
			case Audio::AudioCurveType::SmoothStep:
				m_emitter.pVolumeCurve = (X3DAUDIO_DISTANCE_CURVE*)(&AUDIO_CURVE_SMOOTHSTEP);
				break;
			default:
				break;
		}

		m_emitter.pCone = &m_cone;
		m_emitter.DopplerScaler = m_data.dopplerScale;
	}

	void CWinAudioSource::UpdateEmmitter()
	{
		auto SourcePitch = [this]() {
			return powf(2.0f, (m_data.pitch - 1.0f) * 10.0f);
		};

		if(m_data.spatialBlend < 1.0f)
		{
			if(m_data.pTransform)
			{
				Math::SIMDVector velocity = (m_data.pTransform->GetPosition() - m_lastPosition) / Util::CTimer::Instance().GetDelta();

				m_emitter.Position = *reinterpret_cast<const X3DAUDIO_VECTOR*>(m_data.pTransform->GetPosition().ToFloat());
				m_emitter.OrientFront = *reinterpret_cast<const X3DAUDIO_VECTOR*>(m_data.pTransform->GetForward().ToFloat());
				m_emitter.OrientTop = *reinterpret_cast<const X3DAUDIO_VECTOR*>(m_data.pTransform->GetUp().ToFloat());
				m_emitter.Velocity = *reinterpret_cast<const X3DAUDIO_VECTOR*>(velocity.ToFloat());

				m_lastPosition = m_data.pTransform->GetPosition();
			}

			DWORD dwCalcFlags = 0;
			dwCalcFlags |= X3DAUDIO_CALCULATE_MATRIX;
			//dwCalcFlags |= X3DAUDIO_CALCULATE_LPF_DIRECT;
			//dwCalcFlags |= X3DAUDIO_CALCULATE_LPF_REVERB;
			//dwCalcFlags |= X3DAUDIO_CALCULATE_REVERB;
			//dwCalcFlags |= X3DAUDIO_CALCULATE_DOPPLER;

			CWinAudioListener* pListerner = dynamic_cast<CWinAudioListener*>(m_pAudio->GetListener());
			X3DAudioCalculate(m_pAudio->Getx3dInstance(), pListerner->Getx3dListener(), &m_emitter,
				dwCalcFlags, &m_dspSettings);

			{
				const size_t matrixSz = m_dspSettings.SrcChannelCount * m_dspSettings.DstChannelCount;
				const Math::Vector3 lPos = *(Math::Vector3*)&pListerner->Getx3dListener()->Position;
				const Math::Vector3 ePos = *(Math::Vector3*)&m_emitter.Position;
				const Math::Vector3 diff = ePos - lPos;
				const float dist = diff.LengthSq() > 1e-10f ? diff.Length() : 0.0f;
				const float volume = EvaluateVolumeCurve(dist);

				for(size_t i = 0; i < matrixSz; ++i)
				{
					m_dspSettings.pMatrixCoefficients[i] = Math::Saturate((m_dspSettings.pMatrixCoefficients[i] * (1.0f - m_data.spread) + volume * m_data.spread) *
						m_data.spatialBlend + (1.0f - m_data.spatialBlend));
				}
			}
		}
		else
		{
			// 2D sound.
			m_pVoice->SetFrequencyRatio(SourcePitch());
		}
	}

	void CWinAudioSource::Release()
	{
		SAFE_DELETE_ARRAY(m_emitter.pChannelAzimuths);
		if(m_pVoice) m_pVoice->DestroyVoice();
		SAFE_DELETE_ARRAY(m_dspSettings.pMatrixCoefficients);
	}

	//-----------------------------------------------------------------------------------------------
	// State methods.
	//-----------------------------------------------------------------------------------------------

	void CWinAudioSource::Play()
	{
		UpdateEmmitter();

		StopAPI();
		PlayAPI();
		CAudioSource::Play();
	}

	void CWinAudioSource::Resume()
	{
		ResumeAPI();
		CAudioSource::Resume();
	}

	void CWinAudioSource::Pause()
	{
		PauseAPI();
		CAudioSource::Pause();
	}

	void CWinAudioSource::Stop()
	{
		StopAPI();
		CAudioSource::Stop();
	}

	//-----------------------------------------------------------------------------------------------
	// API methods.
	//-----------------------------------------------------------------------------------------------

	void CWinAudioSource::PlayAPI()
	{
		if(!m_data.pClip->GetInfo().bStream)
		{
			ASSERT_HR_R(m_pVoice->SubmitSourceBuffer(&m_buffer));
		}

		ASSERT_HR_R(m_pVoice->SetVolume(m_data.volume));
		ASSERT_HR_R(m_pVoice->Start());
	}

	void CWinAudioSource::ResumeAPI()
	{
		ASSERT_HR_R(m_pVoice->Start());
	}

	void CWinAudioSource::PauseAPI()
	{
		ASSERT_HR_R(m_pVoice->Stop(XAUDIO2_PLAY_TAILS));
	}

	void CWinAudioSource::StopAPI()
	{
		ASSERT_HR_R(m_pVoice->Stop(XAUDIO2_PLAY_TAILS));
		ASSERT_HR_R(m_pVoice->FlushSourceBuffers());
	}

	//-----------------------------------------------------------------------------------------------
	// Utility methods.
	//-----------------------------------------------------------------------------------------------

	// Method for evaluating the volume curve with a distance.
	float CWinAudioSource::EvaluateVolumeCurve(float dist) const
	{
		if(m_emitter.pVolumeCurve == nullptr)
		{
			return dist <= m_emitter.CurveDistanceScaler ? 1.0f : (m_emitter.CurveDistanceScaler / dist);
		}

		if(isnan(dist))
		{
			dist = 0.0f;
		}
		else
		{
			dist /= m_emitter.CurveDistanceScaler;
			if(dist <= 0.0f) return m_emitter.pVolumeCurve->pPoints[0].DSPSetting;
			if(dist >= 1.0f) return m_emitter.pVolumeCurve->pPoints[m_emitter.pVolumeCurve->PointCount - 1].DSPSetting;
			dist *= (m_emitter.pVolumeCurve->PointCount - 1);
		}

		int index = int(floorf(dist));
		return Math::Lerp(m_emitter.pVolumeCurve->pPoints[index].DSPSetting,
			m_emitter.pVolumeCurve->pPoints[index + 1].DSPSetting, Math::Saturate(dist - index));
	}
};
