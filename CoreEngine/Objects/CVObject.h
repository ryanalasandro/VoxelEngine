//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Objects/CVObject.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CVOBJECT_H
#define CVOBJECT_H

#include "CVComponent.h"
#include "../Globals/CGlobals.h"
#include "../Math/CMathFNV.h"
#include <vector>
#ifdef _DEBUG
#include <unordered_set>
#include <cassert>
#endif

namespace Logic
{
	class CTransform;
};

namespace App
{
	class CPanel;
};

namespace Physics
{
	class CVolume;
};
;

class CVObject
{
public:
	friend CVComponent;

protected:
	CVObject(const wchar_t* pName, u32 sceneHash);
	virtual ~CVObject();
	CVObject(const CVObject&) = delete;
	CVObject(CVObject&&) = delete;
	CVObject& operator = (const CVObject&) = delete;
	CVObject& operator = (CVObject&&) = delete;

protected:
	virtual void Initialize() { }
	virtual void PostInitialize() { }
	virtual void Update() { }
	virtual void LateUpdate() { }
	virtual void Release() { }

	virtual void OnMove(const App::CPanel* pPanel) { }
	virtual void OnResize(const App::CPanel* pPanel) { }

public:
	template<typename T>
	T* FindComponent() const
	{
		for(CVComponent* pComponent : m_componentList)
		{
			T* pT = dynamic_cast<T*>(pComponent);
			if(pT != nullptr) {
				return pT;
			}
		}

		return nullptr;
	}

	// Accessors.
	inline u64 GetHash() const { return m_hash; }
	inline bool HasPhysics() const { return m_bHasPhysics; }

	inline Logic::CTransform* GetTransform() const { return m_pTransform; }

private:
	void AddComponent(CVComponent* pComponent);
	void RemoveComponent(CVComponent* pComponent);
	
	// Modifiers.
	inline void OnPhysicsRegistered() { m_bHasPhysics = true; }
	inline void OnPhysicsDeregistered() { m_bHasPhysics = false; }

private:
	u64 m_hash;
	u64 m_sceneHash;

	bool m_bHasPhysics;

	Logic::CTransform* m_pTransform;
	std::vector<CVComponent*> m_componentList;
#ifdef _DEBUG
	std::unordered_set<u64> m_idSet;
#endif
};

#endif
