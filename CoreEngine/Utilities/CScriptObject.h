//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Utilities/CScriptObject.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CSCRIPTOBJECT_H
#define CSCRIPTOBJECT_H

#include "CDetectComment.h"
#include <string>
#include <unordered_map>
#include <functional>
#include <vector>

namespace Util
{
	class CScriptObject
	{
	public:
		typedef std::function<bool(CScriptObject*)> StateFunc;
		typedef std::function<bool(CScriptObject*, const std::string&)> StateNullFunc;
		typedef std::unordered_map<std::string, StateFunc> StateMap;

		// State class for scriptable object.
		class State
		{
		public:
			State(const StateMap* pMap, const StateNullFunc null) :
				m_pMap(pMap),
				m_null(null) {
			}

			bool ProccessWord(CScriptObject* pObj, const std::string& word) const
			{
				if(m_pMap != nullptr)
				{
					auto elem = m_pMap->find(word);
					if(elem != m_pMap->end())
					{
						return elem->second(pObj);
					}
				}

				if(m_null != nullptr)
				{
					return m_null(pObj, word);
				}

				return false;
			}

		private:
			const StateMap* m_pMap;
			const StateNullFunc m_null;
		};

		// Data containers.
		enum class BlockType
		{
			None,
			PropBlock,
		};

		struct PropBlock
		{
			std::vector<std::vector<std::wstring>> propList;
		};

	public:
		struct Data
		{
			std::wstring filename;
		};

	public:
		CScriptObject();
		~CScriptObject();
		CScriptObject(const CScriptObject&) = delete;
		CScriptObject(CScriptObject&&) = delete;
		CScriptObject& operator = (const CScriptObject&) = delete;
		CScriptObject& operator = (CScriptObject&&) = delete;

		void Build();

		// Accessors.
		inline const std::vector<PropBlock>& GetPropBlockList() const { return m_propBlockList; }
		inline const std::string& GetCode() const { return m_code; }

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }

	private:
		friend bool CreatePropBlock(CScriptObject* pObj);
		friend bool SetVal(CScriptObject* pObj, const std::string& word);
		friend bool NextVal(CScriptObject* pObj);
		friend bool NextProp(CScriptObject* pObj);
		friend bool ClosePropBlock(CScriptObject* pObj);

	private:
		Data m_data;
		std::string m_code;

		CompilerWord m_word;
		CDetectComment m_detectComment;

		BlockType m_blockType;
		std::vector<PropBlock> m_propBlockList;

		const State* m_pState;
	};
};

#endif
