#include "stdafx.h"
#include "HSM\StateMachineManager.h"
#include "SIntroduction.h"
#include "SMain.h"
#include "Graphics\ImageBMP.h"
#include "SGame.h"
#include "SMainMenu.h"
void CSIntroduction::OnEntry()
{
	CImageBMP* pIntro =
		CImageBMP::CreateBitmapFromFile("..\\Assets\\Introduction.bmp", NULL);
	m_pSplash = pIntro->CreateTexture(MAIN->m_pDXManager);
	CImageBMP::DestroyBitmap(pIntro);
	MAIN->m_pDXManager->GetDevice()->CreateShaderResourceView(m_pSplash, NULL, &m_pSRVSplash);
	
	MAIN->m_pSndManager->ClearEngine();
	auto fx=MAIN->m_pSndManager->LoadSoundFx(L"..\\Assets\\Explosion.wav",SND_EXPLOSION);
	if (fx)
		printf("Explosion load success...\n");
	else
		printf("Explosion load failed...\n");
	m_pSndBackground = MAIN->m_pSndManager->LoadSoundFx(L"..\\Assets\\FFX.wav", SND_BACKGROUND);
	if (m_pSndBackground)
		m_pSndBackground->Play(false);
	SetTimer(MAIN->m_hWnd, 1, 2000, NULL);
	SetTimer(MAIN->m_hWnd, 2, 1000, NULL);
	fflush(stdout);
}
#include "HSM\EventWin32.h"
unsigned long CSIntroduction::OnEvent(CEventBase * pEvent)
{
	static float speed = 1.0f;
	if (APP_LOOP == pEvent->m_ulEventType)
	{
		CDXBasicPainter* pPainter = MAIN->m_pDXPainter;
		pPainter->SetRenderTarget(MAIN->m_pDXManager->GetMainRTV(), NULL);
		CDXBasicPainter::VERTEX Frame[4]
		{
			{ { -1,1,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 0,0,0,0 } },
			{ { 1,1,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 1,0,0,0 } },
			{ { -1,-1,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 0,1,0,0 } },
			{ { 1,-1,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 1,1,0,0 } }
		};
		unsigned long FrameIndex[6] = { 0,1,2,2,1,3 };
		pPainter->m_Params.Projection =
			pPainter->m_Params.View =
			pPainter->m_Params.World = Identity();
		MAIN->m_pDXManager->GetContext()->PSSetShaderResources(3, 1, &m_pSRVSplash);
		pPainter->m_Params.Flags = MAPPING_EMISSIVE;
		pPainter->DrawIndexed(Frame, 4, FrameIndex, 6);
		MAIN->m_pDXManager->GetSwapChain()->Present(1, 0);
	}
	if (EVENT_WIN32 == pEvent->m_ulEventType)
	{
		CEventWin32* pWin32 = (CEventWin32*)pEvent;
		switch (pWin32->m_msg)
		{
			case WM_PAINT:
			{
			}
			return 0;
			case WM_TIMER:
				if (1 == pWin32->m_wParam)
				{
					m_pSMOwner->Transition(CLSID_CSMainMenu);
					InvalidateRect(MAIN->m_hWnd, NULL, false);
					return 0;
				}
				if (2 == pWin32->m_wParam)
				{
					KillTimer(MAIN->m_hWnd, 2);
					MAIN->m_pSndManager->PlayFx(SND_EXPLOSION,1,0,0.3);
				}
				break;
			case WM_CHAR:
				switch (pWin32->m_wParam)
				{
				case 'i':
					speed = min(4.0, speed + 0.1);
					m_pSndBackground->SetSpeed(speed);
					break;
				case 'k':
					speed = max(0.1, speed - 0.1);
					m_pSndBackground->SetSpeed(speed);
					break;
				}
				break;
		}
	}
	return __super::OnEvent(pEvent);
}

void CSIntroduction::OnExit()
{
	KillTimer(MAIN->m_hWnd, 1);
	SAFE_RELEASE(m_pSplash);
}

CSIntroduction::CSIntroduction()
{
}


CSIntroduction::~CSIntroduction()
{
}
