#include "stdafx.h"
#include "SMain.h"


CSMain::CSMain()
{
	m_hWnd = NULL;
	m_pDXManager = NULL;
	m_pDXPainter = NULL;
	m_bInitializationCorrect = true;
	m_pSndManager = NULL;
	m_pInputManager = NULL;
}


CSMain::~CSMain()
{
}

void CSMain::OnEntry(void)
{
	printf("Iniciando motores...\n");
	printf("Graphics init...\n");
	fflush(stdout); // Por el llenado de 16 bits
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
	if (m_bInitializationCorrect) {
		printf("Succes\n"); fflush(stdout);
	}

	// Creando el objeto del snd manager
	printf("Sound Init"); fflush(stdout);
	CSndFactory *pFactory = new CSndFactory();
	m_pSndManager = (CSndManager*)pFactory->CreateObject(L"CSndManager");
	if (!m_pSndManager->InitSoundEngine(m_hWnd))
		m_bInitializationCorrect = false;
	if (m_bInitializationCorrect) {
		printf("Succes\n"); fflush(stdout);
	}
	else {
		printf("Bad"); fflush(stdout);
	}

	printf("Input init...\n"); fflush(stdout);
	m_pInputManager = new CInputManager();
	if (!m_pInputManager->InitializeDirectInputSession(m_hInstance)) {
		printf("Bad input init...\n");
	}
	if (m_pInputManager->ConfigureDevices(m_hWnd)) {
		printf("Unable to acquire input devices...\n");
		m_bInitializationCorrect = false;
	}
	fflush(stdout);
}
#include "HSM\EventWin32.h"
#include "HSM\StateMachineManager.h"
unsigned long CSMain::OnEvent(CEventBase * pEvent)
{
	// En tiempos libres libera sonidos
	if (APP_LOOP == pEvent->m_ulEventType) {
		// Encargado de quitar el sonido muerto cuando tenga tiempo libre
		m_pSndManager->RemoveAllSndFxStopped(); // Remover todos los eventos de sonidos que han sido reproducidos

		// Escaneo de dispostivos
		for (int iSource = 0; iSource < m_pInputManager->GetDeviceCount(); iSource++) {
			DIJOYSTATE2 js2;
			if (m_pInputManager->ReadState(js2, iSource)) {
				CInputEvent *pInput = new CInputEvent(iSource, 0, js2);
				m_pSMOwner->PostEvent(pInput);
			}

		}
		// Encargado de Dibujar cuando tenga tiempo libre

		/*
		if (m_bInitializationCorrect)
		{
			ID3D11RenderTargetView* pRTV = m_pDXManager->GetMainRTV();
			ID3D11DepthStencilView* pDSV = m_pDXManager->GetMainDSV();
			ID3D11DeviceContext* pCtx = m_pDXManager->GetContext();
			//Limpieza el render target
			VECTOR4D Color = { 0,0,0,0 };
			pCtx->ClearRenderTargetView(pRTV, (float*)&Color);
			pCtx->ClearDepthStencilView(pDSV, D3D11_CLEAR_STENCIL | D3D11_CLEAR_DEPTH, 1.0f, 0);
			//Establecer Render Target y Buffer Z a trav�s de DXBasicPainter
			m_pDXPainter->SetRenderTarget(pRTV, pDSV);
			CDXBasicPainter::VERTEX Triangle[] =
			{
				//Position  | Normal | Binormal | Tangent | Color | TexCoord
				{ { 0,1,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,0,0,0 },{ 0,0,0,0 } },
				{ { 1,-1,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,1,0,0 },{ 0,0,0,0 } },
				{ { -1,-1,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,1,0 },{ 0,0,0,0 } }
			};
			unsigned long TriangleIndices[] = { 0,1,2 };
			//Establecer el modelo de iluminaci�n a V�rtice Pre-iluminado 
			//As� no se hacen calculos de color adicionales en GPU ya que el atributo Color del v�rtice lo especifica
			m_pDXPainter->m_Params.Flags = VERTEX_PRELIGHTEN;
			m_pDXPainter->DrawIndexed(Triangle, 3, TriangleIndices, 3);
			m_pDXManager->GetSwapChain()->Present(1, 0);
			return 0;
		}
		*/


	}

	if (EVENT_WIN32 == pEvent->m_ulEventType)
	{
		CEventWin32* pWin32 = (CEventWin32*)pEvent;
		switch (pWin32->m_msg)
		{
		case WM_PAINT:
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
	m_pSndManager->UnitializeSoundEngine();
	m_pInputManager->FinalizeDirectInputSession();
	SAFE_DELETE(m_pDXPainter);
	SAFE_DELETE(m_pDXManager);
	CSndFactory Factory;
	Factory.DestroyObject(m_pSndManager);
	SAFE_DELETE(m_pInputManager);
}
