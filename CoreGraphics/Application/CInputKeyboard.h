//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Application/CInputKeyboard.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CINPUTKEYBOARD_H
#define CINPUTKEYBOARD_H

#include "CInputDevice.h"
#include "CInputData.h"
#include <unordered_map>
#include <bitset>

namespace App
{
	enum VK_KEYBOARD
	{
		VK_KB_LBUTTON,
		VK_KB_RBUTTON,
		VK_KB_MBUTTON,
		VK_KB_XBUTTON1,
		VK_KB_XBUTTON2,
		VK_KB_CANCEL,
		VK_KB_BACK,
		VK_KB_TAB,
		VK_KB_CLEAR,
		VK_KB_RETURN,
		VK_KB_SHIFT,
		VK_KB_CONTROL,
		VK_KB_MENU,
		VK_KB_PAUSE,
		VK_KB_CAPITAL,
		VK_KB_KANA,
		VK_KB_JUNJA,
		VK_KB_KANJI,
		VK_KB_FINAL,
		VK_KB_IME_ON,
		VK_KB_IME_OFF,
		VK_KB_ESCAPE,
		VK_KB_CONVERT,
		VK_KB_NONCONVERT,
		VK_KB_ACCEPT,
		VK_KB_MODECHANGE,
		VK_KB_SPACE,
		VK_KB_PRIOR,
		VK_KB_NEXT,
		VK_KB_END,
		VK_KB_HOME,
		VK_KB_LEFT,
		VK_KB_UP,
		VK_KB_RIGHT,
		VK_KB_DOWN,
		VK_KB_SELECT,
		VK_KB_PRINT,
		VK_KB_EXECUTE,
		VK_KB_SNAPSHOT,
		VK_KB_INSERT,
		VK_KB_DELETE,
		VK_KB_HELP,
		VK_KB_0,
		VK_KB_1,
		VK_KB_2,
		VK_KB_3,
		VK_KB_4,
		VK_KB_5,
		VK_KB_6,
		VK_KB_7,
		VK_KB_8,
		VK_KB_9,
		VK_KB_A,
		VK_KB_B,
		VK_KB_C,
		VK_KB_D,
		VK_KB_E,
		VK_KB_F,
		VK_KB_G,
		VK_KB_H,
		VK_KB_I,
		VK_KB_J,
		VK_KB_K,
		VK_KB_L,
		VK_KB_M,
		VK_KB_N,
		VK_KB_O,
		VK_KB_P,
		VK_KB_Q,
		VK_KB_R,
		VK_KB_S,
		VK_KB_T,
		VK_KB_U,
		VK_KB_V,
		VK_KB_W,
		VK_KB_X,
		VK_KB_Y,
		VK_KB_Z,
		VK_KB_LWIN,
		VK_KB_RWIN,
		VK_KB_APPS,
		VK_KB_SLEEP,
		VK_KB_NUMPAD0,
		VK_KB_NUMPAD1,
		VK_KB_NUMPAD2,
		VK_KB_NUMPAD3,
		VK_KB_NUMPAD4,
		VK_KB_NUMPAD5,
		VK_KB_NUMPAD6,
		VK_KB_NUMPAD7,
		VK_KB_NUMPAD8,
		VK_KB_NUMPAD9,
		VK_KB_MULTIPLY,
		VK_KB_ADD,
		VK_KB_SEPARATOR,
		VK_KB_SUBTRACT,
		VK_KB_DECIMAL,
		VK_KB_DIVIDE,
		VK_KB_F1,
		VK_KB_F2,
		VK_KB_F3,
		VK_KB_F4,
		VK_KB_F5,
		VK_KB_F6,
		VK_KB_F7,
		VK_KB_F8,
		VK_KB_F9,
		VK_KB_F10,
		VK_KB_F11,
		VK_KB_F12,
		VK_KB_F13,
		VK_KB_F14,
		VK_KB_F15,
		VK_KB_F16,
		VK_KB_F17,
		VK_KB_F18,
		VK_KB_F19,
		VK_KB_F20,
		VK_KB_F21,
		VK_KB_F22,
		VK_KB_F23,
		VK_KB_F24,
		VK_KB_NUMLOCK,
		VK_KB_SCROLL,
		VK_KB_LSHIFT,
		VK_KB_RSHIFT,
		VK_KB_LCONTROL,
		VK_KB_RCONTROL,
		VK_KB_LMENU,
		VK_KB_RMENU,
		VK_KB_BROWSER_BACK,
		VK_KB_BROWSER_FORWARD,
		VK_KB_BROWSER_REFRESH,
		VK_KB_BROWSER_STOP,
		VK_KB_BROWSER_SEARCH,
		VK_KB_BROWSER_FAVORITES,
		VK_KB_BROWSER_HOME,
		VK_KB_VOLUME_MUTE,
		VK_KB_VOLUME_DOWN,
		VK_KB_VOLUME_UP,
		VK_KB_MEDIA_NEXT_TRACK,
		VK_KB_MEDIA_PREV_TRACK,
		VK_KB_MEDIA_STOP,
		VK_KB_MEDIA_PLAY_PAUSE,
		VK_KB_LAUNCH_MAIL,
		VK_KB_LAUNCH_MEDIA_SELECT,
		VK_KB_LAUNCH_APP1,
		VK_KB_LAUNCH_APP2,
		VK_KB_OEM_PLUS,
		VK_KB_OEM_COMMA,
		VK_KB_OEM_MINUS,
		VK_KB_OEM_PERIOD,
		VK_KB_OEM_CLEAR,
		VK_KB_OEM_1,
		VK_KB_OEM_2,
		VK_KB_OEM_3,
		VK_KB_OEM_4,
		VK_KB_OEM_5,
		VK_KB_OEM_6,
		VK_KB_OEM_7,
		VK_KB_OEM_8,
		VK_KB_OEM_102,
		VK_KB_PROCESSKEY,
		VK_KB_PACKET,
		VK_KB_ATTN,
		VK_KB_CRSEL,
		VK_KB_EXSEL,
		VK_KB_EREOF,
		VK_KB_PLAY,
		VK_KB_ZOOM,
		VK_KB_NONAME,
		VK_KB_PA1,
	};

	class CInputKeyboard : public CInputDevice
	{
	public:
		CInputKeyboard();
		~CInputKeyboard();
		CInputKeyboard(const CInputKeyboard&) = delete;
		CInputKeyboard(CInputKeyboard&&) = delete;
		CInputKeyboard& operator = (const CInputKeyboard&) = delete;
		CInputKeyboard& operator = (CInputKeyboard&&) = delete;

		void Initialize() final;
		void Reset() final;
		void Update() final;
		void PostUpdate() final;
		void Release() final;

		void RegisterBinding(u32 layoutHash, u32 bindingHash, const InputBinding& binding) final;
		void DeregisterBinding(u32 layoutHash, u32 bindingHash, const InputBinding& binding) final;

		void SetKey(VK_KEYBOARD code, bool bPressed);

	private:
		bool m_bAllowModifiers;
		INPUT_MODIFIER_FLAG m_modifierFlag;

		std::bitset<176> m_keySet;

		std::unordered_map<u64, std::unordered_map<u32, InputBinding>> m_bindingMap[INPUT_MODIFIER_TYPE_COUNT];
	};
};

#endif
