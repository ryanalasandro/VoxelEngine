//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Audio/CAudioListener.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CAUDIOLISTENER_H
#define CAUDIOLISTENER_H

#include <Objects/CVComponent.h>

namespace Logic
{
	class CTransform;
};

namespace Audio
{
	class CAudioListener : public CVComponent
	{
	public:
		struct Data
		{
			Logic::CTransform* pTransform;
		};

	protected:
		CAudioListener(const CVObject* pObject);
		CAudioListener(const CAudioListener&) = delete;
		CAudioListener(CAudioListener&&) = delete;
		CAudioListener& operator = (const CAudioListener&) = delete;
		CAudioListener& operator = (CAudioListener&&) = delete;

	public:
		virtual ~CAudioListener();

		void Initialize() = 0;
		void LateUpdate() = 0;
		void Release() = 0;

		inline void SetData(const Data& data) { m_data = data; }

	protected:
		Data m_data;
	};
};

#endif
