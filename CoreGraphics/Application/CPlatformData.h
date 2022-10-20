//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Application/CPlatformData.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CPLATFORMDATA_H
#define CPLATFORMDATA_H

#include <Math/CMathRect.h>

namespace App
{
	enum CURSOR_STATE
	{
		CURSOR_STATE_DEFAULT,
		CURSOR_STATE_LOCKED,
		CURSOR_STATE_CONFINED,
	};

	struct MonitorData
	{
		Math::Rect bounds;
	};
};

#endif
