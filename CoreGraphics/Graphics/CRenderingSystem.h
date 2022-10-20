//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CRenderingSystem.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CRENDERINGSYSTEM_H
#define CRENDERINGSYSTEM_H

#include <Objects/CVObject.h>
#include <Logic/CTransform.h>
#include <unordered_map>
#include <vector>
#include <functional>

namespace UI
{
	class CUICanvas;
};

namespace Graphics
{
	class CRenderingSystem
	{
	public:
		CRenderingSystem();
		~CRenderingSystem();
		CRenderingSystem(const CRenderingSystem&) = delete;
		CRenderingSystem(CRenderingSystem&&) = delete;
		CRenderingSystem& operator = (const CRenderingSystem&) = delete;
		CRenderingSystem& operator = (CRenderingSystem&&) = delete;

		void PostInitialize();
		void Render();

		// (De)registration methods.
		void Register(class CRenderer* pRenderer);
		void Deregister(class CRenderer* pRenderer);

		void Register(UI::CUICanvas* pCanvas);
		void Deregister(UI::CUICanvas* pCanvas);

	private:
		template<typename T>
		void Register(T* pVal, std::vector<T*>& list, std::function<bool(T*, T*)> comp)
		{
			list.push_back(pVal);

			if(comp)
			{
				// Bubble the newly pushed mesh render to the proper position.
				for(s64 i = static_cast<s64>(list.size()) - 1; i > 0; --i)
				{
					if(comp(list[i - 1], list[i]))
					{
						// Swap the two adjacent values.
						T* tmp = list[i - 1];
						list[i - 1] = list[i];
						list[i] = tmp;
					}
					else
					{ // Pushed value at proper spot.
						break;
					}
				}
			}
		}

		template<typename T>
		void Deregister(T* pVal, std::vector<T*>& list)
		{
			size_t i, j;
			for(i = 0, j = 0; i < list.size(); ++i)
			{
				if(list[i] == pVal)
				{
					continue;
				}
				else
				{
					list[j++] = list[i];
				}
			}

			list.resize(j);
		}

	private:
		template<typename T>
		void SortBackToFront(const Math::SIMDVector& camPos, std::vector<T*>& list)
		{
			std::sort(list.begin(), list.end(), [&camPos](T* a, T* b){
				const float d0 = a->GetObject()->GetTransform() ? _mm_cvtss_f32((a->GetObject()->GetTransform()->GetPosition() - camPos).LengthSq()) : FLT_MAX;
				const float d1 = b->GetObject()->GetTransform() ? _mm_cvtss_f32((b->GetObject()->GetTransform()->GetPosition() - camPos).LengthSq()) : FLT_MAX;

				return d0 > d1;
			});
		}

	private:
		std::vector<class CRenderer*> m_depthlessList;
		std::vector<class CRenderer*> m_rendererList;
		std::vector<UI::CUICanvas*> m_canvasList;
	};
};

#endif
