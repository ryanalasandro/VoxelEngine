//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Audio/CAudioClip.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CAUDIOCLIP_H
#define CAUDIOCLIP_H

#include "CAudioData.h"
#include <string>

namespace Audio
{
	class CAudioClip
	{
	public:
		struct Data
		{
			std::wstring filename;
		};

	public:
		CAudioClip();
		~CAudioClip();
		CAudioClip(const CAudioClip&) = delete;
		CAudioClip(CAudioClip&&) = delete;
		CAudioClip& operator = (const CAudioClip&) = delete;
		CAudioClip& operator = (CAudioClip&&) = delete;

		void Initialize();
		void Release();

		// Accessors.
		inline const AudioInfo& GetInfo() const { return m_info; }

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }

	private:
		Data m_data;
		AudioInfo m_info;
	};
};

#endif
