//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Application/CInput.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CInput.h"
#include "CInputDeviceList.h"
#include "CInputLayout.h"
#include "../Factory/CFactory.h"
#include <Utilities/CMemoryFree.h>

namespace App
{
	CInput::CInput() :
		m_pDeviceList(nullptr),
		m_pLayout(nullptr)
	{
	}

	CInput::~CInput() { }

	void CInput::Initialize(class CPanel* pPanel)
	{
		m_pDeviceList = CFactory::Instance().CreateInputDeviceList();
		m_pDeviceList->Initialize(pPanel);
	}

	void CInput::Reset()
	{
		m_pDeviceList->Reset();
	}

	void CInput::Update()
	{
		if(m_pLayout) m_pLayout->Update();
	}

	void CInput::PostUpdate()
	{
		if(m_pLayout) m_pLayout->PostUpdate();
	}

	void CInput::Release()
	{
		m_layoutMap.clear();
		SAFE_RELEASE_DELETE(m_pDeviceList);
	}

	//
	// Layout methods.
	//

	void CInput::CreateLayout(u32 layoutHash, bool bActive)
	{
		CInputLayout* pLayout = new CInputLayout();
		pLayout->Initialize(layoutHash, m_pDeviceList);
		if(bActive) { m_pLayout = pLayout; }

		m_layoutMap.insert({ layoutHash, pLayout });
	}

	void CInput::SwitchLayout(u32 layoutHash)
	{
		m_pLayout = m_layoutMap.find(layoutHash)->second;
	}

	//
	// (De)resgistration methods.
	//

	void CInput::RegisterBinding(const u32* pLayoutHashList, u32 layoutHashCount, const InputBindingSet& bindingSet)
	{
		for(u32 i = 0; i < layoutHashCount; ++i)
		{
			m_layoutMap.find(pLayoutHashList[i])->second->Register(bindingSet);
		}
	}
};
