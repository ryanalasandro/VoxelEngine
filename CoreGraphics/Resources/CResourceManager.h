//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CResourceManager.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CRESOURCEMANAGER_H
#define CRESOURCEMANAGER_H

#include "CResourceFile.h"
#include <Utilities/CCompilerUtil.h>
#include <unordered_map>

namespace Audio
{
	class CAudioClip;
};

namespace Graphics
{
	class CShaderTrie;
	class CMaterial;
};

namespace Resources
{
	static const wchar_t* META_DATA_EXTENSION = L".meta";

	enum RESOURCE_TYPE
	{
		RESOURCE_TYPE_SHADER,
		RESOURCE_TYPE_MATERIAL,
		RESOURCE_TYPE_TEXTURE,
		RESOURCE_TYPE_AUDIO,
		RESOURCE_TYPE_MODEL,
		RESOURCE_TYPE_PREFAB,
		RESOURCE_TYPE_COUNT,
	};

	class CManager
	{
	public:
		struct Data
		{
			const wchar_t* filepath;
			const wchar_t* prodPath;
			const wchar_t* filename;
		};

	private:
		CManager();
		~CManager();
		CManager(const CManager&) = delete;
		CManager(CManager&&) = delete;
		CManager& operator = (const CManager&) = delete;
		CManager& operator = (CManager&&) = delete;

	public:
		static CManager& Instance()
		{
			static CManager instance;
			return instance;
		}

		void Initialize();
		void PostInitialize();
		void Release();

		void BuildProductionResources();

		void* GetResource(RESOURCE_TYPE type, const std::string& key) const;
		void* GetResource(RESOURCE_TYPE type, u64 key) const;

		// Accessors.
		const wchar_t* GetFilePath() const { return m_data.filepath; }
		const wchar_t* GetProductionPath() const { return m_data.prodPath; }

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }

	private:
		void AddResource(const std::string& category, const Util::CompilerTuple<2>& res);

	private:
		Data m_data;
		CResourceFile m_resFile;

		std::unordered_map<u64, Graphics::CShaderTrie*> m_shaderTrieMap;
		std::unordered_map<u64, Graphics::CMaterial*> m_materialMap;
		std::unordered_map<u64, Audio::CAudioClip*> m_audioMap;
	};
};

#endif
