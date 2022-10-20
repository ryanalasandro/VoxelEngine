//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Audio/CWinAudioListener.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CWinAudioListener.h"
#include <Logic/CTransform.h>
#include <Utilities/CTimer.h>

namespace Audio
{
	CWinAudioListener::CWinAudioListener(const CVObject* pObject) :
		CAudioListener(pObject)
	{
		memset(&m_x3dListener, 0, sizeof(m_x3dListener));
		memset(&m_cone, 0, sizeof(m_cone));
	}

	CWinAudioListener::~CWinAudioListener() { }

	void CWinAudioListener::Initialize()
	{
		memset(&m_x3dListener, 0, sizeof(m_x3dListener));

		m_cone.InnerAngle = X3DAUDIO_2PI;
		m_cone.InnerVolume = 1.0f;
		m_cone.OuterAngle = X3DAUDIO_2PI;
		m_cone.OuterVolume = 0.0f;

		if(m_data.pTransform)
		{
			m_lastPosition = m_data.pTransform->GetPosition();
		}
		else
		{
			m_lastPosition = Math::SIMD_VEC_ZERO;
		}
	}

	void CWinAudioListener::LateUpdate()
	{
		if(m_data.pTransform)
		{
			Math::SIMDVector velocity = (m_data.pTransform->GetPosition() - m_lastPosition) / Util::CTimer::Instance().GetDelta();

			const float* p = m_data.pTransform->GetPosition().ToFloat();
			const float* f = m_data.pTransform->GetForward().ToFloat();
			const float* u = m_data.pTransform->GetUp().ToFloat();
			const float* v = velocity.ToFloat();

			m_x3dListener.Position = { p[0], p[1], p[2] };
			m_x3dListener.OrientFront = { f[0], f[1], f[2] };
			m_x3dListener.OrientTop = { u[0], u[1], u[2] };
			m_x3dListener.Velocity = { v[0], v[1], v[2] };

			m_lastPosition = m_data.pTransform->GetPosition();
		}
	}

	void CWinAudioListener::Release()
	{ }
};
