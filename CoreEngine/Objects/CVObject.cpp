//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Objects/CVObject.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CVObject.h"
#include "../Logic/CTransform.h"

CVObject::CVObject(const wchar_t* pName, u32 sceneHash) : 
	m_hash(Math::FNV1a_64(pName)),
	m_sceneHash(sceneHash),
	m_bHasPhysics(false),
	m_pTransform(nullptr)
{
#ifdef _DEBUG
	assert(m_idSet.insert(m_hash).second);
#endif
}

CVObject::~CVObject() { }

void CVObject::AddComponent(CVComponent* pComponent) 
{
	Logic::CTransform* pTransform = dynamic_cast<Logic::CTransform*>(pComponent);
	if(pTransform) { m_pTransform = pTransform; }

	m_componentList.push_back(pComponent);
}

void CVObject::RemoveComponent(CVComponent* pComponent) 
{
	size_t index = 0;
	for(; index < m_componentList.size(); ++index)
	{
		if(m_componentList[index] == pComponent) 
		{
			break;
		}
	}

	if(index < m_componentList.size())
	{
		m_componentList[index] = m_componentList.back();
		m_componentList.pop_back();
	}

	Logic::CTransform* pTransform = dynamic_cast<Logic::CTransform*>(pComponent);
	if(pTransform == m_pTransform) { m_pTransform = nullptr; }
}