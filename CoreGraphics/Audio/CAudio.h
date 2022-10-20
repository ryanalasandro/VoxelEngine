//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Audio/CAudio.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CAUDIO_H
#define CAUDIO_H

class CVObject;

namespace Audio
{
	class CAudio
	{
	public:
		CAudio();
		virtual ~CAudio();
		CAudio(const CAudio&) = delete;
		CAudio(CAudio&&) = delete;
		CAudio& operator = (const CAudio&) = delete;
		CAudio& operator = (CAudio&&) = delete;

		virtual void Initialize() = 0;
		virtual void Release() = 0;

		virtual class CAudioListener* CreateListener(const CVObject* pObject) = 0;
		virtual class CAudioSource* CreateSource(const CVObject* pObject) = 0;

		inline class CAudioListener* GetListener() const { return m_pListener; }

	protected:
		class CAudioListener* m_pListener;
	};
};

#endif
