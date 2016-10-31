#include "stdafx.h"
#include "EventInput.h"


CEventInput::CEventInput(void)
{
	m_ulEventType=EVENT_INPUT;
}


CEventInput::~CEventInput(void)
{
}

bool CEventInput::sm_bKeyState[256];
CEventInput::MOUSE_STATE CEventInput::sm_MouseState;
