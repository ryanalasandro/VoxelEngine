//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Application/CCompManager.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CCOREMANAGER_H
#define CCOREMANAGER_H

#include "../Logic/CTransform.h"
#include "../Logic/CCamera.h"

namespace App
{
	class CCoreManager
	{
	public:
		static CCoreManager& Instance()
		{
			static CCoreManager instance;
			return instance;
		}

	private:
		CCoreManager();
		~CCoreManager();
		CCoreManager(const CCoreManager&) = delete;
		CCoreManager(CCoreManager&&) = delete;
		CCoreManager& operator = (const CCoreManager&) = delete;
		CCoreManager& operator = (CCoreManager&&) = delete;
		
	public:
		void Update();

		inline void QueueTransformUpdate(Logic::CTransform* pTransform) { m_transformQueue.Push(pTransform, pTransform->GetLayer()); }

	private:
		Logic::CTransformQueue m_transformQueue;
	};
};

#endif
