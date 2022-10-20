//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Application/CCompManager.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CCoreManager.h"

namespace App
{
	CCoreManager::CCoreManager()
	{
	}

	CCoreManager::~CCoreManager()
	{
	}
	
	void CCoreManager::Update()
	{
		m_transformQueue.Update();
	}
};
