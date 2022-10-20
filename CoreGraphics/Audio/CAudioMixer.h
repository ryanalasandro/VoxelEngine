//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Audio/CAudioMixer.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CAUDIOMIXER_H
#define CAUDIOMIXER_H

namespace Audio
{
	class CAudioMixer
	{
	public:
		CAudioMixer();
		~CAudioMixer();
		CAudioMixer(const CAudioMixer&) = delete;
		CAudioMixer(CAudioMixer&&) = delete;
		CAudioMixer& operator = (const CAudioMixer&) = delete;
		CAudioMixer& operator = (CAudioMixer&&) = delete;

		void Intialize();
		void Release();

		// Accessors.
		inline class CAudio* Audio() const { return m_pAudio; }

	private:
		class CAudio* m_pAudio;
	};
};

#endif
