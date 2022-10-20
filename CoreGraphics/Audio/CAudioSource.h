//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Audio/CAudioSource.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CAUDIOSOURCE_H
#define CAUDIOSOURCE_H

#include <Objects/CVComponent.h>

namespace Logic
{
	class CTransform;
};

namespace Audio
{
	enum class AudioCurveType
	{
		InvSquare,
		Linear,
		SmoothStep,
	};

	class CAudioSource : public CVComponent
	{
	public:
		struct Data
		{
			AudioCurveType curveType;
			float volume;
			float pitch;
			u32 loopCount;
			float innerRadius;
			float distanceScale;
			float spatialBlend;
			float spread;
			float dopplerScale;

			Logic::CTransform* pTransform;
			class CAudioClip* pClip;
		};

	public:
		CAudioSource(const CVObject* pObject);
		~CAudioSource();
		CAudioSource(const CAudioSource&) = delete;
		CAudioSource(CAudioSource&&) = delete;
		CAudioSource& operator = (const CAudioSource&) = delete;
		CAudioSource& operator = (CAudioSource&&) = delete;

		void Initialize() = 0;

		virtual void Play();
		virtual void Resume();
		virtual void Pause();
		virtual void Stop();

		void Release() = 0;

		inline void SetData(const Data& data) { m_data = data; }

	protected:
		bool m_bActive;

		Data m_data;
	};
};

#endif
