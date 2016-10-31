#pragma once
#include "HSM\StateBase.h"
#define CLSID_CSGame 0x33221100
class CSGame :
	public CStateBase
{
public:
	unsigned long GetClassID() { return CLSID_CSGame; }
	const char* GetClassString() { return "CSGame"; }
protected:
	void OnEntry();
	unsigned long OnEvent(CEventBase* pEvent);
	void OnExit();
public:
	CSGame();
	virtual ~CSGame();
};

