#include "stdafx.h"
#include "SMain.h"


CSMain::CSMain()
{
	m_hWnd = NULL;
	m_pDXManager = NULL;
	m_pDXPainter = NULL;
	m_bInitializationCorrect = true;
}


CSMain::~CSMain()
{
}

void CSMain::OnEntry(void)
{
	m_pDXManager = new CDXManager();
	m_pDXPainter = new CDXBasicPainter(m_pDXManager);
	if (!m_pDXManager->Initialize(m_hWnd, CDXManager::EnumAndChooseAdapter(NULL)))
	{
		m_bInitializationCorrect = false;
		MessageBox(m_hWnd, L"No se pudo iniciar DirectX 11", L"Error", MB_ICONERROR);
	}
	if (!m_pDXPainter->Initialize())
	{
		m_bInitializationCorrect = false;
		MessageBox(m_hWnd, L"Error no se pudo iniciar shaders", L"Error", MB_ICONERROR);
	}
}
#include "HSM\EventWin32.h"
#include "HSM\StateMachineManager.h"
unsigned long CSMain::OnEvent(CEventBase * pEvent)
{
	if (EVENT_WIN32 == pEvent->m_ulEventType)
	{
		CEventWin32* pWin32 = (CEventWin32*)pEvent;
		switch (pWin32->m_msg)
		{
		case WM_PAINT:
			if (m_bInitializationCorrect)
			{
				ID3D11RenderTargetView* pRTV = m_pDXManager->GetMainRTV();
				ID3D11DepthStencilView* pDSV = m_pDXManager->GetMainDSV();
				ID3D11DeviceContext* pCtx = m_pDXManager->GetContext();
				//Limpieza el render target
				VECTOR4D Color = { 0,0,0,0 };
				pCtx->ClearRenderTargetView(pRTV,(float*)&Color);
				pCtx->ClearDepthStencilView(pDSV, D3D11_CLEAR_STENCIL | D3D11_CLEAR_DEPTH, 1.0f, 0);
				//Establecer Render Target y Buffer Z a través de DXBasicPainter
				m_pDXPainter->SetRenderTarget(pRTV, pDSV);
				CDXBasicPainter::VERTEX Triangle[]=
				{
					//Position  | Normal | Binormal | Tangent | Color | TexCoord
					{{ 0,1,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,0,0,0 },{ 0,0,0,0 }},
					{{ 1,-1,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,1,0,0 },{ 0,0,0,0 }},
					{{ -1,-1,0,1},{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,1,0 },{ 0,0,0,0 }}
				};
				unsigned long TriangleIndices[] = { 0,1,2 };
				//Establecer el modelo de iluminación a Vértice Pre-iluminado 
				//Así no se hacen calculos de color adicionales en GPU ya que el atributo Color del vértice lo especifica
				m_pDXPainter->m_Params.Flags = VERTEX_PRELIGHTEN;
				m_pDXPainter->DrawIndexed(Triangle, 3, TriangleIndices, 3);
				m_pDXManager->GetSwapChain()->Present(1, 0);
				ValidateRect(pWin32->m_hWnd, NULL);
				return 0;
			}
			break;
		case WM_CLOSE:
			m_pSMOwner->Transition(CLSID_CStateNull);
			return 0;
		}
	}
	return __super::OnEvent(pEvent);
}

void CSMain::OnExit(void)
{
	m_pDXPainter->Uninitialize();
	m_pDXManager->Uninitialize();
	SAFE_DELETE(m_pDXPainter);
	SAFE_DELETE(m_pDXManager);
}
