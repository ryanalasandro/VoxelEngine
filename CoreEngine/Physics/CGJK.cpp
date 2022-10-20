//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Physics/CGJK.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CGJK.h"
#include "CVolume.h"
#include "CRigidbody.h"
#include <Windows.h>
#include <cassert>

namespace Physics
{
	thread_local Math::SIMDMatrix CGJK::m_simplex = { };
	thread_local Math::Vector4 CGJK::m_lambda = { };
	thread_local u32 CGJK::m_simplexSize = 0;

	thread_local const class CVolume* CGJK::m_pVolumeA = nullptr;
	thread_local const class CVolume* CGJK::m_pVolumeB = nullptr;

	static const int MAX_ITERATIONS = 256;

	bool CGJK::Intersection(const class CVolume* pVolumeA, const class CVolume* pVolumeB)
	{
		m_pVolumeA = pVolumeA;
		m_pVolumeB = pVolumeB;

		Math::SIMDVector v = Support(Math::SIMD_VEC_FORWARD);
		Math::SIMDVector w = Support(-v);
		ResetSimplex();
		
		int iterations = 0;
		float err = Math::g_EpsilonTol;
		while(_mm_cvtss_f32(_mm_sub_ps(v.Dot(v), v.Dot(w))) > err && iterations++ < MAX_ITERATIONS)
		{
			AddSimplex(w);

			// Calculate new error from simplex data.
			err = 0.0f;
			for(u32 i = 0; i < m_simplexSize; ++i)
			{
				const float len = _mm_cvtss_f32(m_simplex.vecs[i].LengthSq());
				if(len > err)
				{
					err = len;
				}
			}

			err *= Math::g_EpsilonTol;
			
			if(m_simplexSize == 0) { return false; }
			if(m_simplexSize == 4)
			{ // Intersection found!
				return true;
			}

			w = Support(-v);
		}
		
		return false;
	}

	Math::SIMDVector CGJK::Distance(const class CVolume* pVolumeA, const class CVolume* pVolumeB, bool bInset)
	{
		m_pVolumeA = pVolumeA;
		m_pVolumeB = pVolumeB;

		float ra = bInset ? pVolumeA->GetSkinDepth() : 0.0f;
		float rb = bInset ? pVolumeB->GetSkinDepth() : 0.0f;

		Math::SIMDVector v = Support(Math::SIMD_VEC_FORWARD, ra, rb);
		Math::SIMDVector w = Support(-v, ra, rb);
		ResetSimplex();
		
		int iterations = 0;
		float err = Math::g_EpsilonTol;
		while(_mm_cvtss_f32(_mm_sub_ps(v.Dot(v), v.Dot(w))) > err && iterations++ < MAX_ITERATIONS)
		{
			AddSimplex(w);

			// Calculate new error from simplex data.
			err = 0.0f;
			for(u32 i = 0; i < m_simplexSize; ++i)
			{
				const float len = _mm_cvtss_f32(m_simplex.vecs[i].LengthSq());
				if(len > err)
				{
					err = len;
				}
			}

			err *= Math::g_EpsilonTol;
			
			if(m_simplexSize == 0) { return 0.0f; }
			if(m_simplexSize == 4)
			{ // Intersection found!
				return 0.0f;
			}

			w = Support(-v, ra, rb);
		}
		
		return v;
	}

	bool CGJK::MovingContact(const class CVolume* pVolumeA, const class CVolume* pVolumeB, const Math::SIMDVector& rDir, Math::SIMDVector& contact, Math::SIMDVector& normal, float& t, Math::SIMDVector& sepAxis)
	{
		m_pVolumeA = pVolumeA;
		m_pVolumeB = pVolumeB;

		t = 0.0f;
		contact = 0.0f;
		normal = 0.0f;
		Math::SIMDVector v = sepAxis;

		Math::SIMDVector p;
		ResetSimplex();

		bool bFirst = true;
		bool bHit = true;
		int iterations = 0;
		
		float err = Math::g_EpsilonTol;
		while(bFirst || _mm_cvtss_f32(v.Dot(v)) > err && iterations++ < MAX_ITERATIONS)
		{
			p = Support(-v);

			const float vp = _mm_cvtss_f32(v.Dot(p));
			const float vr = _mm_cvtss_f32(v.Dot(rDir));
			if(vp > vr * t)
			{
				if(vr > 0.0f)
				{
					t = vp / vr;
					if(t > 1.0f)
					{
						bHit = false;
						break;
					}

					contact = rDir * t;
					ResetSimplex();
					normal = -v.Normalized();
				}
				else if(!bFirst)
				{
					bHit = false;
					break;
				}
			}

			bFirst = false;
			AddSimplex(p - contact);

			// Calculate new error from simplex data.
			err = 0.0f;
			for(u32 i = 0; i < m_simplexSize; ++i)
			{
				const float len = _mm_cvtss_f32(m_simplex.vecs[i].LengthSq());
				if(len > err)
				{
					err = len;
				}
			}

			err *= Math::g_EpsilonTol;

			m_simplexSize = TestSimplex(nullptr, &v);
		}

		if(!bHit)
		{
			sepAxis = v;
		}
		
		return bHit;
	}

	bool CGJK::RestingContact(const class CVolume* pVolumeA, const class CVolume* pVolumeB, Math::SIMDVector& contactA, Math::SIMDVector& contactB, Math::SIMDVector& normal)
	{
		m_pVolumeA = pVolumeA;
		m_pVolumeB = pVolumeB;
		
		const float ra = m_pVolumeA->GetSkinDepth();
		const float rb = m_pVolumeB->GetSkinDepth();

		Math::SIMDVector v = Support(Math::SIMD_VEC_FORWARD);
		Math::SIMDVector w = Support(-v);
		ResetSimplex();

		int iterations = 0;
		
		float err = Math::g_EpsilonTol;
		while(iterations++ < MAX_ITERATIONS)
		{
			float vv = _mm_cvtss_f32(v.Dot(v));
			float vw = _mm_cvtss_f32(v.Dot(w));
			if(vv - vw <= err) { break; }
			if(vw > 0.0f && (vw * vw) / vv > powf(ra + rb, 2.0f)) {
				return false;
			}

			AddSimplex(w);
			
			// Calculate new error from simplex data.
			err = 0.0f;
			for(u32 i = 0; i < m_simplexSize; ++i)
			{
				const float len = _mm_cvtss_f32(m_simplex.vecs[i].LengthSq());
				if(len > err)
				{
					err = len;
				}
			}

			err *= Math::g_EpsilonTol;
			
			m_simplexSize = TestSimplex(nullptr, &v);

			w = Support(-v);
		}
		
		if(_mm_cvtss_f32(v.Dot(v)) > err * err)
		{
			float len = _mm_cvtss_f32(v.Length());
			normal = -v / len;
			contactA = -normal * max(0.0f, ra - (len - rb));
			contactB = normal * max(0.0f, rb - (len - ra));
		}
		else
		{
			// Still can't generate contacts.
			//  For now we're using contracted volumes to generate penetration data.
			//   In the future it might be worth it to put something like EPA in this step for a more robust solution.
			//   Penetration depth should never really be that significant however, so this method might be better than EPA seeing as it's faster.
			v = Support(Math::SIMD_VEC_FORWARD, ra, rb);
			w = Support(-v, ra, rb);
			ResetSimplex();

			iterations = 0;
			err = Math::g_EpsilonTol;
			while(_mm_cvtss_f32(_mm_sub_ps(v.Dot(v), v.Dot(w))) > err && iterations++ < MAX_ITERATIONS)
			{
				AddSimplex(w);
			
				m_simplexSize = TestSimplex(nullptr, &v);

				if(m_simplexSize == 4)
				{ // Intersection found! NOT GOOD!
					return false;
				}

				w = Support(-v, ra, rb);
			}
			
			if(_mm_cvtss_f32(v.LengthSq()) < Math::g_EpsilonTol * Math::g_EpsilonTol)
			{
				return false;
			}

			float len = _mm_cvtss_f32(v.Length());
			normal = -v / len;
			contactA = -normal * max(0.0f, ra * 2.0f - (len - rb));
			contactB = normal * max(0.0f, rb * 2.0f - (len - ra));
		}

		return true;
	}

	//-----------------------------------------------------------------------------------------------
	// Simplex methods.
	//-----------------------------------------------------------------------------------------------
	
	int CGJK::TestSimplex(Math::SIMDVector* pDir, Math::SIMDVector* pPoint)
	{
		int indices = 0;
		if(m_simplexSize == 1) indices = SimplexPoint(0, pDir, pPoint);
		else if(m_simplexSize == 2) indices = SimplexLine(1, 0, pDir, pPoint);
		else if(m_simplexSize == 3) indices = SimplexTriangle(2, 1, 0, pDir, pPoint);
		else if(m_simplexSize == 4) indices = SimplexTetrahedron(3, 2, 1, 0, pDir, pPoint);

		return indices;
	}

	int CGJK::SimplexPoint(u32 a, Math::SIMDVector* pDir, Math::SIMDVector* pPoint)
	{
		if(pDir) *pDir = -m_simplex.vecs[a];
		if(pPoint) *pPoint = m_simplex.vecs[a];
		SetSimplex(m_simplex.vecs[a]);
		return 1;
	}

	int CGJK::SimplexLine(u32 a, u32 b, Math::SIMDVector* pDir, Math::SIMDVector* pPoint)
	{
		Math::SIMDVector AO = -m_simplex.vecs[a];
		Math::SIMDVector AB = m_simplex.vecs[b] - m_simplex.vecs[a];
		if(DotTest(AB, AO))
		{
			Math::SIMDVector BO = -m_simplex.vecs[b];
			Math::SIMDVector BA = m_simplex.vecs[a] - m_simplex.vecs[b];
			if(DotTest(BA, BO))
			{
				// On edge plane.
				if(pDir) *pDir = Math::SIMDVector::Cross(AB, Math::SIMDVector::Cross(AO, AB));
				if(pPoint) *pPoint = ClosestPoint(m_simplex.vecs[a], m_simplex.vecs[b]);
				SetSimplex(m_simplex.vecs[a], m_simplex.vecs[b]);
				return 2;
			}
			else
			{
				// Behind edge plane B.
				return 0;
			}
		}
		else
		{
			// Behind edge plane A.
			SimplexPoint(a, pDir, pPoint);
			return 1;
		}
	}

	int CGJK::SimplexTriangle(u32 a, u32 b, u32 c, Math::SIMDVector* pDir, Math::SIMDVector* pPoint)
	{
		const Math::SIMDVector AO = -m_simplex.vecs[a];
		const Math::SIMDVector AB = m_simplex.vecs[b] - m_simplex.vecs[a];
		const Math::SIMDVector AC = m_simplex.vecs[c] - m_simplex.vecs[a];
		const Math::SIMDVector ABC = Math::SIMDVector::Cross(AB, AC);

		if(DotTest(-ABC, AO))
		{ // AO is below the triangle.
			SetSimplex(m_simplex.vecs[a], m_simplex.vecs[c], m_simplex.vecs[b]);
			return SimplexTriangle(a, b, c, pDir, pPoint);
		}
		else
		{
			const Math::SIMDVector BO = -m_simplex.vecs[b];
			const Math::SIMDVector CO = -m_simplex.vecs[c];

			if(DotTest(Math::SIMDVector::Cross(ABC, AC), AO))
			{ // Check if AO is outside triangle from edge AC.
				if(DotTest(AC, AO))
				{
					if(DotTest(-AC, CO))
					{
						return SimplexLine(a, c, pDir, pPoint);
					}
					else
					{
						return 0;
					}
				}
				else
				{
					return SimplexLine(a, b, pDir, pPoint);
				}
			}
			else
			{
				if(DotTest(Math::SIMDVector::Cross(AB, ABC), AO))
				{
					if(DotTest(-AB, BO))
					{
						return SimplexLine(a, b, pDir, pPoint);
					}
					else
					{
						return 0;
						SimplexLine(b, c, pDir, pPoint);
					}
				}
				else
				{ // AO is within the triangle ABC.
					if(pDir) *pDir = ABC;
					if(pPoint) *pPoint = ClosestPoint(m_simplex.vecs[a], m_simplex.vecs[b], m_simplex.vecs[c]);
					SetSimplex(m_simplex.vecs[a], m_simplex.vecs[b], m_simplex.vecs[c]);
					return 3;
				}
			}
		}
	}

	int CGJK::SimplexTetrahedron(u32 a, u32 b, u32 c, u32 d, Math::SIMDVector* pDir, Math::SIMDVector* pPoint)
	{
		const Math::SIMDVector AO = -m_simplex.vecs[a];
		const Math::SIMDVector BO = -m_simplex.vecs[b];
		const Math::SIMDVector CO = -m_simplex.vecs[c];
		const Math::SIMDVector AB = m_simplex.vecs[b] - m_simplex.vecs[a];
		const Math::SIMDVector AC = m_simplex.vecs[c] - m_simplex.vecs[a];
		const Math::SIMDVector AD = m_simplex.vecs[d] - m_simplex.vecs[a];
		const Math::SIMDVector BC = m_simplex.vecs[c] - m_simplex.vecs[b];
		const Math::SIMDVector BD = m_simplex.vecs[d] - m_simplex.vecs[b];
		const Math::SIMDVector CD = m_simplex.vecs[d] - m_simplex.vecs[c];
		const Math::SIMDVector ABC = Math::SIMDVector::Cross(AC, AB);
		const Math::SIMDVector ADB = Math::SIMDVector::Cross(AB, AD);
		const Math::SIMDVector ACD = Math::SIMDVector::Cross(AD, AC);
		const Math::SIMDVector BDC = Math::SIMDVector::Cross(BC, BD);

		if(DotTest(-ACD, AO) && DotTest(Math::SIMDVector::Cross(ACD, AD), AO) && DotTest(Math::SIMDVector::Cross(AC, ACD), AO) && DotTest(Math::SIMDVector::Cross(CD, ACD), CO))
		{
			SetSimplex(m_simplex.vecs[a], m_simplex.vecs[c], m_simplex.vecs[d]);
		}
		else if(DotTest(-ADB, AO) && DotTest(Math::SIMDVector::Cross(ADB, AB), AO) && DotTest(Math::SIMDVector::Cross(ADB, BD), BO))
		{
			SetSimplex(m_simplex.vecs[a], m_simplex.vecs[d], m_simplex.vecs[b]);
		}
		else if(DotTest(-ABC, AO) && DotTest(Math::SIMDVector::Cross(BC, ABC), BO))
		{
			SetSimplex(m_simplex.vecs[a], m_simplex.vecs[b], m_simplex.vecs[c]);
		}
		else if(DotTest(-BDC, BO))
		{
			return 0;
		}
		else
		{
			if(pPoint) *pPoint = 0.0f;
			return 4;
		}
		
		return SimplexTriangle(2, 1, 0, pDir, pPoint);
	}
	
	//-----------------------------------------------------------------------------------------------
	// Faster simplex processing methods.
	//  Based on: OpenGJK
	//  Not used in the physics system currently however.
	//-----------------------------------------------------------------------------------------------

	inline static bool FastCompareSigns(float a, float b)
	{
		return (a > 0 && b > 0) || (a < 0 && b < 0);
	}

	inline static bool hff1(const Math::SIMDVector& p, const Math::SIMDVector& q) {
		return _mm_cvtss_f32(Math::SIMDVector::Dot(p, (p - q))) > 0;
	}

	inline static bool hff2(const Math::SIMDVector& p, const Math::SIMDVector& q, const Math::SIMDVector& r) {
		Math::SIMDVector ntmp;
		Math::SIMDVector n, pq, pr;
		float tmp = 0;

		pq = q - p;
		pr = r - p;
		
		ntmp = Math::SIMDVector::Cross(pq, pr);
		n = Math::SIMDVector::Cross(pq, ntmp);

		tmp = _mm_cvtss_f32(Math::SIMDVector::Dot(p, n));

		return tmp < 0;
	}

	inline static bool hff3(const Math::SIMDVector& p, const Math::SIMDVector& q, const Math::SIMDVector& r) {
		Math::SIMDVector n, pq, pr;
		float tmp = 0;

		pq = q - p;
		pr = r - p;

		n = Math::SIMDVector::Cross(pq, pr);

		tmp = _mm_cvtss_f32(Math::SIMDVector::Dot(p, n));

		return tmp <= 0;
	}

	int CGJK::FastSimplex(const Math::SIMDMatrix& simplexIn, Math::SIMDMatrix& simplexOut, Math::SIMDVector& pointOut)
	{
		simplexOut = Math::SIMD_MTX4X4_ZERO;

		int indices;
		if(m_simplexSize == 1) { pointOut = simplexOut.rows[0] = simplexIn.rows[0]; indices = 1; }
		else if(m_simplexSize == 2) { indices = FastSimplexLine(1, 0, simplexIn, simplexOut, pointOut); }
		else if(m_simplexSize == 3) { indices = FastSimplexTriangle(2, 1, 0, simplexIn, simplexOut, pointOut); }
		else { indices = FastSimplexTetrahedron(3, 2, 1, 0, simplexIn, simplexOut, pointOut); }

		return indices;
	}

	int CGJK::FastSimplexLine(u32 a, u32 b, const Math::SIMDMatrix& simplexIn, Math::SIMDMatrix& simplexOut, Math::SIMDVector& pointOut)
	{
		if (hff1(simplexIn.vecs[a], simplexIn.vecs[b])) {
			pointOut = ClosestPoint(simplexIn.vecs[a], simplexIn.vecs[b]);  // Update v, no need to update s
			simplexOut = simplexIn;
			return 2;                      // Return V{1,2}
		} else {
			pointOut = simplexOut.vecs[0] = simplexIn.vecs[a];
			return 1;        // Return V{1}
		}
	}
		
	int CGJK::FastSimplexTriangle(u32 a, u32 b, u32 c, const Math::SIMDMatrix& simplexIn, Math::SIMDMatrix& simplexOut, Math::SIMDVector& pointOut)
	{
		bool hff1f_s12 = hff1(simplexIn.vecs[a], simplexIn.vecs[b]);
		bool hff1f_s13 = hff1(simplexIn.vecs[a], simplexIn.vecs[c]);
		bool hff2f_23 = !hff2(simplexIn.vecs[a], simplexIn.vecs[b], simplexIn.vecs[c]);
		bool hff2f_32 = !hff2(simplexIn.vecs[a], simplexIn.vecs[c], simplexIn.vecs[b]);

		if(hff1f_s12) {
			if(hff2f_23) {
				if(hff1f_s13) {
					if(hff2f_32) {
						pointOut = ClosestPoint(simplexIn.vecs[a], simplexIn.vecs[b], simplexIn.vecs[c]);  // Update s, no need to update c
						simplexOut = simplexIn;
						return 3; // Return V{1,2,3}
					} else {
						//S2Dregion13();               // Update s
						pointOut = ClosestPoint(simplexIn.vecs[a], simplexIn.vecs[c]);  // Update v
						simplexOut = Math::SIMDMatrix(simplexIn.rows[c], simplexIn.rows[a], _mm_set1_ps(0.0f), _mm_set1_ps(0.0f)); // Update s
						return 2; // Return V{1,3}
					}
				} else {
					pointOut = ClosestPoint(simplexIn.vecs[a], simplexIn.vecs[b], simplexIn.vecs[c]);  // Update s, no need to update c
					simplexOut = simplexIn;
					return 3; // Return V{1,2,3}
				}
			} else {
				//S2Dregion12();               // Update s
				pointOut = ClosestPoint(simplexIn.vecs[a], simplexIn.vecs[b]);  // Update v
				simplexOut = Math::SIMDMatrix(simplexIn.rows[a], simplexIn.rows[b], _mm_set1_ps(0.0f), _mm_set1_ps(0.0f)); // Update s
				return 2; // Return V{1,2}
			}
		} else if (hff1f_s13) {
			if (hff2f_32) {
				pointOut = ClosestPoint(simplexIn.vecs[a], simplexIn.vecs[b], simplexIn.vecs[c]);  // Update s, no need to update v
				simplexOut = simplexIn;
				return 3; // Return V{1,2,3}
			} else {
				//S2Dregion13();               // Update s
				pointOut = ClosestPoint(simplexIn.vecs[a], simplexIn.vecs[c]);  // Update v
				simplexOut = Math::SIMDMatrix(simplexIn.rows[c], simplexIn.rows[a], _mm_set1_ps(0.0f), _mm_set1_ps(0.0f)); // Update s
				return 2; // Return V{1,3}
			}
		} else {
			//S2Dregion1();  // Update s and v
			pointOut = simplexIn.rows[a];
			simplexOut = Math::SIMDMatrix(simplexIn.rows[a], _mm_set1_ps(0.0f), _mm_set1_ps(0.0f), _mm_set1_ps(0.0f)); // Update s and v
			return 1; // Return V{1}
		}
	}
		
	int CGJK::FastSimplexTetrahedron(u32 a, u32 b, u32 c, u32 d, const Math::SIMDMatrix& simplexIn, Math::SIMDMatrix& simplexOut, Math::SIMDVector& pointOut)
	{
		Math::SIMDVector si, sj, sk;
		bool testLineThree, testLineFour, testPlaneTwo, testPlaneThree, testPlaneFour;
		int dotTotal;
		int i, j, k;

		const Math::SIMDVector& s1 = simplexIn.vecs[3];
		const Math::SIMDVector& s2 = simplexIn.vecs[2];
		const Math::SIMDVector& s3 = simplexIn.vecs[1];
		const Math::SIMDVector& s4 = simplexIn.vecs[0];

		Math::SIMDVector s1s2 = s2 - s1;
		Math::SIMDVector s1s3 = s3 - s1;
		Math::SIMDVector s1s4 = s4 - s1;

		bool hff1_tests[3];
		hff1_tests[2] = hff1(s1, s2);
		hff1_tests[1] = hff1(s1, s3);
		hff1_tests[0] = hff1(s1, s4);
		testLineThree = hff1(s1, s3);
		testLineFour = hff1(s1, s4);

		dotTotal = hff1(s1, s2) + testLineThree + testLineFour;
		if(dotTotal == 0) {
			pointOut = simplexIn.rows[0];
			simplexOut = Math::SIMDMatrix(simplexIn.rows[0], _mm_set1_ps(0.0f), _mm_set1_ps(0.0f), _mm_set1_ps(0.0f));
			return 1;
		}

		float det134 = _mm_cvtss_f32(s1s3.Dot(s1s4.Cross(s1s2)));//determinant(s1s3, s1s4, s1s2);
		int sss;
		if (det134 > 0) {
			sss = 0;
		} else {
			sss = 1;
		}

		auto select_1ik = [&](){
			simplexOut.vecs[2] = simplexIn.vecs[3];
			simplexOut.vecs[1] = si;
			simplexOut.vecs[0] = sk;
			return 3;
		};

		auto select_1ij = [&](){
			simplexOut.vecs[2] = simplexIn.vecs[3];
			simplexOut.vecs[1] = si;
			simplexOut.vecs[0] = sj;
			return 3;
		};

		auto select_1jk = [&](){
			simplexOut.vecs[2] = simplexIn.vecs[3];
			simplexOut.vecs[1] = sj;
			simplexOut.vecs[0] = sk;
			return 3;
		};

		auto select_1i = [&](){
			simplexOut.vecs[1] = simplexIn.vecs[3];
			simplexOut.vecs[0] = si;
			return 2;
		};

		auto select_1j = [&](){
			simplexOut.vecs[1] = simplexIn.vecs[3];
			simplexOut.vecs[0] = sj;
			return 2;
		};

		auto select_1k = [&](){
			simplexOut.vecs[1] = simplexIn.vecs[3];
			simplexOut.vecs[0] = sk;
			return 2;
		};

		testPlaneTwo = hff3(s1, s3, s4) - sss;
		testPlaneTwo = testPlaneTwo * testPlaneTwo;
		testPlaneThree = hff3(s1, s4, s2) - sss;
		testPlaneThree = testPlaneThree * testPlaneThree;
		testPlaneFour = hff3(s1, s2, s3) - sss;
		testPlaneFour = testPlaneFour * testPlaneFour;

		switch (testPlaneTwo + testPlaneThree + testPlaneFour) {
			case 3:
				pointOut = 0.0f;
				return 4;

			case 2: {
				// Only one facing the oring
				// 1,i,j, are the indices of the points on the triangle and remove k from
				// simplex
				
				Math::SIMDMatrix tmpMtx = simplexIn;

				//s->nvrtx = 3;
				if (!testPlaneTwo) {  // k = 2;   removes s2
					tmpMtx.vecs[2] = simplexIn.vecs[3];
				} else if (!testPlaneThree) {  // k = 1; // removes s3
					tmpMtx.vecs[1] = s2;
					tmpMtx.vecs[2] = simplexIn.vecs[3];
				} else if (!testPlaneFour) {  // k = 0; // removes s4  and no need to reorder
					tmpMtx.vecs[0] = s3;
					tmpMtx.vecs[1] = s2;
					tmpMtx.vecs[2] = simplexIn.vecs[3];
				}

				// Call S2D
				return FastSimplexTriangle(2, 1, 0, tmpMtx, simplexOut, pointOut);
			}

			case 1:
				// Two triangles face the origins:
				//    The only positive hff3 is for triangle 1,i,j, therefore k must be in
				//    the solution as it supports the the point of minimum norm.

				// 1,i,j, are the indices of the points on the triangle and remove k from
				// simplex
				//s->nvrtx = 3;
				if (testPlaneTwo) {
					k = 2;  // s2
					i = 1;
					j = 0;
				} else if (testPlaneThree) {
					k = 1;  // s3
					i = 0;
					j = 2;
				} else {
					k = 0;  // s4
					i = 2;
					j = 1;
				}

				si = simplexIn.vecs[i];
				sj = simplexIn.vecs[j];
				sk = simplexIn.vecs[k];

				if (dotTotal == 1) {
					if (hff1_tests[k]) {
						if (!hff2(s1, sk, si)) {
							pointOut = ClosestPoint(s1, si, sk);
							return select_1ik();
						} else if (!hff2(s1, sk, sj)) {
							pointOut = ClosestPoint(s1, sj, sk);
							return select_1jk();
						} else {
							pointOut = ClosestPoint(s1, sk);
							return select_1k();  // select region 1i
						}
					} else if (hff1_tests[i]) {
					if (!hff2(s1, si, sk)) {
							pointOut = ClosestPoint(s1, si, sk);
							return select_1ik();
						} else {
							pointOut = ClosestPoint(s1, si);
							return select_1i();  // select region 1i
						}
					} else {
						if (!hff2(s1, sj, sk)) {
							pointOut = ClosestPoint(s1, sj, sk);
							return select_1jk();
						} else {
							pointOut = ClosestPoint(s1, sj);
							return select_1j();  // select region 1i
						}
					}
				} else if (dotTotal == 2) {
					// Two edges have positive hff1, meaning that for two edges the origin's
					// project fall on the segement.
					//  Certainly the edge 1,k supports the the point of minimum norm, and so
					//  hff1_1k is positive

					if (hff1_tests[i]) {
						if (!hff2(s1, sk, si)) {
							if (!hff2(s1, si, sk)) {
								pointOut = ClosestPoint(s1, si, sk);
								return select_1ik();  // select region 1ik
							} else {
								pointOut = ClosestPoint(s1, sk);
								return select_1k();  // select region 1k
							}
						} else {
							if (!hff2(s1, sk, sj)) {
								pointOut = ClosestPoint(s1, sj, sk);
								return select_1jk();  // select region 1jk
							} else {
								pointOut = ClosestPoint(s1, sk);
								return select_1k();  // select region 1k
							}
						}
					} else if (hff1_tests[j]) {  //  there is no other choice
						if (!hff2(s1, sk, sj)) {
							if (!hff2(s1, sj, sk)) {
								pointOut = ClosestPoint(s1, sj, sk);
								return select_1jk();  // select region 1jk
							} else {
								pointOut = ClosestPoint(s1, sj);
								return select_1j();  // select region 1j
							}
						} else {
							if (!hff2(s1, sk, si)) {
								pointOut = ClosestPoint(s1, si, sk);
								return select_1ik();  // select region 1ik
							} else {
								pointOut = ClosestPoint(s1, sk);
								return select_1k();  // select region 1k
							}
						}
					} else {
						// ERROR;
						return 0;
					}

				} else if (dotTotal == 3) {
					// MM : ALL THIS HYPHOTESIS IS FALSE
					// sk is s.t. hff3 for sk < 0. So, sk must support the origin because
					// there are 2 triangles facing the origin.

					int hff2_ik = hff2(s1, si, sk);
					int hff2_jk = hff2(s1, sj, sk);
					int hff2_ki = hff2(s1, sk, si);
					int hff2_kj = hff2(s1, sk, sj);

					if (hff2_ki == 0 && hff2_kj == 0) {
						//mexPrintf("\n\n UNEXPECTED VALUES!!! \n\n");
						return 0;
					}

					if (hff2_ki == 1 && hff2_kj == 1) {
						pointOut = ClosestPoint(s1, sk);
						return select_1k();
					} else if (hff2_ki) {
						// discard i
						if (hff2_jk) {
							// discard k
							pointOut = ClosestPoint(s1, sj);
							return select_1j();
						} else {
							pointOut = ClosestPoint(s1, sk, sj);
							return select_1jk();
						}
					} else {
						// discard j
						if (hff2_ik) {
							// discard k
							pointOut = ClosestPoint(s1, si);
							return select_1i();
						} else {
							pointOut = ClosestPoint(s1, sk, si);
							return select_1ik();
						}
					}
				}
				break;

			case 0:
				// The origin is outside all 3 triangles
				if (dotTotal == 1) {
					// Here si is set such that hff(s1,si) > 0
					if (testLineThree) {
						k = 2;
						i = 1;  // s3
						j = 0;
					} else if (testLineFour) {
						k = 1;  // s3
						i = 0;
						j = 2;
					} else {
						k = 0;
						i = 2;  // s2
						j = 1;
					}

					si = simplexIn.vecs[i];
					sj = simplexIn.vecs[j];
					sk = simplexIn.vecs[k];

					if (!hff2(s1, si, sj)) {
						pointOut = ClosestPoint(s1, si, sj);
						return select_1ij();
					} else if (!hff2(s1, si, sk)) {
						pointOut = ClosestPoint(s1, si, sk);
						return select_1ik();
					} else {
						pointOut = ClosestPoint(s1, si);
						return select_1i();
					}
				} else if (dotTotal == 2) {
					// Here si is set such that hff(s1,si) < 0
					//s->nvrtx = 3;
					if (!testLineThree) {
						k = 2;
						i = 1;  // s3
						j = 0;
					} else if (!testLineFour) {
						k = 1;
						i = 0;  // s4
						j = 2;
					} else {
						k = 0;
						i = 2;  // s2
						j = 1;
					}

					si = simplexIn.vecs[i];
					sj = simplexIn.vecs[j];
					sk = simplexIn.vecs[k];

					if (!hff2(s1, sj, sk)) {
						if (!hff2(s1, sk, sj)) {
							pointOut = ClosestPoint(s1, sj, sk);
							return select_1jk();  // select region 1jk
						} else if (!hff2(s1, sk, si)) {
							pointOut = ClosestPoint(s1, sk, si);
							return select_1ik();
						} else {
							pointOut = ClosestPoint(s1, sk);
							return select_1k();
						}
					} else if (!hff2(s1, sj, si)) {
						pointOut = ClosestPoint(s1, si, sj);
						return select_1ij();
					} else {
						pointOut = ClosestPoint(s1, sj);
						return select_1j();
					}
				}
				break;
			default:
				return 0;
		}

		return 4;
	}
		
	//-----------------------------------------------------------------------------------------------
	// Utility methods.
	//-----------------------------------------------------------------------------------------------

	Math::SIMDVector CGJK::ClosestPoint(const Math::SIMDVector& A, const Math::SIMDVector& B)
	{
		Math::SIMDVector t = B - A;
		return A + t * _mm_cvtss_f32(Math::SIMDVector::Dot(-A, t)) / _mm_cvtss_f32(Math::SIMDVector::Dot(t, t));
	}

	Math::SIMDVector CGJK::ClosestPoint(const Math::SIMDVector& A, const Math::SIMDVector& B, const Math::SIMDVector& C)
	{
		Math::SIMDVector n = (B - A).Cross(C - A);
		return n * (_mm_cvtss_f32(Math::SIMDVector::Dot(A, n)) / _mm_cvtss_f32(Math::SIMDVector::Dot(n, n)));
	}

	//-----------------------------------------------------------------------------------------------
	// Utility methods.
	//-----------------------------------------------------------------------------------------------

	Math::SIMDVector CGJK::Support(const Math::SIMDVector& dir, float insetA, float insetB)
	{
		return m_pVolumeA->SupportPoint(dir, m_pVolumeB, insetA) - m_pVolumeB->SupportPoint(-dir, m_pVolumeB, insetB);
	}
};
