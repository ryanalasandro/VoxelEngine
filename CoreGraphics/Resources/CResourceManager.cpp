//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CResourceManager.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CResourceManager.h"
#include "../Application/CPlatform.h"
#include "../Factory/CFactory.h"
#include "../Audio/CAudioClip.h"
#include "../Graphics/CShaderTrie.h"
#include "../Graphics/CMaterial.h"
#include "../Utilities/CFileSystem.h"
#include <Math/CMathFNV.h>
#include <Utilities/CDebugError.h>
#include <Utilities/CMemoryFree.h>

namespace Resources
{
	static const std::unordered_map<std::string, RESOURCE_TYPE> RES_FILE_MAP {
		{ "SHADERS", RESOURCE_TYPE_SHADER },
		{ "MATERIALS", RESOURCE_TYPE_MATERIAL },
		{ "TEXTURES", RESOURCE_TYPE_TEXTURE },
		{ "AUDIO", RESOURCE_TYPE_AUDIO },
		{ "MODELS", RESOURCE_TYPE_MODEL },
		{ "PREFABS", RESOURCE_TYPE_PREFAB },
	};

	CManager::CManager() { }
	CManager::~CManager() { }

	void CManager::Initialize()
	{
		CResourceFile::Data resData { };
		resData.filename = m_data.filename;
#ifndef PRODUCTION_BUILD
		resData.filepath = m_data.filepath;
#else
		resData.filepath = m_data.prodPath;
#endif

		resData.addResource = std::bind(&CManager::AddResource, this, std::placeholders::_1, std::placeholders::_2);

		m_resFile.SetData(resData);
		m_resFile.Initialize();

		for(auto shaderTrie : m_shaderTrieMap)
		{
			shaderTrie.second->Setup();
		}

		for(auto material : m_materialMap)
		{
			material.second->ParseFile();
		}

		for(auto material : m_materialMap)
		{
			material.second->Initialize();
		}

		for(auto clip : m_audioMap)
		{
			clip.second->Initialize();
		}
	}

	void CManager::PostInitialize()
	{
		for(auto material : m_materialMap)
		{
			material.second->PostInitialize();
		}
	}

	void CManager::Release()
	{
		for(auto clip : m_audioMap)
		{
			SAFE_RELEASE_DELETE(clip.second);
		}

		for(auto material : m_materialMap)
		{
			SAFE_RELEASE_DELETE(material.second);
		}

		for(auto shaderTrie : m_shaderTrieMap)
		{
			SAFE_RELEASE_DELETE(shaderTrie.second);
		}
	}
	
	//-----------------------------------------------------------------------------------------------
	// Utility methods.
	//-----------------------------------------------------------------------------------------------

	void CManager::BuildProductionResources()
	{
#ifndef PRODUCTION_BUILD
		// Delete
		if(Util::CFileSystem::Instance().VerifyDirectory(m_data.prodPath))
		{
			ASSERT_R(Util::CFileSystem::Instance().DeleteDirectory(m_data.prodPath));
			ASSERT_R(Util::CFileSystem::Instance().NewDirectory(m_data.prodPath));
		}
		else
		{
			ASSERT_R(Util::CFileSystem::Instance().NewPath(m_data.prodPath));
		}

		{ // Even though WUI scripts are separate from the resource manager, this will still handling moving them over.
			CFactory::Instance().GetPlatform()->SaveProductionScripts();
		}

		{ // Copy resource file over to production.
			std::ofstream file(std::wstring(m_data.prodPath) + m_data.filename);
			ASSERT_R(file.is_open());

			size_t sz = m_resFile.GetCode().size();
			file.write(reinterpret_cast<const char*>(&sz), sizeof(sz));
			file.write(m_resFile.GetCode().c_str(), sizeof(char) * sz);

			file.close();
		}

		for(auto shaderTrie : m_shaderTrieMap)
		{
			shaderTrie.second->BuildCSH();
		}

		for(auto material : m_materialMap)
		{
			material.second->SaveProductionFile();
		}
#endif
	}
	
	//-----------------------------------------------------------------------------------------------
	// Accessors.
	//-----------------------------------------------------------------------------------------------

	void* CManager::GetResource(RESOURCE_TYPE type, const std::string& key) const
	{
		return GetResource(type, Math::FNV1a_64(key.c_str(), key.size()));
	}

	void* CManager::GetResource(RESOURCE_TYPE type, u64 key) const
	{
		switch(type)
		{
			case RESOURCE_TYPE_SHADER:
				return m_shaderTrieMap.find(key)->second;
			case RESOURCE_TYPE_MATERIAL:
				return m_materialMap.find(key)->second;
			case RESOURCE_TYPE_TEXTURE:
				break;
			case RESOURCE_TYPE_AUDIO:
				return m_audioMap.find(key)->second;
			case RESOURCE_TYPE_MODEL:
				break;
			case RESOURCE_TYPE_PREFAB:
				break;
		}

		return nullptr;
	}

	void CManager::AddResource(const std::string& category, const Util::CompilerTuple<2>& res)
	{
		auto elem = RES_FILE_MAP.find(category);
		ASSERT_R(elem != RES_FILE_MAP.end());

		switch(elem->second)
		{
			case RESOURCE_TYPE_SHADER:
			{
				Graphics::CShaderTrie* pShaderTrie = new Graphics::CShaderTrie();
				Graphics::CShaderTrie::Data data { };
				data.filename = m_data.filepath + std::wstring(res.GetElement(1).begin(), res.GetElement(1).end());
				pShaderTrie->SetData(data);
				m_shaderTrieMap.insert({ Math::FNV1a_64(res.GetElement(0).c_str(), res.GetElement(0).size()), pShaderTrie });

			} break;
			case RESOURCE_TYPE_MATERIAL:
			{
				Graphics::CMaterial* pMaterial = new Graphics::CMaterial();
				Graphics::CMaterial::Data data { };
				data.filename = m_data.filepath + std::wstring(res.GetElement(1).begin(), res.GetElement(1).end());
				pMaterial->SetData(data);
				m_materialMap.insert({ Math::FNV1a_64(res.GetElement(0).c_str(), res.GetElement(0).size()), pMaterial });

			}	break;
			case RESOURCE_TYPE_TEXTURE:
			{

			} break;
			case RESOURCE_TYPE_AUDIO:
			{
				Audio::CAudioClip* pAudioClip = new Audio::CAudioClip();
				Audio::CAudioClip::Data data { };
				data.filename = m_data.filepath + std::wstring(res.GetElement(1).begin(), res.GetElement(1).end());
				pAudioClip->SetData(data);
				m_audioMap.insert({ Math::FNV1a_64(res.GetElement(0).c_str(), res.GetElement(0).size()), pAudioClip });

			}	break;
			case RESOURCE_TYPE_MODEL:
				break;
			case RESOURCE_TYPE_PREFAB:
				break;
			default:
				break;
		}
	}
};
