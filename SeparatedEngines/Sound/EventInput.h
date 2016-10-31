#pragma once
#include "eventbase.h"
#include "InputManager.h"
#define EVENT_INPUT 0x5fcada8f
#define INPUT_KEYB 0x00
#define INPUT_POINTER  0x01
#define INPUT_JOYSTICK0 0x02
#define INPUT_JOYSTICK1 0x03
#define INPUT_JOYSTICK2 0x04
#define INPUT_JOYSTICK3 0x05
class CEventInput :
	public CEventBase
{
public:
	unsigned long m_ulInputType;
	//Estado del teclado
	static bool sm_bKeyState[256];	
	struct MOUSE_STATE 
	{
		int m_nPosX;
		int m_nPosY;
		int m_nLastX;
		int m_nLastY;
		int m_nDeltaX;
		int m_nDeltaY;
		bool m_bMouseButtons[32];
	} ;
	static MOUSE_STATE sm_MouseState;
	union
	{
		//INPUT_JOYSTICKn
		DIJOYSTATE2 m_JoyState;
		//Estado del apuntador
		
	};
	CEventInput(void);
	virtual ~CEventInput(void);
};

