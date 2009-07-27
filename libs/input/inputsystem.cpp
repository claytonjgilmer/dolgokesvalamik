#include "inputsystem.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#endif

//#define BUFFERED_MOUSE


	//////////////////////////////////////////////////////////////////////////
	//	inputsystem
	//////////////////////////////////////////////////////////////////////////

	DEFINE_SINGLETON(inputsystem);
	
	LPDIRECTINPUT8			g_lpDI=NULL;
	LPDIRECTINPUTDEVICE8	g_lpKeyboardDevice=NULL;
	LPDIRECTINPUTDEVICE8	g_lpMouseDevice=NULL;

	//joystick enumeralas
	BOOL CALLBACK enum_joysticks_callback( const DIDEVICEINSTANCE* pdidInstance,void* pContext )
	{
		HRESULT hr;

		inputsystem* ip=(inputsystem*)pContext;

		hr = g_lpDI->CreateDevice( pdidInstance->guidInstance, &ip->m_JoyStruct[ip->m_JoystickNum].g_lpJoyDevice, NULL );

		if (!FAILED(hr))
			ip->m_JoystickNum++;

		return DIENUM_CONTINUE;
	}


	//egy joy alkatreszeinek enumeralasa
	BOOL CALLBACK EnumObjectsCallback( const DIDEVICEOBJECTINSTANCE* pdidoi,void* tmp )
	{
		inputsystem* ip=(inputsystem*)tmp;
		int joyindex=ip->m_JoystickNum-1;

		static int nSliderCount = 0;  // Number of returned slider controls
		static int nPOVCount = 0;     // Number of returned POV controls

		// For axes that are returned, set the DIPROP_RANGE property for the
		// enumerated axis in order to scale min/max values.
		if( pdidoi->dwType & DIDFT_AXIS )
		{
			DIPROPRANGE diprg; 
			diprg.diph.dwSize       = sizeof(DIPROPRANGE); 
			diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
			diprg.diph.dwHow        = DIPH_BYID; 
			diprg.diph.dwObj        = pdidoi->dwType; // Specify the enumerated axis
			diprg.lMin              = -1000; 
			diprg.lMax              = +1000; 

			// Set the range for the axis
			if( FAILED( ip->m_JoyStruct[joyindex].g_lpJoyDevice->SetProperty( DIPROP_RANGE, &diprg.diph ) ) ) 
				return DIENUM_STOP;

			ip->m_JoyStruct[joyindex].m_AxisNum++;

		}
		else if (pdidoi->dwType & DIDFT_POV)
		{
			ip->m_JoyStruct[joyindex].m_PovNum++;
		}
		else if (pdidoi->dwType & DIDFT_BUTTON)
		{
			ip->m_JoyStruct[joyindex].m_ButtonNum++;
		}
		else
		{
			ip->m_JoyStruct[joyindex].m_ThingAndSomeThingNum++;
		}

		return DIENUM_CONTINUE;
	}








	//inputsystem konstruktor
	inputsystem::inputsystem(const inputinitparams* i_params)
	{
		memset(m_KeyboardState,0,256);
		memset(m_PrevKeyboardState,0,256);
		ZeroMemory(&m_MouseState,sizeof(m_MouseState));
		ZeroMemory(&m_PrevMouseState,sizeof(m_PrevMouseState));

		m_JoystickNum=0;

		init(*i_params);
	}

	//inputsystem destruktor
	inputsystem::~inputsystem()
	{
		if (g_lpKeyboardDevice)
		{
			g_lpKeyboardDevice->Unacquire();
			g_lpKeyboardDevice->Release();
		}

		if (g_lpMouseDevice)
		{
			g_lpMouseDevice->Unacquire();
			g_lpMouseDevice->Release();
		}

		for (unsigned n=0; n<m_JoystickNum; ++n)
		{
			m_JoyStruct[n].g_lpJoyDevice->Unacquire();
			m_JoyStruct[n].g_lpJoyDevice->Release();
		}

		m_JoystickNum=0;

		if (g_lpDI)
			g_lpDI->Release();
	}


	//keyboard inicialilzalasa
	void inputsystem::init_keyboard(HWND i_Hwnd)
	{
		//keyboard
		g_lpDI->CreateDevice(GUID_SysKeyboard, &g_lpKeyboardDevice, NULL);

		if (g_lpKeyboardDevice)
		{
			g_lpKeyboardDevice->SetDataFormat(&c_dfDIKeyboard);
			g_lpKeyboardDevice->SetCooperativeLevel(i_Hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
			g_lpKeyboardDevice->Acquire();
		}
		ZeroMemory(&m_KeyboardState,sizeof(m_KeyboardState));
	}

	//eger inicializalasa
	void inputsystem::init_mouse(HWND i_Hwnd)
	{
		//mouse
		g_lpDI->CreateDevice(GUID_SysMouse, &g_lpMouseDevice, NULL);

		if (g_lpMouseDevice)
		{
			g_lpMouseDevice->SetDataFormat(&c_dfDIMouse2);
			g_lpMouseDevice->SetCooperativeLevel(i_Hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);


#ifdef BUFFERED_MOUSE
#define MOUSEBUFFER_SIZE	16

			DIPROPDWORD dipdw;
			dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
			dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
			dipdw.diph.dwObj        = 0;
			dipdw.diph.dwHow        = DIPH_DEVICE;
			dipdw.dwData            = MOUSEBUFFER_SIZE;

			HRESULT hr;

			if( FAILED( hr = g_lpMouseDevice->SetProperty( DIPROP_BUFFERSIZE, &dipdw.diph ) ) )
				assert(0);
#endif //BUFFERED_MOUSE

		}

		ZeroMemory(&m_MouseState,sizeof(m_MouseState));

		g_lpMouseDevice->Acquire();

	}


	//joystickok inicializalasa
	void inputsystem::init_joys(HWND i_Hwnd)
	{
		//Joysticks
		if ( !FAILED( g_lpDI->EnumDevices(	DI8DEVCLASS_GAMECTRL, 
			enum_joysticks_callback,
			this,
			DIEDFL_ATTACHEDONLY ) ) )
		{
			for (unsigned n=0; n<m_JoystickNum; ++n)
			{
				if (FAILED(m_JoyStruct[n].g_lpJoyDevice->SetDataFormat(&c_dfDIJoystick2)))
					continue;

				if( FAILED(m_JoyStruct[n].g_lpJoyDevice->SetCooperativeLevel(i_Hwnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND )))
					continue;

				if( FAILED(m_JoyStruct[n].g_lpJoyDevice->EnumObjects( EnumObjectsCallback, this, DIDFT_ALL ) ) )
					continue;
			}
		}
	}


	//inputsystem inicializalasa
	void inputsystem::init(const inputinitparams& i_Params)
	{
		DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, 
			IID_IDirectInput8, (void**)&g_lpDI, NULL);


		init_keyboard(i_Params.m_Window);
		init_mouse(i_Params.m_Window);
		init_joys(i_Params.m_Window);

		m_Inited=true;
	}

	void inputsystem::clear()
	{
		update_keyboard();
		update_mouse();
		ZeroMemory(&m_PrevMouseState,sizeof(m_MouseState));
		ZeroMemory(&m_MouseState,sizeof(m_MouseState));
		ZeroMemory(&m_KeyboardState,sizeof(m_KeyboardState));
		ZeroMemory(&m_PrevKeyboardState,sizeof(m_PrevKeyboardState));

	}



	//billentyuzet lekerdezese
	void inputsystem::update_keyboard()
	{
		//keyboard
		if (g_lpKeyboardDevice)
		{
			unsigned char keyboardstate[256];
			HRESULT hr = g_lpKeyboardDevice->Poll();

			if (FAILED(hr)) 
			{
				hr = g_lpKeyboardDevice->Acquire();
				ZeroMemory(&m_KeyboardState,sizeof(m_KeyboardState));
				ZeroMemory(&m_PrevKeyboardState,sizeof(m_KeyboardState));
			}
			else
			{
				HRESULT hr = g_lpKeyboardDevice->GetDeviceState(256,keyboardstate);

				unsigned char* ks=m_KeyboardState;
				unsigned char* aks=keyboardstate;

				memcpy(m_PrevKeyboardState,m_KeyboardState,256);

				for (int n=0; n<256; ++n,++ks,++aks)
				{
					*ks=(*aks & 0x80) >> 7 ;
				}

			}
		}
	}


	//eger lekerdezese
	unsigned maxelemnum=0;
	void inputsystem::update_mouse()
	{
		//mouse
		if (g_lpMouseDevice)
		{
#ifdef BUFFERED_MOUSE
			DIDEVICEOBJECTDATA didod[MOUSEBUFFER_SIZE];
			DWORD elemnum=MOUSEBUFFER_SIZE;



			HRESULT hr=g_lpMouseDevice->GetDeviceData(sizeof(DIDEVICEOBJECTDATA),didod,&elemnum,0);

			if (FAILED(hr))
			{
				hr=g_lpMouseDevice->Acquire();
			}
			else
			{
				if (elemnum>maxelemnum)
					maxelemnum=elemnum;
				m_PrevMouseState=m_MouseState;
				m_MouseState.lX=0;
				m_MouseState.lY=0;
				m_MouseState.lZ=0;
				DIDEVICEOBJECTDATA* ptr=didod;
				for (unsigned n=0; n<elemnum; ++n,ptr++)
				{
					switch(ptr->dwOfs)
					{
					case DIMOFS_BUTTON0:
						m_MouseState.rgbButtons[0]=((BYTE)ptr->dwData & 0x80) >> 7;
						break;
					case DIMOFS_BUTTON1:
						m_MouseState.rgbButtons[1]=((BYTE)ptr->dwData & 0x80) >> 7;
						break;
					case DIMOFS_BUTTON2:
						m_MouseState.rgbButtons[2]=((BYTE)ptr->dwData & 0x80) >> 7;
						break;
					case DIMOFS_BUTTON3:
						m_MouseState.rgbButtons[3]=((BYTE)ptr->dwData & 0x80) >> 7;
						break;
					case DIMOFS_BUTTON4:
						m_MouseState.rgbButtons[4]=((BYTE)ptr->dwData & 0x80) >> 7;
						break;
					case DIMOFS_BUTTON5:
						m_MouseState.rgbButtons[5]=((BYTE)ptr->dwData & 0x80) >> 7;
						break;
					case DIMOFS_BUTTON6:
						m_MouseState.rgbButtons[6]=((BYTE)ptr->dwData & 0x80) >> 7;
						break;
					case DIMOFS_BUTTON7:
						m_MouseState.rgbButtons[7]=((BYTE)ptr->dwData & 0x80) >> 7;
						break;
					case DIMOFS_X:
						m_MouseState.lX+=ptr->dwData;
						break;
					case DIMOFS_Y:
						m_MouseState.lY+=ptr->dwData;
						break;
					case DIMOFS_Z:
						m_MouseState.lZ+=ptr->dwData;
						break;
					}

				}
			}


#else
			HRESULT hr = g_lpMouseDevice->Poll();
			if (FAILED(hr)) 
			{
				hr = g_lpMouseDevice->Acquire();
			}
			else
			{
				m_PrevMouseState=m_MouseState;
				hr = g_lpMouseDevice->GetDeviceState(sizeof(DIMOUSESTATE2), &m_MouseState);

				for (int n=0; n<8; ++n)
				{
					m_MouseState.rgbButtons[n]=(m_MouseState.rgbButtons[n] & 0x80) >> 7;
				}
			}
#endif //BUFFERED_MOUSE
		}
	}

	//joyok lekerdezese
	void inputsystem::update_joys()
	{
		for (unsigned n=0; n<m_JoystickNum; ++n)
		{
			memcpy(&m_JoyStruct[n].m_PrevJoyState,&m_JoyStruct[n].m_JoyState,m_JoystickNum*sizeof(DIJOYSTATE2));
		}

		for (unsigned n=0; n<m_JoystickNum; ++n)
		{
			HRESULT hr;

			hr=m_JoyStruct[n].g_lpJoyDevice->Poll();

			if( FAILED(hr) )  
			{
				m_JoyStruct[n].g_lpJoyDevice->Acquire();
				continue;
			}

			if(FAILED( m_JoyStruct[n].g_lpJoyDevice->GetDeviceState( sizeof(DIJOYSTATE2), &m_JoyStruct[n].m_JoyState)))
				continue;

			for (int m=0; m<m_JoyStruct[n].m_ButtonNum; ++m)
			{
				m_JoyStruct[n].m_JoyState.rgbButtons[m]=(m_JoyStruct[n].m_JoyState.rgbButtons[m] & 0x80) >> 7;
			}
		}
	}

	void inputsystem::update(bool eger_is)
	{
		update_keyboard();

		if (eger_is)
		update_mouse();
//		UpdateJoys();
	}
