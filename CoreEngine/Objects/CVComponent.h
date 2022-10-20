//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Objects/CVComponent.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CVCOMPONENT_H
#define CVCOMPONENT_H

#include "../Globals/CGlobals.h"

class CVObject;

class CVComponent
{
protected:
	CVComponent(const CVObject* pObject);
	virtual ~CVComponent();
	
	CVComponent(const CVComponent&) = default;
	CVComponent(CVComponent&&) = default;
	CVComponent& operator = (const CVComponent&) = default;
	CVComponent& operator = (CVComponent&&) = default;

public:
	virtual void operator ()() { }

protected:
	virtual void Initialize() { }
	virtual void Reset() { }
	virtual bool TryUpdate(u8 frame) { Update(); return true; }
	virtual void Update() { }
	virtual void LateUpdate() { }
	virtual void Release() { }

	void AddComponent();
	void RemoveComponent();
	
	void OnPhysicsRegistered();
	void OnPhysicsDeregistered();

protected:
	const CVObject* m_pObject;
};

#endif
