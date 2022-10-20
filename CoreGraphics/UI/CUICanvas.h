//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: UI/CUICanvas.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CUICANVAS_H
#define CUICANVAS_H

#include "../Graphics/CMeshData.h"
#include <Logic/CCamera.h>
#include <Objects/CVComponent.h>
#include <unordered_map>

namespace Graphics
{
	class CRenderingSystem;
	class CMaterial;
};

namespace UI
{
	enum class CanvasStyle
	{
		Screen,
		World
	};

	class CUICanvas : public CVComponent
	{
	public:
		friend Graphics::CRenderingSystem;

	public:
		struct Data
		{
			CanvasStyle style;
			class CUITransform* pTransform;
		};

	public:
		CUICanvas(const CVObject* pObject);
		~CUICanvas();
		CUICanvas(const CUICanvas&) = delete;
		CUICanvas(CUICanvas&&) = delete;
		CUICanvas& operator = (const CUICanvas&) = delete;
		CUICanvas& operator = (CUICanvas&&) = delete;

		void Initialize();
		void Update();
		void Release();

		// Accessors.
		inline const CVObject* GetObject() const { return m_pObject; }
		inline bool IsActive() const { return m_bActive; }

		// Modifiers.
		inline void SetActive(bool bActive) { m_bActive = bActive; }
		inline void SetData(const Data& data) { m_data = data; }

	private:
		void Render();

		void GatherRenderers(class CUITransform* pUITransform);

	private:
		bool m_bActive;
		Data m_data;

		std::unordered_map<u64, u32> m_instanceCounts;
	};
};

#endif
