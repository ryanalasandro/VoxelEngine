//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Physics/CPhysicsData.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CPHYSICSDATA_H
#define CPHYSICSDATA_H

#include "../Math/CSIMDRay.h"
#include <functional>
#include <vector>

namespace Physics
{
	enum class ColliderType
	{
		None,
		Trigger,
		Collider,
	};

	struct ContactData
	{
		Math::SIMDVector hitPoint;
		Math::SIMDVector hitNormal;
		float interval;
		class CVolume* pVolume;
		int index;
	};

	struct RaycastInfo
	{
		int index;
		float distance;
		const class CVolume* pVolume;
		Math::SIMDVector normal;
	};

	struct QueryRay
	{
		Math::CSIMDRay ray;
		std::function<void(const std::vector<RaycastInfo>&)> callback;
	};
};

#endif
