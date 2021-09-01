#pragma once
#include "Macro.h"

SSS_BEGIN

enum KEY_CODE
{
	KEY_ESCAPE					= 0x01,
	KEY_1							= 0x02,
	KEY_2							= 0x03,
	KEY_3							= 0x04,
	KEY_4							= 0x05,
	KEY_5							= 0x06,
	KEY_6							= 0x07,
	KEY_7							= 0x08,
	KEY_8							= 0x09,
	KEY_9							= 0x0A,
	KEY_0							= 0x0B,
	KEY_MINUS					= 0x0C,    /* - on main keyboard */
	KEY_EQUALS					= 0x0D,
	KEY_BACKSPACE			= 0x0E,
	KEY_TAB						= 0x0F,
	KEY_Q							= 0x10,
	KEY_W							= 0x11,
	KEY_E							= 0x12,
	KEY_R							= 0x13,
	KEY_T							= 0x14,
	KEY_Y							= 0x15,
	KEY_U							= 0x16,
	KEY_I							= 0x17,
	KEY_O							= 0x18,
	KEY_P							= 0x19,
	KEY_LEFT_BRACKET		= 0x1A,
	KEY_RIGHT_BRACKET		= 0x1B,
	KEY_RETURN					= 0x1C,    /* Enter on main keyboard */
	KEY_LEFT_CONTROL		= 0x1D,
	KEY_A							= 0x1E,
	KEY_S							= 0x1F,
	KEY_D							= 0x20,
	KEY_F							= 0x21,
	KEY_G							= 0x22,
	KEY_H							= 0x23,
	KEY_J							= 0x24,
	KEY_K							= 0x25,
	KEY_L							= 0x26,
	KEY_SEMICOLON			= 0x27,
	KEY_APOSTROPHE			= 0x28,
	KEY_GRAVE					= 0x29,   /* accent grave */
	KEY_LEFT_SHIFT			= 0x2A,
	KEY_BACKSLASH			= 0x2B,
	KEY_Z							= 0x2C,
	KEY_X							= 0x2D,
	KEY_C							= 0x2E,
	KEY_V							= 0x2F,
	KEY_B							= 0x30,
	KEY_N							= 0x31,
	KEY_M							= 0x32,
	KEY_COMMA					= 0x33,
	KEY_PERIOD					= 0x34,   /* . on main keyboard */
	KEY_SLASH					= 0x35,   /* / on main keyboard */
	KEY_RIGHT_SHIFT			= 0x36,
	KEY_NUMPAD_MULTIPLY	= 0x37,   /* * on numeric keypad */
	KEY_LEFT_ALT				= 0x38,
	KEY_SPACE					= 0x39,
	KEY_CAPSLOCK				= 0x3A,
	KEY_F1							= 0x3B,
	KEY_F2							= 0x3C,
	KEY_F3							= 0x3D,
	KEY_F4							= 0x3E,
	KEY_F5							= 0x3F,
	KEY_F6							= 0x40,
	KEY_F7							= 0x41,
	KEY_F8							= 0x42,
	KEY_F9							= 0x43,
	KEY_F10						= 0x44,
	KEY_NUMLOCK				= 0x45,
	KEY_SCROLL_LOCK		= 0x46,
	KEY_NUMPAD_7				= 0x47,
	KEY_NUMPAD_8				= 0x48,
	KEY_NUMPAD_9				= 0x49,
	KEY_NUMPAD_MINUS		= 0x4A,   /* - on numeric keypad */
	KEY_NUMPAD_4				= 0x4B,
	KEY_NUMPAD_5				= 0x4C,
	KEY_NUMPAD_6				= 0x4D,
	KEY_NUMPAD_PLUS		= 0x4E,    /* + on numeric keypad */
	KEY_NUMPAD_1				= 0x4F ,
	KEY_NUMPAD_2				= 0x50,
	KEY_NUMPAD_3				= 0x51,
	KEY_NUMPAD_0				= 0x52,
	KEY_NUMPAD_PERIOD	= 0x53,    /* . on numeric keypad */
	KEY_OEM_102				= 0x56,    /* <> or \| on RT 102-key keyboard (Non-U.S.) */
	KEY_F11						= 0x57,
	KEY_F12						= 0x58,
	KEY_F13						= 0x64,    /*                     (NEC PC98) */
	KEY_F14						= 0x65,    /*                     (NEC PC98) */
	KEY_F15						= 0x66,    /*                     (NEC PC98) */
	KEY_KANA						= 0x70,    /* (Japanese keyboard)            */
	KEY_ABNT_C1				= 0x73,    /* /? on Brazilian keyboard */
	KEY_CONVERT				= 0x79,    /* (Japanese keyboard)            */
	KEY_NOCONVERT			= 0x7B,    /* (Japanese keyboard)            */
	KEY_YEN						= 0x7D,    /* (Japanese keyboard)            */
	KEY_ABNT_C2				= 0x7E,    /* Numpad . on Brazilian keyboard */
	KEY_NUMPAD_EQUALS	= 0x8D,    /* = on numeric keypad (NEC PC98) */
	KEY_PREVTRACK			= 0x90,    /* Previous Track (DIK_CIRCUMFLEX on Japanese keyboard) */
	KEY_AT							= 0x91,    /*                     (NEC PC98) */
	KEY_COLON					= 0x92,    /*                     (NEC PC98) */
	KEY_UNDERLINE			= 0x93,    /*                     (NEC PC98) */
	KEY_KANJI						= 0x94,    /* (Japanese keyboard)            */
	KEY_STOP						= 0x95,    /*                     (NEC PC98) */
	KEY_AX							= 0x96,    /*                     (Japan AX) */
	KEY_UNLABELED			= 0x97,    /*                        (J3100) */
	KEY_NEXTTRACK			= 0x99,    /* Next Track */
	KEY_NUMPADENTER		= 0x9C,    /* Enter on numeric keypad */
	KEY_RCONTROL				= 0x9D,
	KEY_MUTE						= 0xA0,    /* Mute */
	KEY_CALCULATOR			= 0xA1,    /* Calculator */
	KEY_PLAYPAUSE				= 0xA2,    /* Play / Pause */
	KEY_MEDIASTOP			= 0xA4,    /* Media Stop */
	KEY_VOLUMEDOWN		= 0xAE,    /* Volume - */
	KEY_VOLUMEUP				= 0xB0,    /* Volume + */
	KEY_WEBHOME				= 0xB2,    /* Web home */
	KEY_NUMPADCOMMA		= 0xB3,    /* , on numeric keypad (NEC PC98) */
	KEY_NUMPAD_SLASH		= 0xB5,    /* / on numeric keypad */
	KEY_SYSRQ					= 0xB7,
	KEY_RIGHT_ALT				= 0xB8,    /* right Alt */
	KEY_PAUSE					= 0xC5,    /* Pause */
	KEY_HOME						= 0xC7,    /* Home on arrow keypad */
	KEY_UP							= 0xC8,    /* UpArrow on arrow keypad */
	KEY_PAGE_UP				= 0xC9,    /* PgUp on arrow keypad */
	KEY_LEFT						= 0xCB,    /* LeftArrow on arrow keypad */
	KEY_RIGHT					= 0xCD,    /* RightArrow on arrow keypad */
	KEY_END						= 0xCF,    /* End on arrow keypad */
	KEY_DOWN					= 0xD0,    /* DownArrow on arrow keypad */
	KEY_PAGE_DOWN			= 0xD1,    /* PgDn on arrow keypad */
	KEY_INSERT					= 0xD2,    /* Insert on arrow keypad */
	KEY_DELETE					= 0xD3,    /* Delete on arrow keypad */
	KEY_LEFT_WINDOW		= 0xDB,    /* Left Windows key */
	KEY_RIGHT_WINDOW		= 0xDC,    /* Right Windows key */
	KEY_APPS						= 0xDD,    /* AppMenu key */
	KEY_POWER					= 0xDE,    /* System Power */
	KEY_SLEEP					= 0xDF,    /* System Sleep */
	KEY_WAKE						= 0xE3,    /* System Wake */
	KEY_WEBSEARCH			= 0xE5,    /* Web Search */
	KEY_WEBFAVORITES		= 0xE6,    /* Web Favorites */
	KEY_WEBREFRESH			= 0xE7,    /* Web Refresh */
	KEY_WEBSTOP				= 0xE8,    /* Web Stop */
	KEY_WEBFORWARD		= 0xE9,    /* Web Forward */
	KEY_WEBBACK				= 0xEA,    /* Web Back */
	KEY_MYCOMPUTER			= 0xEB,    /* My Computer */
	KEY_MAIL						= 0xEC,    /* Mail */
	KEY_MEDIASELECT			= 0xED,    /* Media Select */

	MOUSE_LEFT					= 0x0100, /*Left Mouse Button*/
	MOUSE_RIGHT				= 0x0101, /*Right Mouse Button*/
	MOUSE_WHEEL				= 0x0102, /*Wheel Button*/
	MOUSE_WHEEL_UP		= 0x0103,
	MOUSE_WHEEL_DOWN	= 0x0104
};

enum ITEM_TYPE
{
	IT_TRADE,
	IT_EXPENDABLE,
	IT_EVENT,
	IT_EQUIPMENT
};

enum ITEM_GRADE
{
	IG_NORMAL,
	IG_BRONZE,
	IG_SILVER,
	IG_GOLD,
	IG_PLATINUM,
	IG_DIAMOND,
	IG_LEGENDARY
};

enum ITEM_EQUIP_PART
{
	IEP_NONE,
	IEP_HAIR,
	IEP_UPPER,
	IEP_SHOULDER,
	IEP_LOWER,
	IEP_HELMET,
	IEP_GLOVES,
	IEP_BOOTS,
	IEP_BELT,
	IEP_WEAPON_LEFT,
	IEP_WEAPON_RIGHT
};

enum SPACE
{
	SPACE_NONE = 0x0000,
	SPACE_2D = 0x0001,
	SPACE_3D = 0x0002,
	SPACE_LOCAL = 0x0010,
	SPACE_GLOBAL = 0x0020,
	SPACE_WINDOW = 0x0040,
	SPACE_DEVICE = 0x0080
};

enum METADATA_FLAG
{
	MF_NONE = 0x0000,
	MF_ATLAS = 0x0020,
	MF_TRANSFORM,
};

enum ANIMATION_CONDITION
{
	AC_NONE = 0x0000,
	AC_2D	= 0x0001,
	AC_3D = 0x0002,
	AC_BOTH = 0x0003,
	AC_USE_ATLAS = 0x0010
};

enum DIRECTION
{
	DIR_CENTER = 0x0000,
	DIR_LEFT = 0x0001,
	DIR_TOP = 0x0002,
	DIR_RIGHT = 0x0004,
	DIR_BOTTOM = 0x0008,
	DIR_FORWARD = 0x0010,
	DIR_BACKWARD = 0x0020
};

// Animation Type
enum ANIMATION_TYPE
{
	AT_NONE,
	AT_ATLAS,
	AT_FRAME, 
	AT_CUSTOM,
	AT_END
};

// Animation Option
enum ANIMATION_OPTION
{
	AO_LOOP,
	AO_STOP_AT_END,
	AO_ONCE_RETURN,
	AO_ONCE_DESTROY
};

enum PARAMETER_CONDITION
{
	PC_DEFAULT,
	PC_GRATER,
	PC_GRATER_EQUAL,
	PC_LESS,
	PC_LESS_EQUAL,
	PC_EQUALS,
	PC_NOT_EQUAL
};

enum PARAMETER_TYPE
{
	PT_DEFAULT,
	PT_BOOL,
	PT_INT,
	PT_FLOAT,
	PT_TRIGGER
};



enum RENDER_STATE
{
	RS_BLEND,
	RS_RASTERIZER,
	RS_DEPTHSTENCIL,
	RS_END
};


enum DIMENSION_TYPE
{
	DT_2D = 0x02,
	DT_3D = 0x03
};

enum SHADER_TYPE
{
	ST_VERTEX,
	ST_PIXEL,
	ST_GEOMETRY,
	ST_END
};

enum AXIS
{
	AX_X,
	AX_Y,
	AX_Z,
	AX_XR,
	AX_YR,
	AX_ZR,
	AX_END
};

enum UI_TYPE
{
	UT_BUTTON,
	UT_TEXTURE,
};

// Collider Type
enum COLLIDER_TYPE
{
	CT_RECT,
	CT_CIRCLE,	
	CT_UI,
	CT_SPHERE,
	CT_BOX,
	CT_MESH,
	CT_TERRAIN
};


// Light Type
enum LIGHT_TYPE
{
	LT_DIRECTIONAL,
	LT_POINT,
	LT_SPOT,
	LT_END
};

enum CBUFFER_TRANSFER
{
	CBT_VERTEX = 0x1,
	CBT_PIXEL = 0x2,
	CBT_GEOMETRY = 0x4
};


enum RENDER_MODE
{
	RM_2D,
	RM_3D
};

enum TILE_TYPE
{
	TT_RC,
	TT_ISO
};

enum TILE_FLAG
{
	TF_NONE = -1,
	TF_NORMAL,
	TF_NOMOVE,
	TF_OBJECT,
	TF_DEBUG_PATH
};

enum Effect_FLAG
{
	VF_NONE = 0x00000000,
	VF_BURN = 0x00000001,
	VF_LIMLIGHT = 0x00000002,
	VF_PADDING1 = 0x00000004,
	VF_PADDING2 = 0x00000008,
	VF_PADDING3 = 0x00000010,
	VF_PADDING4 = 0x00000020,
	VF_PADDING5 = 0x00000040,
	VF_PADDING6 = 0x00000080,
	VF_PADDING7 = 0x00000100

//	uint32_t iFlag = 0;
//  iFlag ^= VF_LIMLIGHT;
//	iFlag |= VF_LIMLIGHT;
//	iFlag & VF_LIMLIGHT == true	
//}
};

SSS_END
