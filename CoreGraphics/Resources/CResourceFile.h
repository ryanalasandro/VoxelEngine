//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CResourceFile.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CRESOURCEFILE_H
#define CRESOURCEFILE_H

#include <Globals/CGlobals.h>
#include <Utilities/CDSTrie.h>
#include <Utilities/CDetectComment.h>
#include <Utilities/CCompilerUtil.h>
#include <unordered_set>
#include <string>
#include <functional>

namespace Resources
{
	class CResourceFile
	{
	private:
		enum SECTION_TYPE
		{
			SECTION_TYPE_ROOT,
			SECTION_TYPE_CATEGORY,
			SECTION_TYPE_RESOURCE,
			SECTION_TYPE_MACRO,
		};

	public:
		struct Data
		{
			const wchar_t* filename;
			std::wstring filepath;
			std::function<void(const std::string&, const Util::CompilerTuple<2>&)> addResource;
		};

	public:
		CResourceFile();
		~CResourceFile();
		CResourceFile(const CResourceFile&) = delete;
		CResourceFile(CResourceFile&&) = delete;
		CResourceFile& operator = (const CResourceFile&) = delete;
		CResourceFile& operator = (CResourceFile&&) = delete;

		void Initialize();
		
		// Accessors.
		inline const std::string& GetCode() const { return m_code; }

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }

	private:
		bool ParseFile(const wchar_t* filename);
		void Parse(char ch);

		bool ProcessRoot(char ch);
		bool ProcessCategory(char ch);
		bool ProcessResource(char ch);
		bool ProcessMacro(char ch);

	public:
		void MacroInclude();

	private:
		bool m_bInQuote;
		SECTION_TYPE m_sectionType;

		Data m_data;
		std::string m_code;

		Util::CDetectComment m_detectComment;

		Util::CompilerWord m_word;
		std::string m_category;
		Util::CompilerTuple<2> m_resource;
		Util::CompilerTuple<2> m_macro;
		std::unordered_set<u32> m_includeSet;
	};
};

#endif
