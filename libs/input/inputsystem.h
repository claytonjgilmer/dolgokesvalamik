#ifndef _inputsystem_h_
#define _inputsystem_h_

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include "utils/singleton.h"
#include "utils/assert.h"

namespace input
{
	class inputinitparams
	{
	public:
		HWND	m_Window;
	};
	class system
	{
		friend 	BOOL CALLBACK EnumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance,void* pContext );
		friend	BOOL CALLBACK EnumObjectsCallback( const DIDEVICEOBJECTINSTANCE* pdidoi,void* tmp );

		DECLARE_SINGLETON_DESC(system,inputinitparams)

	public:
		system(const inputinitparams* i_params);
		~system();
		void Init(const inputinitparams& i_Params);
		void Update();
		void Clear();

		int KeyDown(int i_Key)
		{
			utils::assertion(i_Key>=0 && i_Key<256);
			return (m_KeyboardState[i_Key]);
		}

		int KeyPressed(int i_Key)
		{
			utils::assertion(i_Key>=0 && i_Key<256);
			return m_KeyboardState[i_Key] && !m_PrevKeyboardState[i_Key];
		}

		int KeyReleased(int i_Key)
		{
			utils::assertion(i_Key>=0 && i_Key<256);
			return !m_KeyboardState[i_Key] && m_PrevKeyboardState[i_Key];
		}

		int GetMouseX()
		{
			return m_MouseState.lX;
		}
		int GetMouseY()
		{
			return m_MouseState.lY;
		}
		int GetMouseZ()
		{
			return m_MouseState.lZ;
		}
		int MouseButtonDown(int i_Btn)
		{
			utils::assertion (i_Btn>=0 && i_Btn<8);
			return (m_MouseState.rgbButtons[i_Btn]);
		}

		int MouseButtonPressed(int i_Btn)
		{
			utils::assertion (i_Btn>=0 && i_Btn<8);
			return (m_MouseState.rgbButtons[i_Btn] && !m_PrevMouseState.rgbButtons[i_Btn]);
		}

		int MouseButtonReleased(int i_Btn)
		{
			utils::assertion (i_Btn>=0 && i_Btn<8);
			return (!m_MouseState.rgbButtons[i_Btn] & m_PrevMouseState.rgbButtons[i_Btn]);
		}

		int JoyNum()
		{
			return m_JoystickNum;
		}

		int JoyButtonNum(int i_JoyIndex)
		{
			return m_JoyStruct[i_JoyIndex].m_ButtonNum;
		}

		int JoyAxisNum(int i_JoyIndex)
		{
			return m_JoyStruct[i_JoyIndex].m_AxisNum;
		}


		int JoyButtonDown(int i_JoyIndex, int i_Btn)
		{
			utils::assertion(i_JoyIndex>=0 && i_JoyIndex<4);
			utils::assertion(i_Btn>=0 && i_Btn<128);

			return m_JoyStruct[i_JoyIndex].m_JoyState.rgbButtons[i_Btn];
		}

		int JoyButtonPressed(int i_JoyIndex, int i_Btn)
		{
			utils::assertion(i_JoyIndex>=0 && i_JoyIndex<4);
			utils::assertion(i_Btn>=0 && i_Btn<128);

			return m_JoyStruct[i_JoyIndex].m_JoyState.rgbButtons[i_Btn] & (m_JoyStruct[i_JoyIndex].m_PrevJoyState.rgbButtons[i_Btn] ^ 1);
		}

		int JoyButtonReleased(int i_JoyIndex, int i_Btn)
		{
			utils::assertion(i_JoyIndex>=0 && i_JoyIndex<4);
			utils::assertion(i_Btn>=0 && i_Btn<128);

			return m_JoyStruct[i_JoyIndex].m_PrevJoyState.rgbButtons[i_Btn] & (m_JoyStruct[i_JoyIndex].m_JoyState.rgbButtons[i_Btn] ^ 1);
		}

/*
		int JoyAxis(int i_JoyIndex, int i_Axis)
		{
			return m_JoyStruct[i_JoyIndex].m_JoyState.
		}
*/

		bool Inited()
		{
			return m_Inited;
		}
	private:
		unsigned char	m_KeyboardState[256];
		unsigned char	m_PrevKeyboardState[256];
		DIMOUSESTATE2	m_MouseState;
		DIMOUSESTATE2	m_PrevMouseState;



		class JoyStruct
		{
		public:
			DIJOYSTATE2	m_JoyState;
			DIJOYSTATE2	m_PrevJoyState;
			int			m_AxisNum;
			int			m_ButtonNum;
			int			m_PovNum;
			int			m_ThingAndSomeThingNum;
			LPDIRECTINPUTDEVICE8	g_lpJoyDevice;

			JoyStruct()
			{
				ZeroMemory(&m_JoyState,sizeof(m_JoyState));
				ZeroMemory(&m_PrevJoyState,sizeof(m_JoyState));
				m_AxisNum=0;
				m_ButtonNum=0;
				m_PovNum=0;
				m_ThingAndSomeThingNum=0;
				g_lpJoyDevice=NULL;
			}
		};

		JoyStruct	m_JoyStruct[4];
		unsigned	m_JoystickNum;


		bool m_Inited;

		void			InitKeyboard(HWND i_Hwnd);
		void			InitMouse(HWND i_Hwnd);
		void			InitJoys(HWND i_Hwnd);

		void			UpdateKeyboard();
		void			UpdateMouse();
		void			UpdateJoys();
	};
}


#define KEYCODE_ESCAPE          0x01
#define KEYCODE_1               0x02
#define KEYCODE_2               0x03
#define KEYCODE_3               0x04
#define KEYCODE_4               0x05
#define KEYCODE_5               0x06
#define KEYCODE_6               0x07
#define KEYCODE_7               0x08
#define KEYCODE_8               0x09
#define KEYCODE_9               0x0A
#define KEYCODE_0               0x0B
#define KEYCODE_MINUS           0x0C    /* - on main keyboard */
#define KEYCODE_EQUALS          0x0D
#define KEYCODE_BACK            0x0E    /* backspace */
#define KEYCODE_TAB             0x0F
#define KEYCODE_Q               0x10
#define KEYCODE_W               0x11
#define KEYCODE_E               0x12
#define KEYCODE_R               0x13
#define KEYCODE_T               0x14
#define KEYCODE_Y               0x15
#define KEYCODE_U               0x16
#define KEYCODE_I               0x17
#define KEYCODE_O               0x18
#define KEYCODE_P               0x19
#define KEYCODE_LBRACKET        0x1A
#define KEYCODE_RBRACKET        0x1B
#define KEYCODE_RETURN          0x1C    /* Enter on main keyboard */
#define KEYCODE_LCONTROL        0x1D
#define KEYCODE_A               0x1E
#define KEYCODE_S               0x1F
#define KEYCODE_D               0x20
#define KEYCODE_F               0x21
#define KEYCODE_G               0x22
#define KEYCODE_H               0x23
#define KEYCODE_J               0x24
#define KEYCODE_K               0x25
#define KEYCODE_L               0x26
#define KEYCODE_SEMICOLON       0x27
#define KEYCODE_APOSTROPHE      0x28
#define KEYCODE_GRAVE           0x29    /* accent grave */
#define KEYCODE_LSHIFT          0x2A
#define KEYCODE_BACKSLASH       0x2B
#define KEYCODE_Z               0x2C
#define KEYCODE_X               0x2D
#define KEYCODE_C               0x2E
#define KEYCODE_V               0x2F
#define KEYCODE_B               0x30
#define KEYCODE_N               0x31
#define KEYCODE_M               0x32
#define KEYCODE_COMMA           0x33
#define KEYCODE_PERIOD          0x34    /* . on main keyboard */
#define KEYCODE_SLASH           0x35    /* / on main keyboard */
#define KEYCODE_RSHIFT          0x36
#define KEYCODE_MULTIPLY        0x37    /* * on numeric keypad */
#define KEYCODE_LMENU           0x38    /* left Alt */
#define KEYCODE_SPACE           0x39
#define KEYCODE_CAPITAL         0x3A
#define KEYCODE_F1              0x3B
#define KEYCODE_F2              0x3C
#define KEYCODE_F3              0x3D
#define KEYCODE_F4              0x3E
#define KEYCODE_F5              0x3F
#define KEYCODE_F6              0x40
#define KEYCODE_F7              0x41
#define KEYCODE_F8              0x42
#define KEYCODE_F9              0x43
#define KEYCODE_F10             0x44
#define KEYCODE_NUMLOCK         0x45
#define KEYCODE_SCROLL          0x46    /* Scroll Lock */
#define KEYCODE_NUMPAD7         0x47
#define KEYCODE_NUMPAD8         0x48
#define KEYCODE_NUMPAD9         0x49
#define KEYCODE_SUBTRACT        0x4A    /* - on numeric keypad */
#define KEYCODE_NUMPAD4         0x4B
#define KEYCODE_NUMPAD5         0x4C
#define KEYCODE_NUMPAD6         0x4D
#define KEYCODE_ADD             0x4E    /* + on numeric keypad */
#define KEYCODE_NUMPAD1         0x4F
#define KEYCODE_NUMPAD2         0x50
#define KEYCODE_NUMPAD3         0x51
#define KEYCODE_NUMPAD0         0x52
#define KEYCODE_DECIMAL         0x53    /* . on numeric keypad */
#define KEYCODE_OEM_102         0x56    /* <> or \| on RT 102-key keyboard (Non-U.S.) */
#define KEYCODE_F11             0x57
#define KEYCODE_F12             0x58
#define KEYCODE_F13             0x64    /*                     (NEC PC98) */
#define KEYCODE_F14             0x65    /*                     (NEC PC98) */
#define KEYCODE_F15             0x66    /*                     (NEC PC98) */
#define KEYCODE_KANA            0x70    /* (Japanese keyboard)            */
#define KEYCODE_ABNT_C1         0x73    /* /? on Brazilian keyboard */
#define KEYCODE_CONVERT         0x79    /* (Japanese keyboard)            */
#define KEYCODE_NOCONVERT       0x7B    /* (Japanese keyboard)            */
#define KEYCODE_YEN             0x7D    /* (Japanese keyboard)            */
#define KEYCODE_ABNT_C2         0x7E    /* Numpad . on Brazilian keyboard */
#define KEYCODE_NUMPADEQUALS    0x8D    /* = on numeric keypad (NEC PC98) */
#define KEYCODE_PREVTRACK       0x90    /* Previous Track (KEYCODE_CIRCUMFLEX on Japanese keyboard) */
#define KEYCODE_AT              0x91    /*                     (NEC PC98) */
#define KEYCODE_COLON           0x92    /*                     (NEC PC98) */
#define KEYCODE_UNDERLINE       0x93    /*                     (NEC PC98) */
#define KEYCODE_KANJI           0x94    /* (Japanese keyboard)            */
#define KEYCODE_STOP            0x95    /*                     (NEC PC98) */
#define KEYCODE_AX              0x96    /*                     (Japan AX) */
#define KEYCODE_UNLABELED       0x97    /*                        (J3100) */
#define KEYCODE_NEXTTRACK       0x99    /* Next Track */
#define KEYCODE_NUMPADENTER     0x9C    /* Enter on numeric keypad */
#define KEYCODE_RCONTROL        0x9D
#define KEYCODE_MUTE            0xA0    /* Mute */
#define KEYCODE_CALCULATOR      0xA1    /* Calculator */
#define KEYCODE_PLAYPAUSE       0xA2    /* Play / Pause */
#define KEYCODE_MEDIASTOP       0xA4    /* Media Stop */
#define KEYCODE_VOLUMEDOWN      0xAE    /* Volume - */
#define KEYCODE_VOLUMEUP        0xB0    /* Volume + */
#define KEYCODE_WEBHOME         0xB2    /* Web home */
#define KEYCODE_NUMPADCOMMA     0xB3    /* , on numeric keypad (NEC PC98) */
#define KEYCODE_DIVIDE          0xB5    /* / on numeric keypad */
#define KEYCODE_SYSRQ           0xB7
#define KEYCODE_RMENU           0xB8    /* right Alt */
#define KEYCODE_PAUSE           0xC5    /* Pause */
#define KEYCODE_HOME            0xC7    /* Home on arrow keypad */
#define KEYCODE_UP              0xC8    /* UpArrow on arrow keypad */
#define KEYCODE_PRIOR           0xC9    /* PgUp on arrow keypad */
#define KEYCODE_LEFT            0xCB    /* LeftArrow on arrow keypad */
#define KEYCODE_RIGHT           0xCD    /* RightArrow on arrow keypad */
#define KEYCODE_END             0xCF    /* End on arrow keypad */
#define KEYCODE_DOWN            0xD0    /* DownArrow on arrow keypad */
#define KEYCODE_NEXT            0xD1    /* PgDn on arrow keypad */
#define KEYCODE_INSERT          0xD2    /* Insert on arrow keypad */
#define KEYCODE_DELETE          0xD3    /* Delete on arrow keypad */
#define KEYCODE_LWIN            0xDB    /* Left Windows key */
#define KEYCODE_RWIN            0xDC    /* Right Windows key */
#define KEYCODE_APPS            0xDD    /* AppMenu key */
#define KEYCODE_POWER           0xDE    /* System Power */
#define KEYCODE_SLEEP           0xDF    /* System Sleep */
#define KEYCODE_WAKE            0xE3    /* System Wake */
#define KEYCODE_WEBSEARCH       0xE5    /* Web Search */
#define KEYCODE_WEBFAVORITES    0xE6    /* Web Favorites */
#define KEYCODE_WEBREFRESH      0xE7    /* Web Refresh */
#define KEYCODE_WEBSTOP         0xE8    /* Web Stop */
#define KEYCODE_WEBFORWARD      0xE9    /* Web Forward */
#define KEYCODE_WEBBACK         0xEA    /* Web Back */
#define KEYCODE_MYCOMPUTER      0xEB    /* My Computer */
#define KEYCODE_MAIL            0xEC    /* Mail */
#define KEYCODE_MEDIASELECT     0xED    /* Media Select */

/*
*  Alternate names for keys, to facilitate transition from DOS.
*/
#define KEYCODE_BACKSPACE       KEYCODE_BACK            /* backspace */
#define KEYCODE_NUMPADSTAR      KEYCODE_MULTIPLY        /* * on numeric keypad */
#define KEYCODE_LALT            KEYCODE_LMENU           /* left Alt */
#define KEYCODE_CAPSLOCK        KEYCODE_CAPITAL         /* CapsLock */
#define KEYCODE_NUMPADMINUS     KEYCODE_SUBTRACT        /* - on numeric keypad */
#define KEYCODE_NUMPADPLUS      KEYCODE_ADD             /* + on numeric keypad */
#define KEYCODE_NUMPADPERIOD    KEYCODE_DECIMAL         /* . on numeric keypad */
#define KEYCODE_NUMPADSLASH     KEYCODE_DIVIDE          /* / on numeric keypad */
#define KEYCODE_RALT            KEYCODE_RMENU           /* right Alt */
#define KEYCODE_UPARROW         KEYCODE_UP              /* UpArrow on arrow keypad */
#define KEYCODE_PGUP            KEYCODE_PRIOR           /* PgUp on arrow keypad */
#define KEYCODE_LEFTARROW       KEYCODE_LEFT            /* LeftArrow on arrow keypad */
#define KEYCODE_RIGHTARROW      KEYCODE_RIGHT           /* RightArrow on arrow keypad */
#define KEYCODE_DOWNARROW       KEYCODE_DOWN            /* DownArrow on arrow keypad */
#define KEYCODE_PGDN            KEYCODE_NEXT            /* PgDn on arrow keypad */


#endif //_inputsystem_h_