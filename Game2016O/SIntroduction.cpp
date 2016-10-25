#include "stdafx.h"
#include "SIntroduction.h"
#include "HSM\StateMachineManager.h"
#include "SMain.h"
#include "SGame.h"
#include "Graphics\ImageBMP.h"

void CSIntroduction::OnEntry()
{
	CImageBMP *pIntro = CImageBMP::CreateBitmapFromFile("..\\Assets\\intro.bmp", NULL);
	m_pSplash = pIntro->CreateTexture(MAIN->m_pDXManager);
	CImageBMP::DestroyBitmap(pIntro);
	MAIN->m_pDXManager->GetDevice()->CreateShaderResourceView(m_pSplash, NULL, &m_pSRVSplash);
	SetTimer(MAIN->m_hWnd, 1, 5000, NULL);
}

#include "HSM\EventWin32.h"
unsigned long CSIntroduction::OnEvent(CEventBase * pEvent)
{
	if (EVENT_WIN32 == pEvent->m_ulEventType) {
		CEventWin32 *pWin32 = (CEventWin32*)pEvent;
		switch (pWin32->m_msg)
		{
			case WM_PAINT:
			{
				CDXBasicPainter *pPainter = MAIN->m_pDXPainter;
				CDXBasicPainter::VERTEX Frame[4]
				{
					{{ -1, 1, 0, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}, { 0, 0, 0, 0 }, { 1, 1, 1, 1 }, { 0, 0, 0, 0 }},
					{{  1, 1, 0, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}, { 0, 0, 0, 0 }, { 1, 1, 1, 1 }, { 1, 0, 0, 0 }},
					{{ -1, -1, 0, 1},{0, 0, 0, 0}, {0, 0, 0, 0}, { 0, 0, 0, 0 }, { 1, 1, 1, 1 }, { 0, 1, 0, 0 }},
					{{  1, -1, 0, 1},{0, 0, 0, 0}, {0, 0, 0, 0}, { 0, 0, 0, 0 }, { 1, 1, 1, 1 }, { 1, 1, 0, 0 }}
				};
				unsigned long FrameIndex[6] = { 0, 1, 2, 2, 1, 3 };

				pPainter->m_Params.Projection =
					pPainter->m_Params.View =
					pPainter->m_Params.World = Identity();

				MAIN->m_pDXManager->GetContext()->PSSetShaderResources(3, 1, &m_pSRVSplash);
				pPainter->m_Params.Flags = MAPPING_EMISSIVE;
				pPainter->SetRenderTarget(MAIN->m_pDXManager->GetMainRTV(), NULL);
				pPainter->DrawIndexed(Frame, 4, FrameIndex, 6);
				MAIN->m_pDXManager->GetSwapChain()->Present(1, 0);
			}
			return 0;
			case WM_TIMER:
				if (1 == pWin32->m_wParam) {
					m_pSMOwner->Transition(CLSID_CSGame);
					InvalidateRect(MAIN->m_hWnd, NULL, false);
				}
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
