//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Utilities/CConvertUtil.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CCONVERTUTIL_H
#define CCONVERTUTIL_H

#include "../Globals/CGlobals.h"
#include "../Math/CMathVectorInt2.h"
#include <string>
#include <unordered_map>
#include <functional>

namespace Util
{
	//-----------------------------------------------------------------------------------------------
	// String
	//-----------------------------------------------------------------------------------------------

	inline u32 SplitString(const std::string& str, char splitter, std::function<void(u32, const std::string&)> func)
	{
		size_t start = 0;
		while(start < str.length() && str[start] <= 0x20)
		{
			++start;
		}

		u32 index = 0;
		for(size_t r = start, l = start; r <= str.length(); ++r)
		{
			if(r == str.length() || str[r] == splitter)
			{
				size_t len = r - l; // Trim right.
				while(len && str[l + len - 1] <= 0x20)
				{
					--len;
				}

				if(len)
				{ // Call.
					func(index, str.substr(l, len));
				}

				l = ++r; // Trim left.
				while(l < str.length() && str[l] <= 0x20)
				{
					l = r = l + 1;
				}

				++index;
			}
		}

		return index;
	}

	inline bool StringToBool(const std::string& str)
	{
		static const std::unordered_map<std::string, bool> MAP = {
			{ "true", true },
			{ "True", true },
			{ "TRUE", true },
			{ "false", false },
			{ "False", false },
			{ "FALSE", false },
		};

		return MAP.find(str)->second;
	}

	inline Math::VectorInt2 StringToVectorInt2(const std::string& str)
	{
		Math::VectorInt2 res;

		char* pEnd = const_cast<char*>(str.c_str());
		for(size_t i = 0; i < 2; ++i)
		{
			res[i] = strtol(pEnd, &pEnd, 10);
			while(pEnd && *pEnd != L'\0' && !isdigit(*pEnd))
			{
				++pEnd;
			}
		}

		return res;
	}

	//-----------------------------------------------------------------------------------------------
	// WString
	//-----------------------------------------------------------------------------------------------

	inline u32 SplitWString(const std::wstring& str, char splitter, std::function<void(u32, const std::wstring&)> func)
	{
		size_t start = 0;
		while(start < str.length() && str[start] <= 0x20)
		{
			++start;
		}

		u32 index = 0;
		for(size_t r = start, l = start; r <= str.length(); ++r)
		{
			if(r == str.length() || str[r] == splitter)
			{
				size_t len = r - l; // Trim right.
				while(len && str[l + len - 1] <= 0x20)
				{
					--len;
				}

				if(len)
				{ // Call.
					func(index, str.substr(l, len));
				}

				l = ++r; // Trim left.
				while(l < str.length() && str[l] <= 0x20)
				{
					l = r = l + 1;
				}

				++index;
			}
		}

		return index;
	}

	inline bool WStringToBool(const std::wstring& str)
	{
		static const std::unordered_map<std::wstring, bool> MAP = {
			{ L"true", true },
			{ L"True", true },
			{ L"TRUE", true },
			{ L"false", false },
			{ L"False", false },
			{ L"FALSE", false },
		};

		return MAP.find(str)->second;
	}

	inline Math::VectorInt2 WStringToVectorInt2(const std::wstring& str)
	{
		Math::VectorInt2 res;

		wchar_t* pEnd = const_cast<wchar_t*>(str.c_str());
		for(size_t i = 0; i < 2; ++i)
		{
			res[i] = wcstol(pEnd, &pEnd, 10);
			while(pEnd && *pEnd != L'\0' && !isdigit(*pEnd))
			{
				++pEnd;
			}
		}

		return res;
	}
};

#endif
