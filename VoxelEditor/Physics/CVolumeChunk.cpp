//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Physics/CVolumeChunk.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CVolumeChunk.h"
#include "../Universe/CNodeChunk.h"
#include <Windows.h>
#include <string>

namespace Physics
{
	CVolumeChunk::CVolumeChunk(const CVObject* pObject) :
		CVolume(pObject),
		m_blockHalfSize(0.52f),
		m_blockOffset(0.0f)
	{
	}

	CVolumeChunk::~CVolumeChunk() { }
	
	void CVolumeChunk::UpdateBounds()
	{
		m_minExtents = m_maxExtents = 0.0f;
		const static Math::SIMDVector UNIT_VERTICES[] = {
			{ -1.0f, -1.0f, -1.0f },
			{  1.0f, -1.0f, -1.0f },
			{ -1.0f, -1.0f,  1.0f },
			{  1.0f, -1.0f,  1.0f },
			{ -1.0f,  1.0f, -1.0f },
			{  1.0f,  1.0f, -1.0f },
			{ -1.0f,  1.0f,  1.0f },
			{  1.0f,  1.0f,  1.0f }
		};

		for(int i = 0; i < 8; ++i)
		{
			Math::Vector3 vertex = *(Math::Vector3*)(GetRotation().Conjugate() * UNIT_VERTICES[i].PointwiseProduct(m_halfSize)).ToFloat();
			
			for(int i = 0; i < 3; ++i)
			{
				if(vertex[i] < m_minExtents[i])
				{
					m_minExtents[i] = vertex[i];
				}

				if(vertex[i] > m_maxExtents[i])
				{
					m_maxExtents[i] = vertex[i];
				}
			}
		}
	}

	//-----------------------------------------------------------------------------------------------
	// Solvers methods.
	//-----------------------------------------------------------------------------------------------
		
	bool CVolumeChunk::MotionSolver(CVolume* pOther)
	{
		bool bAdjusted = false;

		Math::Vector3 center = *(Math::Vector3*)GetPosition().ToFloat();
		Math::Vector3 origin = *reinterpret_cast<const Math::Vector3*>(pOther->GetSolverPosition().ToFloat());
		Math::Vector3 dir = *reinterpret_cast<const Math::Vector3*>(pOther->GetSolverVelocity().ToFloat());
		Math::Vector3 mn = center - m_halfSize;
		Math::Vector3 mx = center + m_halfSize;
		RaycastInfo info { };

		struct HitInfo
		{
			RaycastInfo info;
			Math::Vector3 pt;
		};

		std::vector<HitInfo> infoList;
		float dialation = 1.0f;

		info.distance = dir.Length();
		dir /= info.distance;
		info.distance += 2.0f;
		if(OctreeRayTest(center, origin, dir, mn, mx, -pOther->GetMaxExtents() - dialation, -pOther->GetMinExtents() + dialation, info, false,
			[](float a, float b){ return true; },
			[&infoList](const RaycastInfo& i, const Math::Vector3& pt){ 
				/*if(infoList.size() && infoList[0].info.distance > i.distance)
				{
					infoList.clear(); 
				}*/
				
				infoList.push_back({ i, pt });
			}))
		{
			for(size_t i = 0; i < infoList.size(); ++i)
			{
				m_blockOffset = infoList[i].pt;
				bAdjusted |= CVolume::MotionSolver(pOther);
			}
		}

		return bAdjusted;
	}

	bool CVolumeChunk::IdleSolver(CVolume* pOther)
	{
		bool bAdjusted = false;

		Math::Vector3 center = *(Math::Vector3*)GetPosition().ToFloat();
		Math::Vector3 origin = *reinterpret_cast<const Math::Vector3*>(pOther->GetSolverPosition().ToFloat());
		Math::Vector3 mn = center - m_halfSize;
		Math::Vector3 mx = center + m_halfSize;

		struct HitInfo
		{
			u32 index;
			Math::Vector3 pt;
		};

		std::vector<HitInfo> infoList;
		float dialation = 2.0f;//GetSkinDepth() + pOther->GetSkinDepth();

		if(OctreeIntersectionTest(center, origin, mn, mx, -pOther->GetMaxExtents() - dialation, -pOther->GetMinExtents() + dialation,
			[&infoList](u32 i, const Math::Vector3& pt) {
				infoList.push_back({ i, pt });
			}))
		{
			for(size_t i = 0; i < infoList.size(); ++i)
			{
				m_blockOffset = infoList[i].pt;
				bAdjusted |= CVolume::IdleSolver(pOther);
			}
		}

		return bAdjusted;
	}
	
	//-----------------------------------------------------------------------------------------------
	// Utility methods.
	//-----------------------------------------------------------------------------------------------
		
	void CVolumeChunk::SetData(const Data& data) {
		m_data = data;
		m_halfSize = Math::Vector3(float(m_data.pChunk->GetWidth()), float(m_data.pChunk->GetHeight()), float(m_data.pChunk->GetLength())) * 0.5f;
	}

	bool CVolumeChunk::RayTest(const QueryRay& query, RaycastInfo& info) const
	{
		Math::Vector3 center = *(Math::Vector3*)GetPosition().ToFloat();

		Math::Vector3 origin = *reinterpret_cast<const Math::Vector3*>(query.ray.GetOrigin().ToFloat());
		Math::Vector3 dir = *reinterpret_cast<const Math::Vector3*>(query.ray.GetDirection().ToFloat());
		Math::Vector3 mn = center - m_halfSize;
		Math::Vector3 mx = center + m_halfSize;
		info.distance = query.ray.GetDistance();
		
		if(!OctreeRayTest(center, origin, dir, mn, mx, 0.0f, 0.0f, info, true, [](float a, float b){ return a < b; }, nullptr))
		{
			// Test planes
			float tmin = 0.0f;
			float tmax = info.distance;

			// Test for ray intersection with the current AABB.
			for(u32 i = 0; i < 3; ++i)
			{
				float invD = 1.0f / dir[i];
				float t0 = (mn[i] - origin[i]) * invD;
				float t1 = (mx[i] - origin[i]) * invD;

				if(invD < 0.0f)
				{
					std::swap(t0, t1);
				}

				tmin = t0 > tmin ? t0 : tmin;
				tmax = t1 < tmax ? t1 : tmax;

				if(tmax <= tmin) { return false; }
			}

			// Find Block.
			Math::Vector3 hit = origin + dir * tmax - center;
			
			float largestCoord = 0.0f;
			Universe::SIDE side = Universe::SIDE::SIDE_TOP;
			for(u32 i = 0; i < 3; ++i)
			{
				if(-hit[i] > largestCoord) { largestCoord = -hit[i]; side = static_cast<Universe::SIDE>((i << 1) + 1); }
				if( hit[i] > largestCoord) { largestCoord =  hit[i]; side = static_cast<Universe::SIDE>((i << 1)); }
			}

			hit += m_halfSize - Universe::SIDE_NORMAL[side] * 0.5f;

			int i = static_cast<int>(floorf(hit.x));
			int j = static_cast<int>(floorf(hit.y));
			int k = static_cast<int>(floorf(hit.z));
			
			info.index = m_data.pChunk->GetIndexInt(i, j, k);
			info.distance = tmax;
			info.pVolume = this;
			info.normal = Universe::SIDE_NORMAL[side];

			return true;
		}
		else
		{
			return true;
		}
	}

	Math::SIMDVector CVolumeChunk::SupportPoint(const Math::SIMDVector& dir, const CVolume* pVolumeA, float inset) const
	{
		Math::SIMDVector position = GetSolverPosition() + m_blockOffset - pVolumeA->GetSolverPosition();

		Math::SIMDVector localDir = GetSolverRotation().Conjugate() * dir;
		Math::SIMDVector maxCorner(_mm_or_ps(_mm_and_ps(localDir.m_xmm, _mm_set_ps1(-0.0f)),
			_mm_setr_ps(m_blockHalfSize.x - inset, m_blockHalfSize.y - inset, m_blockHalfSize.z - inset, 0.0f)));
		return position + GetSolverRotation() * maxCorner;
	}

	//-----------------------------------------------------------------------------------------------
	// Utility methods.
	//-----------------------------------------------------------------------------------------------

	bool CVolumeChunk::OctreeRayTest(const Math::Vector3& center, const Math::Vector3& origin, const Math::Vector3& dir, 
		const Math::Vector3& mn, const Math::Vector3& mx, const Math::Vector3& mnOffset, const Math::Vector3& mxOffset,
		RaycastInfo& info, bool bGenerateNormal, std::function<bool(float, float)> comp, std::function<void(const RaycastInfo&, const Math::Vector3&)> onFound) const
	{
		const Math::Vector3 sz = mx - mn;
		bool bVoxel = (sz.x <= 1.1f && sz.y <= 1.1f && sz.z <= 1.1f);
		float tmin = 0.0f;
		float tmax = info.distance;

		// Test for ray intersection with the current AABB.
		for(u32 i = 0; i < 3; ++i)
		{
			float invD = 1.0f / dir[i];
			float t0 = (mn[i] + mnOffset[i] - origin[i]) * invD;
			float t1 = (mx[i] + mxOffset[i] - origin[i]) * invD;

			if(invD < 0.0f)
			{
				std::swap(t0, t1);
			}

			tmin = t0 > tmin ? t0 : tmin;
			tmax = t1 < tmax ? t1 : tmax;

			if(tmax <= tmin) { return false; }
		}

		bool bResult = false;
		bool bFlipped = tmin <= 0.0f;

		if(bFlipped) {
			tmin = tmax;
		}
		
		if(comp(tmin, info.distance))
		{
			// Collision found.
			if(bVoxel)
			{
				// Inside voxel.
				const Math::Vector3 localCoord = ((mn - center) + m_halfSize) + 0.5f;
				u32 i = static_cast<u32>(floorf(localCoord.x));
				u32 j = static_cast<u32>(floorf(localCoord.y));
				u32 k = static_cast<u32>(floorf(localCoord.z));

				const u32 index = m_data.pChunk->GetIndex(i, j, k);
				const Universe::CNodeChunk::Block block = m_data.pChunk->GetBlock(index);

				if(block.id != 0)
				{
					if(bGenerateNormal)
					{
						Math::Vector3 hit = origin + dir * tmin;
						Math::Vector3 offCenter = (hit - mn - 0.5f) * 2.0f;

						std::pair<float, Universe::SIDE> closestList[3] { };
						u32 closestIndex = 0;

						for(u32 i = 0; i < 3; ++i)
						{
							if(-offCenter[i] > 0.99f) { closestList[closestIndex++] = { offCenter[i], static_cast<Universe::SIDE>((i << 1)) }; }
							if( offCenter[i] > 0.99f) { closestList[closestIndex++] = { offCenter[i], static_cast<Universe::SIDE>((i << 1) + 1) }; }
						}

						if(closestIndex > 1)
						{
							if(fabsf(closestList[0].first) < fabsf(closestList[1].first)) std::swap(closestList[0], closestList[1]);

							if(closestIndex > 2)
							{
								if(fabsf(closestList[0].first) < fabsf(closestList[2].first)) std::swap(closestList[0], closestList[2]);
								if(fabsf(closestList[1].first) < fabsf(closestList[2].first)) std::swap(closestList[1], closestList[2]);
							}
						}

						int intIndex = m_data.pChunk->GetIndexInt(i, j, k);

						for(u32 i = 0; i < closestIndex; ++i)
						{
							if(block.sideFlag & (1 << closestList[i].second))
							{
								info.index = intIndex;
								info.distance = tmin;
								info.pVolume = this;
								info.normal = Universe::SIDE_NORMAL[closestList[i].second];
								if(onFound) onFound(info, mn + 0.5f);
								return true;
							}
						}
					}
					else
					{
						info.index = index;
						info.distance = tmin;
						info.pVolume = this;
						info.normal = Math::SIMD_VEC_ZERO;
						if(onFound) onFound(info, mn + 0.5f);
						return true;
					}
				}
			}
			else
			{
				// Inside octree cell.
				Math::Vector3 half = sz * 0.5f;
				
				u32 mnMask = 0xFF;

				if(half.x < 0.9) mnMask &= ~(2 | 8 | 32 | 128);
				if(half.y < 0.9) mnMask &= ~(4 | 8 | 64 | 128);
				if(half.z < 0.9) mnMask &= ~(16 | 32 | 64 | 128);

				half.x = half.x < 1.0f ? 1.0f : half.x;
				half.y = half.y < 1.0f ? 1.0f : half.y;
				half.z = half.z < 1.0f ? 1.0f : half.z;

				// Perhaps look for a union style solution in the future.
				const Math::Vector3 mnList[] = {
					{ mn.x, mn.y, mn.z },
					{ mn.x + half.x, mn.y, mn.z },
					{ mn.x, mn.y, mn.z + half.z },
					{ mn.x + half.x, mn.y, mn.z + half.z },

					{ mn.x, mn.y + half.y, mn.z },
					{ mn.x + half.x, mn.y + half.y, mn.z },
					{ mn.x, mn.y + half.y, mn.z + half.z },
					{ mn.x + half.x, mn.y + half.y, mn.z + half.z },
				};

				for(u32 i = 0; i < 8; ++i)
				{
					if(mnMask & (0x1 << i)) { bResult |= OctreeRayTest(center, origin, dir, mnList[i], mnList[i] + half, mnOffset, mxOffset, info, bGenerateNormal, comp, onFound); }
				}
			}
		}

		return bResult;
	}
	
	bool CVolumeChunk::OctreeIntersectionTest(const Math::Vector3& center, const Math::Vector3& origin, 
		const Math::Vector3& mn, const Math::Vector3& mx, const Math::Vector3& mnOffset, const Math::Vector3& mxOffset, 
		std::function<void(u32, const Math::Vector3&)> onFound) const
	{
		const Math::Vector3 sz = mx - mn;
		bool bVoxel = (sz.x <= 1.1f && sz.y <= 1.1f && sz.z <= 1.1f);

		for(int i = 0; i < 3; ++i)
		{
			if(origin[i] < mn[i] + mnOffset[i]) return false;
			if(origin[i] > mx[i] + mxOffset[i]) return false;
		}

		bool bResult = false;
		
		// Collision found.
		if(bVoxel)
		{
			// Inside voxel.
				
			const Math::Vector3 localCoord = ((mn - center) + m_halfSize) + 0.5f;
			u32 i = static_cast<u32>(floorf(localCoord.x));
			u32 j = static_cast<u32>(floorf(localCoord.y));
			u32 k = static_cast<u32>(floorf(localCoord.z));

			const u32 index = m_data.pChunk->GetIndex(i, j, k);
			const Universe::CNodeChunk::Block block = m_data.pChunk->GetBlock(index);

			if(block.id != 0)
			{
				if(onFound) onFound(index, mn + 0.5f);
				return true;
			}
		}
		else
		{
			// Inside octree cell.
			Math::Vector3 half = sz * 0.5f;
				
			u32 mnMask = 0xFF;

			if(half.x < 0.9) mnMask &= ~(2 | 8 | 32 | 128);
			if(half.y < 0.9) mnMask &= ~(4 | 8 | 64 | 128);
			if(half.z < 0.9) mnMask &= ~(16 | 32 | 64 | 128);

			half.x = half.x < 1.0f ? 1.0f : half.x;
			half.y = half.y < 1.0f ? 1.0f : half.y;
			half.z = half.z < 1.0f ? 1.0f : half.z;

			// Perhaps look for a union style solution in the future.
			const Math::Vector3 mnList[] = {
				{ mn.x, mn.y, mn.z },
				{ mn.x + half.x, mn.y, mn.z },
				{ mn.x, mn.y, mn.z + half.z },
				{ mn.x + half.x, mn.y, mn.z + half.z },

				{ mn.x, mn.y + half.y, mn.z },
				{ mn.x + half.x, mn.y + half.y, mn.z },
				{ mn.x, mn.y + half.y, mn.z + half.z },
				{ mn.x + half.x, mn.y + half.y, mn.z + half.z },
			};

			for(u32 i = 0; i < 8; ++i)
			{
				if(mnMask & (0x1 << i)) { bResult |= OctreeIntersectionTest(center, origin, mnList[i], mnList[i] + half, mnOffset, mxOffset, onFound); }
			}
		}

		return true;
	}
};
