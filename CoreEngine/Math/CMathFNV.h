//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Math/CMathFNV.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CMATHFNV_H
#define CMATHFNV_H

#include "../Globals/CGlobals.h"
#include <cstring>
#include <cwchar>

namespace Math
{
	//-----------------------------------------------------------------------------------------------
	// 32-bit hash
	//-----------------------------------------------------------------------------------------------

	// ASCII.
	inline u32 FNV1a_32(const char* str, size_t sz)
	{
		u32 hash = 0x811c9dc5;
		for(size_t i = 0; i < sz; ++i)
		{
			hash = hash ^ str[i];
			hash *= 0x1000193;
		} return hash;
	}

	inline u32 FNV1a_32(const char* str)
	{
		return FNV1a_32(str, std::strlen(str));
	}

	// Unicode.
	inline u32 FNV1a_32(const wchar_t* str, size_t sz)
	{
		u32 hash = 0x811c9dc5;
		for(size_t i = 0; i < sz; ++i)
		{
			hash = hash ^ str[i];
			hash *= 0x1000193;
		} return hash;
	}

	inline u32 FNV1a_32(const wchar_t* str)
	{
		return FNV1a_32(str, std::wcslen(str));
	}

	//-----------------------------------------------------------------------------------------------
	// 64-bit hash
	//-----------------------------------------------------------------------------------------------

	// ASCII.
	inline u64 FNV1a_64(const char* str, size_t sz)
	{
		u64 hash = 0xcbf29ce484222325;
		for(size_t i = 0; i < sz; ++i)
		{
			hash = hash ^ str[i];
			hash *= 0x100000001B3;
		} return hash;
	}

	inline u64 FNV1a_64(const char* str)
	{
		return FNV1a_64(str, std::strlen(str));
	}

	// Unicode.
	inline u64 FNV1a_64(const wchar_t* str, size_t sz)
	{
		u64 hash = 0xcbf29ce484222325;
		for(size_t i = 0; i < sz; ++i)
		{
			hash = hash ^ str[i];
			hash *= 0x100000001B3;
		} return hash;
	}

	inline u64 FNV1a_64(const wchar_t* str)
	{
		return FNV1a_64(str, std::wcslen(str));
	}
};

#endif
