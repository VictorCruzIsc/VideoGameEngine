#pragma once
#include "eventbase.h"

#define EVENT_CTRL 0x7cfad445

class CEventCtrl :
	public CEventBase
{
public:
	enum Controls{ UP,DOWN,LEFT,RIGHT,FRONT,BACK,ROLL_LEFT,ROLL_RIGHT, BUTTON0,BUTTON1,BUTTON2,BUTTON3};
	Controls m_Ctrl;
	float    m_fParam;
	CEventCtrl(void);
	virtual ~CEventCtrl(void);
};

