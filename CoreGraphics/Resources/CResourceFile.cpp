//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CResourceFile.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CResourceFile.h"
#include <Math/CMathFNV.h>
#include <Utilities/CDebugError.h>
#include <fstream>
#include <cassert>
#include <unordered_map>
#include <functional>

namespace Resources {
	static const std::unordered_map<std::string, std::function<void(CResourceFile*)>> MACRO_MAP = {
		{"include", [](CResourceFile* pResFile){ pResFile->MacroInclude(); }}
	};

	CResourceFile::CResourceFile() : 
		m_bInQuote(false),
		m_sectionType(SECTION_TYPE_ROOT) {
	}

	CResourceFile::~CResourceFile() { }
	
	void CResourceFile::Initialize() {
		ParseFile(m_data.filename);
	}

	bool CResourceFile::ParseFile(const wchar_t* filename) {
		const u32 hash = Math::FNV1a_32(filename);
		if(m_includeSet.find(hash) != m_includeSet.end()) { return false; }
		m_includeSet.insert(hash);

		std::ifstream input(m_data.filepath + filename);
		assert(input.is_open());
		
		m_code.clear();

#ifndef PRODUCTION_BUILD
		bool bAwaitingPrintable = true;

		char ch;
		while((ch = input.get()) != -1)
		{
			Util::COMMENT_TYPE commentType;
			if(!m_bInQuote && (commentType = m_detectComment.InComment(ch, m_word))) {
				if((commentType & Util::COMMENT_TYPE::COMMENT_TYPE_OPEN) && !bAwaitingPrintable)
				{
					m_code.pop_back();
					bAwaitingPrintable = true;
				}

				// At any point we're in a macro and a new line is found, the macro needs to be completed.
				if(ch == '\n') {
					if(m_sectionType == SECTION_TYPE_MACRO) {
						ProcessMacro(ch);
					}
				}
			} else {
				if(ch == '\n')
				{
					if(!bAwaitingPrintable)
					{
						m_code.push_back(ch);
					}

					bAwaitingPrintable = true;
				}
				else if(!bAwaitingPrintable || ch > 0x20)
				{
					m_code.push_back(ch);
					bAwaitingPrintable = false;
				}

				Parse(ch);
			}

		} Parse('\n'); // Parse an implicit new line to finalize any sections that are being processed.
#else
		size_t sz;
		input.read(reinterpret_cast<char*>(&sz), sizeof(sz));
		m_code.resize(sz);
		input.read(m_code.data(), sizeof(char) * sz);

		for(char ch : m_code)
		{
			Parse(ch);
		} Parse('\n'); // Parse an implicit new line to finalize any sections that are being processed.
#endif

		input.close();

		return true;
	}

	void CResourceFile::Parse(char ch) {
		switch(m_sectionType) {
			case SECTION_TYPE_ROOT: {
				ProcessRoot(ch);
			} break;
			case SECTION_TYPE_CATEGORY: {
				ProcessCategory(ch);
			} break;
			case SECTION_TYPE_RESOURCE: {
				ProcessResource(ch);
			} break;
			case SECTION_TYPE_MACRO: {
				ProcessMacro(ch);
			} break;
			default:
				break;
		}
	}

	//-----------------------------------------------------------------------------------------------
	// Section processors.
	//-----------------------------------------------------------------------------------------------
	
	// Method for processing the contents of root.
	bool CResourceFile::ProcessRoot(char ch) {
		if(ch == '{') { // Attempting to enter into the previously stated category.
			assert(!m_word.Empty());
			m_word.Move(m_category);
			m_sectionType = SECTION_TYPE_CATEGORY;
		}
		
		else if(ch > 0x20) {
			if(ch == '#') { // Attempting to enter into a macro.
				assert(m_word.Empty());
				m_sectionType = SECTION_TYPE_MACRO;
			}
			
			else { // If the category has whitespace between characters, it is simply ignored.
				m_word.AddChar(ch);
			}
		}

		return m_sectionType == SECTION_TYPE_ROOT;
	}

	// Method for processing the contents of a category.
	bool CResourceFile::ProcessCategory(char ch) {
		if(ch == '}') { // Exiting the category.
			m_category.clear();
			m_sectionType = SECTION_TYPE_ROOT;
		}
		
		else if(ch == '(') { // Entering into a resource block.
			m_sectionType = SECTION_TYPE_RESOURCE;
		}

		else { // Beyond the above characters, only whitespace is supported in this section.
			assert(ch <= 0x20);
		}

		return m_sectionType == SECTION_TYPE_CATEGORY;
	}

	// Method for processing a resource block.
	bool CResourceFile::ProcessResource(char ch) {
		if(ch == ')') {
			assert(!m_bInQuote);

			m_resource.SetTarget(m_word);
			ASSERT(m_resource.IsComplete());

			// Process resource.
			m_data.addResource(m_category, m_resource);

			m_resource.Clear();
			m_sectionType = SECTION_TYPE_CATEGORY;
		}

		else {
			if(ch == ',') {
				assert(!m_bInQuote);

				m_resource.SetTarget(m_word);
				ASSERT(m_resource.Continue());
			}
				
			else if(ch == '\"') {
				m_bInQuote = !m_bInQuote;
			}
				
			else if(ch > 0x20 || m_bInQuote) {
				m_word.AddChar(ch);
			}
		}
		
		return m_sectionType == SECTION_TYPE_RESOURCE;
	}

	// Method for processing a macro sequence.
	bool CResourceFile::ProcessMacro(char ch) {
		if(ch == '\n') {
			assert(!m_bInQuote);

			// Process contents of macro based on its keyword.
			m_macro.SetTarget(m_word);

			auto elem = MACRO_MAP.find(m_macro.GetElement(0));
			assert(elem != MACRO_MAP.end());
			elem->second(this);
		}

		else if(ch > 0x20) {
			if(ch == '\"') {
				m_bInQuote = !m_bInQuote;
			}

			m_word.AddChar(ch);
		}

		else {
			if(m_macro.GetElement(0).length() == 0) {
				m_macro.SetTarget(m_word);
			}

			if(m_macro.Continue()) {
				assert(m_macro.GetElement(0).length() > 0);
			}
		}
		
		return m_sectionType == SECTION_TYPE_MACRO;
	}

	//-----------------------------------------------------------------------------------------------
	// Macro methods.
	//-----------------------------------------------------------------------------------------------

	void CResourceFile::MacroInclude() {
		std::string include = "";
		bool bInQuote = false;

		for(int i = 0; i < m_macro.GetElement(1).length(); ++i) {
			if(m_macro.GetElement(1)[i] == '\"') {
				bInQuote = !bInQuote;
			} else if(bInQuote) {
				include += m_macro.GetElement(1)[i];
			}
		}

		m_macro.Clear();
		m_sectionType = SECTION_TYPE_ROOT;
		
		std::wstring wInclude(include.begin(), include.end());
		ParseFile(wInclude.c_str());
	}
};
