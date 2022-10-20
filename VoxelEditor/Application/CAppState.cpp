//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CAppState.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CAppState.h"
#include "CAppData.h"
#include <Application/CInput.h>

namespace App
{
	CAppState::CAppState()
	{
	}

	CAppState::~CAppState()
	{
	}
	
	void CAppState::Reset()
	{
		m_bPlaying = false;
	}

	bool CAppState::UpdatePlayMode(bool bPlaying)
	{
		if(m_bPlaying != bPlaying)
		{
			if(bPlaying)
			{
				CInput::Instance().SwitchLayout(INPUT_HASH_LAYOUT_PLAYER);
			}
			else
			{
				CInput::Instance().SwitchLayout(INPUT_HASH_LAYOUT_EDITOR);
			}

			m_bPlaying = bPlaying;
		}

		return m_bPlaying;
	}
};
