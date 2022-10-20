//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Utilities/CScriptObject.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CScriptObject.h"
#include "CDebugError.h"
#include <fstream>
#include <unordered_set>

namespace Util
{
	static const std::unordered_set<char> WORD_BREAK_SET = {
		'~',
		'!',
		'@',
		'#',
		'%',
		'^',
		'&',
		'*',
		'(',
		')',
		'-',
		'=',
		'+',
		'[',
		'{',
		']',
		'}',
		'|',
		':',
		';',
		'\'',
		'\"',
		',',
		'<',
		'.',
		'>',
		'/',
		'?',
	};

	static const CScriptObject::StateMap BASE_MAP = {
		{ "PropBlock", [](CScriptObject* pObj) { return CreatePropBlock(pObj); } },
	};

	static const CScriptObject::StateMap PROP_BLOCK_SYMBOL_MAP = {
		{ "{", [](CScriptObject* pObj) { return NextProp(pObj); } },
		{ ":", [](CScriptObject* pObj) { return NextVal(pObj); } },
		{ ";", [](CScriptObject* pObj) { return NextProp(pObj); } },
	};

	static const CScriptObject::StateMap PROP_BLOCK_BREAK_MAP = {
		{ "}", [](CScriptObject* pObj) { return ClosePropBlock(pObj); } },
	};

	static const CScriptObject::State STATE_BASE(&BASE_MAP, nullptr);
	static const CScriptObject::State STATE_PROP_NAME(&PROP_BLOCK_BREAK_MAP, [](CScriptObject* pObj, const std::string& word) { return SetVal(pObj, word); });
	static const CScriptObject::State STATE_PROP_SYMBOLS(&PROP_BLOCK_SYMBOL_MAP, nullptr);

	//-----------------------------------------------------------------------------------------------
	// CScriptObject
	//-----------------------------------------------------------------------------------------------

	CScriptObject::CScriptObject() : m_pState(&STATE_BASE) { }

	CScriptObject::~CScriptObject() { }

	void CScriptObject::Build()
	{
		std::ifstream file(m_data.filename.c_str());
		assert(file.is_open());

		m_code.clear();
		bool bInQuote;
		char lastCh;

#ifndef PRODUCTION_BUILD
		bInQuote = false;
		lastCh = '\0';
		bool bAwaitingPrintable = true;
		char ch = '\0';

		do
		{ // Remove comments.
			lastCh = ch;
			ch = file.get();

			COMMENT_TYPE commentType = COMMENT_TYPE_NONE;
			if(ch != -1 && (bInQuote || (commentType = m_detectComment.InComment(ch, m_word)) == COMMENT_TYPE_NONE))
			{
				if(bInQuote && lastCh != '\\' && ch == '\"') { bInQuote = false; }

				if(ch > 0x20)
				{
					bAwaitingPrintable = false;
				}

				if(!bAwaitingPrintable)
				{
					if(ch == '\n')
					{
						bAwaitingPrintable = true;
					}

					m_word.AddChar(ch);
				}

				if(!bInQuote && ch == '\"') { bInQuote = true; }
			}
			else
			{
				std::string str;
				m_word.Move(str);
				if(str.empty())
				{
					bAwaitingPrintable = true;
				}
				else
				{
					m_code += str;
				}
			}

		} while(ch != -1);
#else
		size_t fileSz;
		file.read(reinterpret_cast<char*>(&fileSz), sizeof(fileSz));
		m_code.resize(fileSz);
		file.read(m_code.data(), sizeof(char) * fileSz);
#endif

		file.close();

		// Parse code.
		bInQuote = false;
		lastCh = '\0';
		for(char ch : m_code)
		{
			if(!bInQuote && ch == '\"')
			{
				bInQuote = true;
			}
			else if(bInQuote && lastCh != '\\' && ch == '\"')
			{
				bInQuote = false;

			}
			else if(!bInQuote && (ch <= 0x20 || WORD_BREAK_SET.find(ch) != WORD_BREAK_SET.end()))
			{
				// End of word.
				std::string str = "";
				if(!m_word.Empty())
				{
					m_word.Move(str);
					ASSERT_R(m_pState->ProccessWord(this, str));
				}
				
				if(ch > 0x20)
				{ // Process symbol.
					str = ch;
					ASSERT_R(m_pState->ProccessWord(this, str));
				}

			}
			else if(ch > 0x20 || bInQuote)
			{
				m_word.AddChar(ch);
			}

			lastCh = ch;
		}
	}

	//-----------------------------------------------------------------------------------------------
	// PropBlock methods.
	//-----------------------------------------------------------------------------------------------

	bool CreatePropBlock(CScriptObject* pObj)
	{
		pObj->m_propBlockList.push_back(CScriptObject::PropBlock());
		NextProp(pObj);

		return true;
	}

	bool SetVal(CScriptObject* pObj, const std::string& word)
	{
		pObj->m_propBlockList.back().propList.back().back() = std::wstring(word.begin(), word.end());
		pObj->m_pState = &STATE_PROP_SYMBOLS;

		return true;
	}

	bool NextVal(CScriptObject* pObj)
	{
		pObj->m_propBlockList.back().propList.back().push_back(L"");
		pObj->m_pState = &STATE_PROP_NAME;

		return true;
	}

	bool NextProp(CScriptObject* pObj)
	{
		pObj->m_propBlockList.back().propList.push_back(std::vector<std::wstring>());
		NextVal(pObj);

		return true;
	}

	bool ClosePropBlock(CScriptObject* pObj)
	{
		pObj->m_pState = &STATE_BASE;
		return true;
	}
};
