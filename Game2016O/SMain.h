#pragma once
#include "HSM\StateBase.h"
#include "Graphics\DXManager.h"
#include "Graphics\DXBasicPainter.h"
#include "Sound\SndManager.h"
#include "Sound\SndControl.h"
#include "Sound\SndFx.h"
#include "Sound\SndFactory.h"
#include "Input\InputManager.h"

#define MAIN ((CSMain*)m_pSMOwner->GetObjectByID(CLSID_CSMain))

#define CLSID_CSMain 0x0dccd3ed

#define INPUT_EVENT 0x12345678
class CInputEvent : public CEventBase
{
public:
	int				m_nSource; // Quien fue
	unsigned long	m_ulTime; // Estampa temporal, para por ejemplo los combos
	DIJOYSTATE2		m_js2;

	CInputEvent(int nSource, unsigned long ulTime, DIJOYSTATE2& js2) {
		m_ulEventType = INPUT_EVENT;
		m_nSource = nSource;
		m_ulTime = ulTime;
		m_js2 = js2;
	}
};

class CSMain :
	public CStateBase
{
public:
	HINSTANCE	m_hInstance;
	HWND		m_hWnd;
	CDXManager* m_pDXManager;
	CDXBasicPainter* m_pDXPainter;
	CSndManager* m_pSndManager;
	CInputManager* m_pInputManager;
	bool        m_bInitializationCorrect;
	unsigned long GetClassID() { return CLSID_CSMain; }
	const char* GetClassString() { return "CSMain"; }
	CSMain();
	virtual ~CSMain();
protected:
	void OnEntry(void);
	unsigned long OnEvent(CEventBase* pEvent);
	void OnExit(void);
};