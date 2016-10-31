#pragma once
#include "HSM\StateBase.h"
#include <d3d11.h>
#define SND_EXPLOSION 1
#define SND_BACKGROUND 2
#define CLSID_CSIntroduction 0x43215678
#include "Sound\SndControl.h"
class CSIntroduction :
	public CStateBase
{
public:
	unsigned long GetClassID() { return CLSID_CSIntroduction; }
	const char* GetClassString() { return "CSIntroduction"; }
protected:
	void OnEntry();
	unsigned long OnEvent(CEventBase*pEvent);
	void OnExit();
	//Assets locales
	ID3D11Texture2D* m_pSplash;
	ID3D11ShaderResourceView* m_pSRVSplash;
	CSndControl* m_pSndBackground;
public:
	CSIntroduction();
	virtual ~CSIntroduction();
};

