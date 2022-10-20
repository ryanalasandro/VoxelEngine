//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CAppState.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CAPPSTATE_H
#define CAPPSTATE_H

#include <Globals/CGlobals.h>
#include <mutex>
#include <shared_mutex>

namespace App
{
	class CAppState
	{
	public:
		CAppState();
		~CAppState();
		CAppState(const CAppState&) = delete;
		CAppState(CAppState&&) = delete;
		CAppState& operator = (const CAppState&) = delete;
		CAppState& operator = (CAppState&&) = delete;

		void Reset();

		// Accessors.
		inline bool IsPlaying() const { return m_bPlaying; }

		// Modifiers.
		inline void SetPlayMode(bool bPlaying) { UpdatePlayMode(bPlaying); }
		inline bool TogglePlayMode() { return UpdatePlayMode(!m_bPlaying); }

	private:
		bool UpdatePlayMode(bool bPlaying);

	private:
		mutable std::shared_mutex m_mutex;

		Abool m_bPlaying;
	};
};

#endif
