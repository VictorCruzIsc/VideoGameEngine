#include "stdafx.h"
#include "SMainMenu.h"
#include "HSM\EventWin32.h"
#include "HSM\StateMachineManager.h"
#include "Graphics\ImageBMP.h"
#include "SMain.h"
CSMainMenu::CSMainMenu()
{
	m_pSRVBackGround = NULL;
	m_pSRVMainOption1 = NULL;
	m_pSRVMainOption2 = NULL;
}
CSMainMenu::~CSMainMenu()
{
}

void CSMainMenu::OnEntry()
{
	m_nOption = 0;
	printf("Iniciando menú principal...\n"); fflush(stdout);
	{
		printf("Cargando recurso de fondo..."); fflush(stdout);
		auto img = CImageBMP::CreateBitmapFromFile("..\\Assets\\MainMenu.bmp", NULL);
		if (!img) { printf("Recurso no encontrado\n"); fflush(stdout); }
		else
		{
			auto tex = img->CreateTexture(MAIN->m_pDXManager);
			MAIN->m_pDXManager->GetDevice()->CreateShaderResourceView(
				tex, NULL, &m_pSRVBackGround);
		}
	}
	{
		printf("Cargando recurso de opcion 1..."); fflush(stdout);
		auto img = CImageBMP::CreateBitmapFromFile("..\\Assets\\MainOption1.bmp", NULL);
		if (!img) { printf("Recurso no encontrado\n"); fflush(stdout); }
		else
		{
			auto tex = img->CreateTexture(MAIN->m_pDXManager);
			MAIN->m_pDXManager->GetDevice()->CreateShaderResourceView(
				tex, NULL, &m_pSRVMainOption1);
		}
	}
	{
		printf("Cargando recurso de opcion 2..."); fflush(stdout);
		auto img = CImageBMP::CreateBitmapFromFile("..\\Assets\\MainOption2.bmp", NULL);
		if (!img) { printf("Recurso no encontrado\n"); fflush(stdout); }
		else
		{
			auto tex = img->CreateTexture(MAIN->m_pDXManager);
			MAIN->m_pDXManager->GetDevice()->CreateShaderResourceView(
				tex, NULL, &m_pSRVMainOption2);
		}
	}
	MAIN->m_pSndManager->ClearEngine();
	MAIN->m_pSndManager->LoadSoundFx(L"..\\Assets\\Explosion.wav", 1);
	m_fOffsetX = 0.0f;
	m_fOffsetY = 0.0f;
}
void CSMainMenu::OnExit()
{
	SAFE_RELEASE(m_pSRVBackGround);
	SAFE_RELEASE(m_pSRVMainOption1);
	SAFE_RELEASE(m_pSRVMainOption2);
}
#include "ActionEvent.h"
#include "SGame.h"
unsigned long CSMainMenu::OnEvent(CEventBase* pEvent)
{
	if (ACTION_EVENT == pEvent->m_ulEventType)
	{
		CActionEvent* pInput = (CActionEvent*)pEvent;
		if (JOY_BUTTON_A_PRESSED == pInput->m_nAction) {
			//MAIN->m_pSndManager->PlayFx(1);
			m_pSMOwner->Transition(CLSID_CSGame);
		}
		if (JOY_AXIS_LX == pInput->m_nAction)
		{
			m_fOffsetX = m_fOffsetX + (pInput->m_fAxis - m_fOffsetX)*0.1;
		}
		if (JOY_AXIS_LY == pInput->m_nAction)
		{
			m_fOffsetY =m_fOffsetY + (pInput->m_fAxis - m_fOffsetY)*0.1;
		}
	}
	if (APP_LOOP == pEvent->m_ulEventType)
	{
		auto Painter = MAIN->m_pDXPainter;
		auto Ctx = MAIN->m_pDXManager->GetContext();
		CDXBasicPainter::VERTEX Frame[4]
		{
			{ { -1,1,1,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 0,0,0,0 } },
			{ { 1,1,1,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 1,0,0,0 } },
			{ { -1,-1,1,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 0,1,0,0 } },
			{ { 1,-1,1,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 1,1,0,0 } }
		};
		unsigned long FrameIndex[6] = { 0,1,2,2,1,3 };
		Ctx->ClearDepthStencilView(MAIN->m_pDXManager->GetMainDSV(),
			D3D11_CLEAR_DEPTH, 1.0f, 0);
		Painter->SetRenderTarget(MAIN->m_pDXManager->GetMainRTV(), NULL);
		Painter->m_Params.View =
			Painter->m_Params.World =
			Painter->m_Params.Projection = Identity();
		Painter->m_Params.World = Translation(m_fOffsetX, m_fOffsetY, 0);
		Painter->m_Params.Flags = MAPPING_EMISSIVE;
		Ctx->PSSetShaderResources(3, 1, &m_pSRVBackGround);
		Painter->DrawIndexed(Frame, 4, FrameIndex, 6);
		
		Painter->m_Params.World = Identity();
		//Dibujar la opcion 1
		CDXBasicPainter::VERTEX Frame1[4]
		{
			{ { -0.5f,2.0f / 3.0f,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 0,0,0,0 } },
			{ { 0.5f, 2.0f / 3.0f,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 1,0,0,0 } },
			{ { -0.5f,1.0f / 3.0f,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 0,1,0,0 } },
			{ { 0.5,  1.0f / 3.0f,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 1,1,0,0 } }
		};
		Ctx->PSSetShaderResources(3, 1, &m_pSRVMainOption1);
		Painter->DrawIndexed(Frame1, 4, FrameIndex, 6);
		//Dibujar la opción 2
		CDXBasicPainter::VERTEX Frame2[4]
		{
			{ { -0.5f,-1.0f / 3.0f,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 0,0,0,0 } },
			{ { 0.5f, -1.0f / 3.0f,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 1,0,0,0 } },
			{ { -0.5f,-2.0f / 3.0f,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 0,1,0,0 } },
			{ { 0.5,  -2.0f / 3.0f,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 1,1,0,0 } }
		};
		Ctx->PSSetShaderResources(3, 1, &m_pSRVMainOption2);
		Painter->DrawIndexed(Frame2, 4, FrameIndex, 6);
		MAIN->m_pDXManager->GetSwapChain()->Present(1, 0);
	}
	if (EVENT_WIN32 == pEvent->m_ulEventType)
	{
		CEventWin32* pWin32 = (CEventWin32*)pEvent;
		switch (pWin32->m_msg)
		{
		case WM_CHAR:
			break;
		}
	}
	return __super::OnEvent(pEvent);
}