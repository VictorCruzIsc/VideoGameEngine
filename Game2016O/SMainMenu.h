#pragma once
#include "HSM\StateBase.h"
#include <d3d11.h>
#define CLSID_CSMainMenu 0x12345566
class CSMainMenu :
	public CStateBase
{
public:
	const char* GetClassString() { return "CSMainMenu"; }
	unsigned long GetClassID() { return CLSID_CSMainMenu; }
public:
	CSMainMenu();
	virtual ~CSMainMenu();
private:
	ID3D11ShaderResourceView
		*m_pSRVBackGround,
		*m_pSRVMainOption1,
		*m_pSRVMainOption2;
	int m_nOption;    //0: Option1, 1: Option2
	void OnEntry();
	unsigned long OnEvent(CEventBase* pEvent);
	void OnExit();
	float m_fOffsetX;
	float m_fOffsetY;
};

