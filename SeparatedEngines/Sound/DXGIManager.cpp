#include "StdAfx.h"
#include "DXGIManager.h"
#include "GFXBase.h"
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <fstream>
using namespace std;

CDXGIManager::CDXGIManager(void)
{
	m_pISwapChain=NULL;
	m_pID3D11Dev=NULL;
	m_pID3D11DevContext=NULL;
	m_pID3D11SystemMem=NULL;
	m_pID3D11BackBuffer=NULL;
	m_pID3D11BackBufferRTV=NULL;
	m_pID3D11PresentBuffer=NULL;
	m_pID3D11PresentSRV=NULL;
	m_pID3D11PresentRTV=NULL;
	m_pID3D11DepthStencil=NULL;
	m_pID3D11PresentDSV=NULL;
	m_pID3D11DepthSRV=NULL;
	m_pszVSProfile="Unknown";
	m_pszPSProfile="Unknown";
	m_pszCSProfile="Unknown";
	m_FeatureLevel=(D3D_FEATURE_LEVEL)0;
	memset(&m_AdapterDesc,0,sizeof(DXGI_ADAPTER_DESC));
	m_MultiSampleConfig.Count=1;
	m_MultiSampleConfig.Quality=0;
	memset(&m_dscd,0,sizeof(DXGI_SWAP_CHAIN_DESC));
	m_dscd.BufferDesc.RefreshRate.Numerator=0;
	m_dscd.BufferDesc.RefreshRate.Denominator=0;
	m_dscd.BufferDesc.Scaling=DXGI_MODE_SCALING_STRETCHED;
	m_dscd.BufferDesc.ScanlineOrdering=DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
	m_dscd.BufferUsage=DXGI_USAGE_RENDER_TARGET_OUTPUT;
	m_dscd.SwapEffect=DXGI_SWAP_EFFECT_DISCARD;
	m_dscd.BufferDesc.Format=DXGI_FORMAT_R8G8B8A8_UNORM;
	m_dscd.BufferCount=3; //Un backbuffer
	m_dscd.Flags=DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	m_dscd.Windowed=true;
	memset(m_vSamplerStates,0,sizeof(m_vSamplerStates));
	memset(m_vRasterizerStates,0,sizeof(m_vRasterizerStates));
	memset(m_vDepthStencilStates,0,sizeof(m_vDepthStencilStates));
	memset(m_vBlendStates,0,sizeof(m_vBlendStates));
	m_pszApplicationTitle=NULL;
}
CDXGIManager::~CDXGIManager(void)
{

}
#include <vector>
bool CDXGIManager::InitializeSwapChain(HWND hWnd, bool bFullScreen,bool bUseGPU,bool bEnableAntialising,unsigned int uiNumberOfSamplesPerPixel,unsigned int uiSampleQuality ,unsigned int uiFeatureLevel)
{
	RECT rc;
	GetClientRect(hWnd,&rc);
	m_dscd.OutputWindow=hWnd;
	m_dscd.Windowed=!bFullScreen;
	if(m_pszApplicationTitle)
	{
		SetWindowText(hWnd,m_pszApplicationTitle);
	}
	vector<IDXGIAdapter*> vecAdapters;
	IDXGIAdapter* pAdapter=NULL;
	IDXGIFactory1* pFactory=NULL;
	printf("%s","Initializing...\n");
	FlushConsole();
	if(bUseGPU)
	{
		printf("Press SHIFT key to use WARP Device");
		FlushConsole();
		for(int i=0;i<5;i++)
		{
			printf(".");
			FlushConsole();
			Sleep(1500/5);
			if(GetAsyncKeyState(VK_SHIFT)&0x8000)
			{
				printf("\r%s","SHIFT key pressed, trying to use WARP Device             \n");
				bUseGPU=false;
				break;
			}
		}
		if(bUseGPU)
		{
			printf("\r");
		}
		
	}
	if(bUseGPU)
	{
		printf("%s","Enumerating available adapters...                    \n");
		FlushConsole();
		CreateDXGIFactory1(__uuidof(IDXGIFactory1),(void**)&pFactory);
		unsigned int iAdapter=0;
		while(DXGI_ERROR_NOT_FOUND!=pFactory->EnumAdapters(iAdapter,&pAdapter))
		{
			DXGI_ADAPTER_DESC1 ad1;
			IDXGIAdapter1* pAdapter1=NULL;
			pAdapter->QueryInterface(__uuidof(IDXGIAdapter1),(void**)&pAdapter1);
			pAdapter1->GetDesc1(&ad1);
			wprintf(L"Adapter %d:%s\n",iAdapter,ad1.Description);
			FlushConsole();
			vecAdapters.push_back(pAdapter);
			iAdapter++;
			SAFE_RELEASE(pAdapter1);
		}
		printf("Total devices detected:%d\n",vecAdapters.size());
		FlushConsole();
		if(vecAdapters.size()>1)
		{
			DXGI_ADAPTER_DESC ad;
			for(unsigned int i=0;i<vecAdapters.size();i++)
			{
				TCHAR szMessage[1024];
				vecAdapters[i]->GetDesc(&ad);
				wsprintf(szMessage,L"Use this hardware adapter?\r\n%s",ad.Description);
				switch(MessageBox(hWnd,szMessage,L"Select adapter",MB_ICONQUESTION|MB_YESNOCANCEL))
				{
				case IDCANCEL:
					i=vecAdapters.size();
					break;
				case IDYES:
					pAdapter=vecAdapters[i];
					i=vecAdapters.size();
					break;
				case IDNO:
					continue;
				}
			}
			if(!pAdapter)
			{
				while(vecAdapters.size())
				{
					SAFE_RELEASE(vecAdapters.back());
					vecAdapters.pop_back();
				}
				SAFE_RELEASE(pFactory);
				return false;
			}
		}
		else if(vecAdapters.size()==1)
		{
			pAdapter=vecAdapters[0];
		}
		else
		{
			SAFE_RELEASE(pFactory);
			return false;
		}
		SAFE_RELEASE(pFactory);
	}

			
	if(bEnableAntialising)
	{
		m_MultiSampleConfig.Count=uiNumberOfSamplesPerPixel;
		m_MultiSampleConfig.Quality=uiSampleQuality;
	}
	else
	{
		m_MultiSampleConfig.Count=1;
		m_MultiSampleConfig.Quality=0;	
	}
	m_dscd.SampleDesc=m_MultiSampleConfig;
	DXGI_SWAP_CHAIN_DESC dscd=m_dscd;

	if(!bFullScreen)
	{
		dscd.BufferDesc.Height=rc.bottom;
		dscd.BufferDesc.Width=rc.right;
	}
	dscd.SampleDesc.Count=1;
	dscd.SampleDesc.Quality=0;
	HRESULT hr;
	if(uiFeatureLevel)
	{
		//Seleccionar conjunto de features solicitado
		D3D_FEATURE_LEVEL FeatureLevel=(D3D_FEATURE_LEVEL)uiFeatureLevel;
		printf("Requesting DirectX 11 feature level:%d.%d\n",uiFeatureLevel>>12,(uiFeatureLevel&0xfff)>>8);
		if(bUseGPU)
			hr=D3D11CreateDeviceAndSwapChain(pAdapter,D3D_DRIVER_TYPE_UNKNOWN,NULL,0,&FeatureLevel,1,D3D11_SDK_VERSION,&dscd,&m_pISwapChain,&m_pID3D11Dev,&m_FeatureLevel,&m_pID3D11DevContext);
		else
			hr=D3D11CreateDeviceAndSwapChain(NULL,D3D_DRIVER_TYPE_WARP,NULL,0,&FeatureLevel,1,D3D11_SDK_VERSION,&dscd,&m_pISwapChain,&m_pID3D11Dev,&m_FeatureLevel,&m_pID3D11DevContext);
	}
	else
	{
		printf("Requesting highest DirectX 11 feature level available...\n"); 
		//Buscar else mejor nivel de Feature Level DirectX 11 disponible
		if(bUseGPU)
			hr=D3D11CreateDeviceAndSwapChain(pAdapter,D3D_DRIVER_TYPE_UNKNOWN,NULL,0,NULL,0,D3D11_SDK_VERSION,&dscd,&m_pISwapChain,&m_pID3D11Dev,&m_FeatureLevel,&m_pID3D11DevContext);
		else
			hr=D3D11CreateDeviceAndSwapChain(NULL,D3D_DRIVER_TYPE_WARP,NULL,0,NULL,0,D3D11_SDK_VERSION,&dscd,&m_pISwapChain,&m_pID3D11Dev,&m_FeatureLevel,&m_pID3D11DevContext);
	}
	if(FAILED(hr))
	{
		SAFE_RELEASE(pAdapter);
		FlushConsole();
		MessageBox(hWnd,L"Failed to initialize DirectX 11 Device and Swap Chain.\n",L"Error",MB_ICONERROR);
		return false;
	}


	IDXGIDevice * pDXGIDevice;
	hr = m_pID3D11Dev->QueryInterface(IID_IDXGIDevice, (void **)&pDXGIDevice);
      
	IDXGIAdapter * pDXGIAdapter;
	hr = pDXGIDevice->GetParent(IID_IDXGIAdapter, (void **)&pDXGIAdapter);
	pDXGIAdapter->GetDesc(&m_AdapterDesc);
	if(m_pszApplicationTitle)
	{
		TCHAR szTitle[8192];
		TCHAR* pszFeatureLevel=L"Level Unknown";
		switch(m_FeatureLevel)
		{
		case D3D_FEATURE_LEVEL_9_1:
			pszFeatureLevel=L"D3D_FEATURE_LEVEL_9_1";
			break;
		case D3D_FEATURE_LEVEL_9_2:
			pszFeatureLevel=L"D3D_FEATURE_LEVEL_9_2";
			break;
		case D3D_FEATURE_LEVEL_9_3:
			pszFeatureLevel=L"D3D_FEATURE_LEVEL_9_3";
			break;
		case D3D_FEATURE_LEVEL_10_0:
			pszFeatureLevel=L"D3D_FEATURE_LEVEL_10_0";
			break;
		case D3D_FEATURE_LEVEL_10_1:
			pszFeatureLevel=L"D3D_FEATURE_LEVEL_10_1";
			break;
		case D3D_FEATURE_LEVEL_11_0:
			pszFeatureLevel=L"D3D_FEATURE_LEVEL_11_0";
			break;
		case D3D_FEATURE_LEVEL_11_1:
			pszFeatureLevel=L"D3D_FEATURE_LEVEL_11_1";
			break;
		}
		wsprintf(szTitle,L"%s - %s - %s",m_pszApplicationTitle,m_AdapterDesc.Description,pszFeatureLevel);
		SetWindowText(hWnd,szTitle);
	}
	printf("Graphics Adapter and Device information:\n");
	wprintf(TEXT("Description             :%s\n"),m_AdapterDesc.Description);
	wprintf(TEXT("Vendor ID               :0x%x\n"),m_AdapterDesc.VendorId);
	wprintf(TEXT("Revision                :0x%x\n"),m_AdapterDesc.Revision);
	wprintf(TEXT("Device ID               :0x%x\n"),m_AdapterDesc.DeviceId);
	wprintf(TEXT("Dedicated Video Memory  :%8u MB\n"),m_AdapterDesc.DedicatedVideoMemory/(1024*1024));
	wprintf(TEXT("Dedicated System Memory :%8u MB\n"),m_AdapterDesc.DedicatedSystemMemory/(1024*1024));
	wprintf(TEXT("Shared System Memory    :%8u MB\n"),m_AdapterDesc.SharedSystemMemory/(1024*1024));
	SAFE_RELEASE(pDXGIAdapter);
	SAFE_RELEASE(pDXGIDevice);
	PerFeatureLevel();
	FlushConsole();
	while(vecAdapters.size())
	{
		SAFE_RELEASE(vecAdapters.back());
		vecAdapters.pop_back();
	}
	Resize(dscd.BufferDesc.Width,dscd.BufferDesc.Height);

	return true;
}
void CDXGIManager::Resize(int cx,int cy)
{
	HRESULT hr;
	if(m_pISwapChain)
	{
		if(cx && cy)
		{
			m_dscd.BufferDesc.Width=cx;
			m_dscd.BufferDesc.Height=cy;

			SAFE_RELEASE(m_pID3D11PresentRTV);
			SAFE_RELEASE(m_pID3D11PresentSRV);
			SAFE_RELEASE(m_pID3D11PresentBuffer);
			SAFE_RELEASE(m_pID3D11BackBufferRTV);
			SAFE_RELEASE(m_pID3D11BackBuffer);
			SAFE_RELEASE(m_pID3D11SystemMem);
			SAFE_RELEASE(m_pID3D11PresentDSV);
			SAFE_RELEASE(m_pID3D11DepthSRV);
			SAFE_RELEASE(m_pID3D11DepthStencil);
			m_pID3D11DevContext->OMSetRenderTargets(0,0,0);
			m_pID3D11DevContext->ClearState();
			printf("Resize:(%d,%d)\n",cx,cy);
			hr=m_pISwapChain->ResizeBuffers(3,cx,cy,m_dscd.BufferDesc.Format,DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
			if(FAILED(hr))
				printf("%s","Failed to resize buffers\n");
			//Extraer Referencia al Backbuffer
			hr=m_pISwapChain->GetBuffer(0,IID_ID3D11Texture2D,(void**)&m_pID3D11BackBuffer);
			hr=m_pID3D11Dev->CreateRenderTargetView(m_pID3D11BackBuffer,NULL,&m_pID3D11BackBufferRTV);
			//Crear el bloque de memoria de sistema sobre el cual vamos a escribir (Dibujar)
			D3D11_TEXTURE2D_DESC dtd;
			memset(&dtd,0,sizeof(D3D11_TEXTURE2D_DESC));
			dtd.Width=cx;
			dtd.Height=cy;
			dtd.Format=m_dscd.BufferDesc.Format;
			dtd.ArraySize=1;
			dtd.MipLevels=1;
			dtd.CPUAccessFlags=D3D11_CPU_ACCESS_WRITE|D3D11_CPU_ACCESS_READ;
			dtd.Usage=D3D11_USAGE_STAGING;
			dtd.SampleDesc.Count=1;
			hr=m_pID3D11Dev->CreateTexture2D(&dtd,NULL,&m_pID3D11SystemMem);
			if(FAILED(hr))
				printf("%s","Failed to create System Memory Buffer\n");

			//Crear el bloque de memoria de video para resolución multisample o singlesample
			dtd.CPUAccessFlags=0;
			dtd.Usage=D3D11_USAGE_DEFAULT;
			
				
			unsigned int ulMultisampleQualityLevelsSupported=0;
			hr=m_pID3D11Dev->CheckMultisampleQualityLevels(dtd.Format,m_MultiSampleConfig.Count,&ulMultisampleQualityLevelsSupported);
			if(FAILED(hr))
				printf("%s","Failed to check Multisample capabilities\n"); 
		
			if(FAILED(hr) || 0==ulMultisampleQualityLevelsSupported)
			{
				printf("WARNING! Device %S does not support %d samples per pixel and quality level %d render targets\n",m_AdapterDesc.Description,m_MultiSampleConfig.Count,m_MultiSampleConfig.Quality);
				printf("Using 1 sample per pixel and quality level 0\n");
				m_MultiSampleConfig.Count=1;
				m_MultiSampleConfig.Quality=0;
			}
			dtd.SampleDesc=m_MultiSampleConfig;
			if(m_FeatureLevel<=D3D_FEATURE_LEVEL_9_3 && m_dscd.SampleDesc.Count>1 )
				dtd.BindFlags=D3D11_BIND_RENDER_TARGET;
			else
				dtd.BindFlags=D3D11_BIND_RENDER_TARGET|D3D11_BIND_SHADER_RESOURCE;
			
			hr=m_pID3D11Dev->CreateTexture2D(&dtd,NULL,&m_pID3D11PresentBuffer);
			if(FAILED(hr))
				printf("%s","Failed to create MultiSample or SingleSample Present Buffer\n");
			hr=m_pID3D11Dev->CreateShaderResourceView(m_pID3D11PresentBuffer,NULL,&m_pID3D11PresentSRV);
			if(FAILED(hr))
				printf("%s","Failed to create MultiSample or SingleSample Present SRV\n");
			hr=m_pID3D11Dev->CreateRenderTargetView(m_pID3D11PresentBuffer,NULL,&m_pID3D11PresentRTV);
			if(FAILED(hr))
				printf("%s","Failed to create MultiSample or SingleSample Present RTV\n");
			//Crear buffer de profundidad
			dtd.ArraySize=1;
			dtd.BindFlags =D3D11_BIND_DEPTH_STENCIL |((m_FeatureLevel>=D3D_FEATURE_LEVEL_10_1)?D3D11_BIND_SHADER_RESOURCE:0);
			dtd.CPUAccessFlags=0;
			//dtd.Format=DXGI_FORMAT_D24_UNORM_S8_UINT;
			dtd.Format=(m_FeatureLevel>=D3D_FEATURE_LEVEL_10_0)?DXGI_FORMAT_R24G8_TYPELESS:DXGI_FORMAT_D24_UNORM_S8_UINT;

			dtd.Usage=D3D11_USAGE_DEFAULT;
			hr=m_pID3D11Dev->CreateTexture2D(&dtd,NULL,&m_pID3D11DepthStencil);
			if(FAILED(hr))
				printf("%s","Failed to create MultiSample or SingleSample Depth Stencil Texture\n");
			D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
			D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
			memset(&dsvd,0,sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
			memset(&srvd,0,sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
			dsvd.Format=DXGI_FORMAT_D24_UNORM_S8_UINT;
			srvd.Format=DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
			if(m_MultiSampleConfig.Count>1)
			{
				dsvd.ViewDimension=D3D11_DSV_DIMENSION_TEXTURE2DMS;
				srvd.ViewDimension=D3D11_SRV_DIMENSION_TEXTURE2DMS;
			}
			else
			{
				dsvd.ViewDimension=D3D11_DSV_DIMENSION_TEXTURE2D;
				srvd.ViewDimension=D3D11_SRV_DIMENSION_TEXTURE2D;
				dsvd.Texture2D.MipSlice=0;
				srvd.Texture2D.MipLevels=1;
				srvd.Texture2D.MostDetailedMip=0;
			}
			hr=m_pID3D11Dev->CreateDepthStencilView(m_pID3D11DepthStencil,&dsvd,&m_pID3D11PresentDSV);
			if(FAILED(hr))
				printf("%s","Failed to create Multisample or SingleSample Depth Stencil View\n");
			hr=m_pID3D11Dev->CreateShaderResourceView(m_pID3D11DepthStencil,&srvd,&m_pID3D11DepthSRV);
			if(FAILED(hr))
				printf("%s","Failed to create Multisample or SingleSample Depth Shader Resource View\n");
			FlushConsole();
		}
	}
}
void CDXGIManager::PerFeatureLevel()
{
	switch(m_FeatureLevel)
	{
	case D3D_FEATURE_LEVEL_11_0:
		printf("%s","DirectX 11: Feature Level 11.0 Shader Model 5.0 Activated\n");
		m_pszVSProfile="vs_5_0";
		m_pszPSProfile="ps_5_0";
		m_pszCSProfile="cs_5_0";
		break;
	case D3D_FEATURE_LEVEL_10_1:
		printf("%s","DirectX 11: Feature Level 10.1 Shader Model 4.1 Activated\n");
		m_pszVSProfile="vs_4_1";
		m_pszPSProfile="ps_4_1";
		m_pszCSProfile="cs_4_1";

		break;
	case D3D_FEATURE_LEVEL_10_0:
		printf("%s","DirectX 11: Feature Level 10.0 Shader Model 4.0 Activated\n");
		m_pszVSProfile="vs_4_0";
		m_pszPSProfile="ps_4_0";
		m_pszCSProfile="cs_4_0";
		break;
	case D3D_FEATURE_LEVEL_9_3:
		printf("%s","DirectX 11: Feature Level 9.3 Shader Model 4.0 10Level9.3 Activated\n");
		m_pszVSProfile="vs_4_0_level_9_3";
		m_pszPSProfile="ps_4_0_level_9_3";
	
		break;
	case D3D_FEATURE_LEVEL_9_2:
		printf("%s","DirectX 11: Feature Level 9.2 Shader Model 4.0 10Level9.2 Activated\n");
		m_pszVSProfile="vs_4_0_level_9_1";
		m_pszPSProfile="ps_4_0_level_9_1";
		break;
	case D3D_FEATURE_LEVEL_9_1:
		printf("%s","DirectX 11: Feature Level 9.1 Shader Model 4.0 10Level9.1 Activated\n");
		m_pszVSProfile="vs_4_0_level_9_1";
		m_pszPSProfile="ps_4_0_level_9_1";
		break;
	}
	D3D11_FEATURE_DATA_D3D10_X_HARDWARE_OPTIONS hwopts = { 0 } ;
    m_pID3D11Dev->CheckFeatureSupport( D3D11_FEATURE_D3D10_X_HARDWARE_OPTIONS, &hwopts, sizeof(hwopts) );
    if ( !hwopts.ComputeShaders_Plus_RawAndStructuredBuffers_Via_Shader_4_x )
	{
		printf("%s\n","Current device abstraction does not support DirectCompute");
		m_pszCSProfile="Unsupported Profile";
	}
	printf("Activated shader profiles:\nPixel Shader=%s\nVertex Shader=%s\nCompute Shader=%s\n",
		m_pszPSProfile,m_pszVSProfile,m_pszCSProfile);
}

void CDXGIManager::Uninitialize()
{
	if(m_pISwapChain)
	{
		m_pISwapChain->SetFullscreenState(false,0);
	}
	ClearCache();
	if(m_pID3D11DevContext)
	{
		m_pID3D11DevContext->ClearState();
		m_pID3D11DevContext->Flush();
	}
	for(int i=0;i<16;i++)
	{
		SAFE_RELEASE(m_vBlendStates[i]);
		SAFE_RELEASE(m_vDepthStencilStates[i]);
		SAFE_RELEASE(m_vRasterizerStates[i]);
		SAFE_RELEASE(m_vSamplerStates[i]);
	}
	SAFE_RELEASE(m_pID3D11PresentDSV);
	SAFE_RELEASE(m_pID3D11DepthSRV);
	SAFE_RELEASE(m_pID3D11DepthStencil);
	SAFE_RELEASE(m_pID3D11PresentRTV);
	SAFE_RELEASE(m_pID3D11PresentSRV);
	SAFE_RELEASE(m_pID3D11PresentBuffer);
	SAFE_RELEASE(m_pID3D11BackBufferRTV);
	SAFE_RELEASE(m_pID3D11SystemMem);
	SAFE_RELEASE(m_pID3D11BackBuffer);
	SAFE_RELEASE(m_pISwapChain);
	SAFE_RELEASE(m_pID3D11DevContext);
	SAFE_RELEASE(m_pID3D11Dev);
	
}


//CCanvas* CDXGIManager::BeginDraw()
//{
//	if(m_pID3D11SystemMem)
//	{
//		D3D11_MAPPED_SUBRESOURCE map;
//		D3D11_TEXTURE2D_DESC     desc;
//		m_pID3D11SystemMem->GetDesc(&desc);
//		memset(&map,0,sizeof(D3D11_MAPPED_SUBRESOURCE));
//		m_pID3D11DevContext->Map(m_pID3D11SystemMem,0,D3D11_MAP_READ_WRITE,0,&map);
//		CCanvas* pNewCanvas=CCanvas::CreateCanvasFromSharedMemory(map.pData,map.RowPitch,desc.Width,desc.Height);
//		return pNewCanvas;
//	}
//	return NULL;
//}
//void CDXGIManager::EndDraw(CCanvas* pCanvas)
//{
//	if(m_pID3D11SystemMem)
//	{
//		CCanvas::DestroyCanvasFromSharedMemory(pCanvas);
//		m_pID3D11DevContext->Unmap(m_pID3D11SystemMem,0);
//		m_pID3D11DevContext->CopyResource(m_pID3D11BackBuffer,m_pID3D11SystemMem);
//	}
//}


void CDXGIManager::OpenDebugAndInformationConsole(void)
{
	BOOL bOk = AllocConsole(); 
	if (bOk) 
	{ 
		int fd;
		HANDLE hOutput;
		FILE* fp;
		hOutput = GetStdHandle(STD_OUTPUT_HANDLE); 
		fd = _open_osfhandle((intptr_t)hOutput, _O_TEXT); 
		fp = _fdopen( fd, "w" ); 
		*stdout = *fp;
		char *pBuffer=(char*)malloc(32);
		setvbuf( stdout, pBuffer, _IOFBF, 32); 
		SetConsoleTitle(TEXT("@Ware Interactive Foundation Framework - Debug and Information Console")); 
	}
}

void CDXGIManager::FlushConsole(void)
{
	fflush(stdout);
}

bool CDXGIManager::InitCommonStates()
{
	HRESULT hr;
	if(!m_pID3D11Dev)
		return false;
	//Configurar el estado de los Rasterizadores de hardware

	ID3D11RasterizerState *pRS=NULL;
	
	D3D11_RASTERIZER_DESC drd;
	memset(&drd,0,sizeof(D3D11_RASTERIZER_DESC));
	drd.DepthClipEnable=true;
	drd.FrontCounterClockwise=false; //Los polígonos delanteros son aquellos cuyos vértices se disponen en el sentido de las manecillas del reloj
	drd.MultisampleEnable=m_MultiSampleConfig.Count>1;     //Multi muestreo activado si el numero de muestras es mayor a 1
	drd.AntialiasedLineEnable=true;  //Las líneas se muestran con antialiasing (Mediante Alpha Blending)

	//Rasterizador 0 - Back Cull 
	drd.CullMode=D3D11_CULL_BACK;   //No recortar polígonos
	drd.FillMode=D3D11_FILL_SOLID;  //Relleno sólido
	hr=m_pID3D11Dev->CreateRasterizerState(&drd,&pRS);
	
	m_vRasterizerStates[0]=pRS;

	drd.CullMode=D3D11_CULL_FRONT;
	hr=m_pID3D11Dev->CreateRasterizerState(&drd,&pRS);
	
	m_vRasterizerStates[1]=pRS;

	drd.CullMode=D3D11_CULL_NONE;
	hr=m_pID3D11Dev->CreateRasterizerState(&drd,&pRS);

	m_vRasterizerStates[2]=pRS;

	

	//Rasterizador Wireframe
	drd.FillMode=D3D11_FILL_WIREFRAME;
	drd.CullMode=D3D11_CULL_BACK;
	hr=m_pID3D11Dev->CreateRasterizerState(&drd,&pRS);
	
	m_vRasterizerStates[3]=pRS;

	//Rasterizador Wireframe
	drd.FillMode=D3D11_FILL_WIREFRAME;
	drd.CullMode=D3D11_CULL_FRONT;
	hr=m_pID3D11Dev->CreateRasterizerState(&drd,&pRS);
	
	m_vRasterizerStates[4]=pRS;

	//Rasterizador Wireframe
	drd.FillMode=D3D11_FILL_WIREFRAME;
	drd.CullMode=D3D11_CULL_NONE;
	hr=m_pID3D11Dev->CreateRasterizerState(&drd,&pRS);
	
	m_vRasterizerStates[5]=pRS;

	
	//Configurar los samplers que serán utilizados
	ID3D11SamplerState* pSS=NULL;
	D3D11_SAMPLER_DESC dss;
	memset(&dss,0,sizeof(D3D11_SAMPLER_DESC));
	dss.MaxLOD=D3D11_FLOAT32_MAX;  //Less detailed Mip-map
	dss.MinLOD=0.0f;    //Most detailed Mip-map
	dss.MipLODBias=0.0f;
	dss.ComparisonFunc=D3D11_COMPARISON_ALWAYS;
	dss.MaxAnisotropy=1;

	
	//Primer Sampler POINT SAMPLING
	dss.Filter=D3D11_FILTER_MIN_MAG_MIP_POINT; //Point Sampler
	dss.AddressU=dss.AddressV=dss.AddressW=D3D11_TEXTURE_ADDRESS_WRAP; //Textura repetida en U,V y W
	dss.BorderColor[0]=0.5f;
	dss.BorderColor[1]=0.5f;
	dss.BorderColor[2]=0.5f;
	dss.BorderColor[3]=1.0f;
	hr=m_pID3D11Dev->CreateSamplerState(&dss,&pSS);

	m_vSamplerStates[0]=pSS;
	
	//Segundo Sampler LINEAR SAMPLING
	dss.Filter=D3D11_FILTER_MIN_MAG_MIP_LINEAR; //Trilinear Sampler
	hr=m_pID3D11Dev->CreateSamplerState(&dss,&pSS);
	
	m_vSamplerStates[1]=pSS;

	
	//Tercer Sampler ANISOTROPIC   //Con alta calidad de texturización en presencia de pendientes altas con respecto a la profundidad
	dss.Filter=D3D11_FILTER_ANISOTROPIC;
	dss.MaxAnisotropy=m_FeatureLevel>D3D_FEATURE_LEVEL_9_1?4:2;
	hr=m_pID3D11Dev->CreateSamplerState(&dss,&pSS);

	m_vSamplerStates[2]=pSS;

	
	//Cuarto Sampler LINEAR SAMPLING CLAMPED  (Para efectos convolutivos, donde la textura no deberá repetirse durante el muestreo)
	dss.Filter=D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	dss.AddressU=dss.AddressV=dss.AddressW=D3D11_TEXTURE_ADDRESS_CLAMP;  //El texel del borde se repite.
	dss.MaxAnisotropy=1;
	hr=m_pID3D11Dev->CreateSamplerState(&dss,&pSS);
	
	m_vSamplerStates[3]=pSS;



	//Quinto Sampler LINEAR SAMPLING MIRROR
	dss.Filter=D3D11_FILTER_MIN_MAG_MIP_POINT; //Point Sampler
	dss.AddressU=dss.AddressV=dss.AddressW=D3D11_TEXTURE_ADDRESS_MIRROR;
	hr=m_pID3D11Dev->CreateSamplerState(&dss,&pSS);
	
	m_vSamplerStates[4]=pSS;


	//Sexto Sampler LINEAR SAMPLING MIRROR
	dss.Filter=D3D11_FILTER_MIN_MAG_MIP_LINEAR; //Trilinear Sampler
	dss.AddressU=dss.AddressV=dss.AddressW=D3D11_TEXTURE_ADDRESS_MIRROR;
	hr=m_pID3D11Dev->CreateSamplerState(&dss,&pSS);
	
	m_vSamplerStates[5]=pSS;


	//Séptimo Sampler POINT SAMPLING CLAMPED  (Para efectos convolutivos, donde la textura no deberá repetirse durante el muestreo)
	dss.Filter=D3D11_FILTER_MIN_MAG_MIP_POINT;
	dss.AddressU=dss.AddressV=dss.AddressW=D3D11_TEXTURE_ADDRESS_CLAMP;  //El texel del borde se repite.
	hr=m_pID3D11Dev->CreateSamplerState(&dss,&pSS);
	m_vSamplerStates[6]=pSS;


	//El texel del borde se repite.
	dss.Filter=D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	dss.AddressU=dss.AddressV=dss.AddressW=D3D11_TEXTURE_ADDRESS_BORDER;
	dss.MaxAnisotropy=0;
	hr=m_pID3D11Dev->CreateSamplerState(&dss,&pSS);
	m_vSamplerStates[7]=pSS;
	


	//MEZCLADO DE COMBINACIÓN ALPHA Channel

	D3D11_BLEND_DESC bsd;
	ID3D11BlendState* pIBlendState=NULL;
	memset(&bsd,0,sizeof(D3D11_BLEND_DESC));

	//ALPHA BLEND DISABLED
	bsd.AlphaToCoverageEnable=false;
	bsd.IndependentBlendEnable=FALSE;
	bsd.RenderTarget[0].BlendEnable=FALSE;
	bsd.RenderTarget[0].BlendOp=D3D11_BLEND_OP_ADD;
	// Color=ColorSrc*(as) + ColorDest*(1-as)
	bsd.RenderTarget[0].SrcBlend=D3D11_BLEND_SRC_ALPHA;
	bsd.RenderTarget[0].DestBlend=D3D11_BLEND_INV_SRC_ALPHA;
	bsd.RenderTarget[0].BlendOpAlpha=D3D11_BLEND_OP_ADD;
	// Alpha=AlphaSrc*(as) + AlphaDest*(1-as)
	bsd.RenderTarget[0].SrcBlendAlpha=D3D11_BLEND_SRC_ALPHA;
	bsd.RenderTarget[0].DestBlendAlpha=D3D11_BLEND_INV_SRC_ALPHA;
	//Modificar todos los canales (rgb)
	bsd.RenderTarget[0].RenderTargetWriteMask=D3D11_COLOR_WRITE_ENABLE_ALL;
	hr=m_pID3D11Dev->CreateBlendState(&bsd,&pIBlendState);
	
	m_vBlendStates[0]=pIBlendState;
	

	//ALPHA BLEND 1 Enable

	bsd.AlphaToCoverageEnable=false;
	bsd.IndependentBlendEnable=FALSE;
	bsd.RenderTarget[0].BlendEnable=TRUE;
	bsd.RenderTarget[0].BlendOp=D3D11_BLEND_OP_ADD;
	// Color=ColorSrc*(as) + ColorDest*(1-as)
	bsd.RenderTarget[0].SrcBlend=D3D11_BLEND_SRC_ALPHA;
	bsd.RenderTarget[0].DestBlend=D3D11_BLEND_INV_SRC_ALPHA;
	bsd.RenderTarget[0].BlendOpAlpha=D3D11_BLEND_OP_ADD;
	// Alpha=AlphaSrc*(as) + AlphaDest*(1-as)
	bsd.RenderTarget[0].SrcBlendAlpha=D3D11_BLEND_SRC_ALPHA;
	bsd.RenderTarget[0].DestBlendAlpha=D3D11_BLEND_INV_SRC_ALPHA;
	//Modificar todos los canales (rgb)
	bsd.RenderTarget[0].RenderTargetWriteMask=D3D11_COLOR_WRITE_ENABLE_ALL;
	hr=m_pID3D11Dev->CreateBlendState(&bsd,&pIBlendState);
	
	m_vBlendStates[1]=pIBlendState;

	//ALPHA BLEND 2 Alpha to Coverage
	bsd.AlphaToCoverageEnable=true;
	bsd.IndependentBlendEnable=FALSE;
	bsd.RenderTarget[0].BlendEnable=TRUE;
	bsd.RenderTarget[0].BlendOp=D3D11_BLEND_OP_ADD;
	// Color=ColorSrc*(as) + ColorDest*(1-as)
	bsd.RenderTarget[0].SrcBlend=D3D11_BLEND_SRC_ALPHA;
	bsd.RenderTarget[0].DestBlend=D3D11_BLEND_INV_SRC_ALPHA;
	bsd.RenderTarget[0].BlendOpAlpha=D3D11_BLEND_OP_ADD;
	// Alpha=AlphaSrc*(as) + AlphaDest*(1-as)
	bsd.RenderTarget[0].SrcBlendAlpha=D3D11_BLEND_SRC_ALPHA;
	bsd.RenderTarget[0].DestBlendAlpha=D3D11_BLEND_INV_SRC_ALPHA;
	//Modificar todos los canales (rgb)
	bsd.RenderTarget[0].RenderTargetWriteMask=D3D11_COLOR_WRITE_ENABLE_ALL;
	hr=m_pID3D11Dev->CreateBlendState(&bsd,&pIBlendState);
	if(FAILED(hr)) 
	{
		printf("Alpha to Coverage Unsupported\n");
	}
	m_vBlendStates[2]=pIBlendState;	// Configuración del modo de operación del stencil y del buffer de profundidad.
	
	
	//MERGE BLEND 3
	bsd.AlphaToCoverageEnable=false;
	bsd.IndependentBlendEnable=FALSE;
	bsd.RenderTarget[0].BlendEnable=TRUE;
	bsd.RenderTarget[0].BlendOp=D3D11_BLEND_OP_ADD;
	// Color=ColorSrc*(1) + ColorDest*(1)
	bsd.RenderTarget[0].SrcBlend=D3D11_BLEND_ONE;
	bsd.RenderTarget[0].DestBlend=D3D11_BLEND_ONE;
	bsd.RenderTarget[0].BlendOpAlpha=D3D11_BLEND_OP_ADD;
	// Alpha=AlphaSrc*(1) + AlphaDest*(1)
	bsd.RenderTarget[0].SrcBlendAlpha=D3D11_BLEND_ONE;
	bsd.RenderTarget[0].DestBlendAlpha=D3D11_BLEND_ONE;
	//Modificar todos los canales (rgb)
	bsd.RenderTarget[0].RenderTargetWriteMask=D3D11_COLOR_WRITE_ENABLE_ALL;
	hr=m_pID3D11Dev->CreateBlendState(&bsd,&pIBlendState);
	if(FAILED(hr)) 
	{
		printf("Warning Color Merge Unsupported\n");
	}
	m_vBlendStates[3]=pIBlendState;	// Configuración del modo de operación del stencil y del buffer de profundidad.
	
	

	D3D11_DEPTH_STENCIL_DESC dsd;
	ID3D11DepthStencilState* pID3D11DepthStencilState=0;
	memset(&dsd,0,sizeof(D3D11_DEPTH_STENCIL_DESC));
	dsd.DepthEnable = true;
	dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsd.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	dsd.StencilEnable = true;
	dsd.StencilReadMask = 0xFF;
	dsd.StencilWriteMask = 0xFF;
	// Stencil operations if pixel is front-facing.
	dsd.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	dsd.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	// Stencil operations if pixel is back-facing.
	dsd.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	dsd.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	hr=m_pID3D11Dev->CreateDepthStencilState(&dsd,&pID3D11DepthStencilState);
	
	m_vDepthStencilStates[0]=pID3D11DepthStencilState;
	

	memset(&dsd,0,sizeof(D3D11_DEPTH_STENCIL_DESC));	
	dsd.DepthEnable = true;
	dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsd.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	dsd.StencilEnable = true;
	dsd.StencilReadMask = 0xFF;
	dsd.StencilWriteMask = 0xFF;
	// Stencil operations if pixel is front-facing.
	dsd.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsd.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	// Stencil operations if pixel is back-facing.
	dsd.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsd.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	hr=m_pID3D11Dev->CreateDepthStencilState(&dsd,&pID3D11DepthStencilState);
	
	m_vDepthStencilStates[1]=pID3D11DepthStencilState;
	
	return true;
}

void CDXGIManager::CachedDrawIndexed(void * pVertices,unsigned long ulVertexStride,unsigned long  ulVerticesCount,unsigned long* pIndices,unsigned long ulIndicesCount,unsigned long ulStartVertexLocation,unsigned long ulStartIndexLocation,unsigned long ulIndicesPrimitiveCount)
{
	ID3D11Buffer* pVB=0,*pIB=0;
	//Buscar dentro del cache de buffers para verificar la existencia de recursos en GPU/CPU
	auto itCachedVertexBuffer=m_mapBufferCache.find(pVertices);
	auto itCachedIndexBuffer=m_mapBufferCache.find(pIndices);
	if(itCachedVertexBuffer!=m_mapBufferCache.end())
	{
		//Usar buffer preasignado
		pVB=itCachedVertexBuffer->second;
	}
	else
	{
		//Crear recursos en abstracción de dispositivo una vez.
		D3D11_BUFFER_DESC bd;
		HRESULT hr=E_FAIL;
		memset(&bd,0,sizeof(D3D11_BUFFER_DESC));
		bd.BindFlags=D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags=D3D11_CPU_ACCESS_WRITE;
		bd.ByteWidth=ulVerticesCount*ulVertexStride;
		bd.Usage=D3D11_USAGE_DYNAMIC;
		D3D11_SUBRESOURCE_DATA sd;
		sd.pSysMem=pVertices;
		sd.SysMemPitch=0;
		sd.SysMemSlicePitch=0;
		GetDevice()->CreateBuffer(&bd,&sd,&pVB);
		m_mapBufferCache.insert(make_pair(pVertices,pVB));
	}
	if(itCachedIndexBuffer!=m_mapBufferCache.end())
	{
		pIB=itCachedIndexBuffer->second;	
	}
	else
	{
		D3D11_BUFFER_DESC bd;
		HRESULT hr=E_FAIL;
		memset(&bd,0,sizeof(D3D11_BUFFER_DESC));
		bd.BindFlags=D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags=D3D11_CPU_ACCESS_WRITE;
		bd.ByteWidth=sizeof(unsigned long)*ulIndicesCount;
		bd.Usage=D3D11_USAGE_DYNAMIC;
		D3D11_SUBRESOURCE_DATA sd;
		sd.pSysMem=pIndices;
		sd.SysMemPitch=0;
		sd.SysMemSlicePitch=0;
		hr=GetDevice()->CreateBuffer(&bd,&sd,&pIB);
		m_mapBufferCache.insert(make_pair(pIndices,pIB));
	}
	ID3D11DeviceContext* pCtx=GetContext();
	unsigned int Zero=0;
	unsigned int Stride=ulVertexStride;
	pCtx->IASetVertexBuffers(0,1,&pVB,&Stride,&Zero);
	pCtx->IASetIndexBuffer(pIB,DXGI_FORMAT_R32_UINT,0);
	pCtx->DrawIndexed(ulIndicesPrimitiveCount,ulStartIndexLocation,ulStartVertexLocation);
}
void CDXGIManager::CachedDrawIndexed16(void * pVertices,unsigned long ulVertexStride,unsigned long  ulVerticesCount,unsigned short* pIndices,unsigned long ulIndicesCount,unsigned long ulStartVertexLocation,unsigned long ulStartIndexLocation,unsigned long ulIndicesPrimitiveCount)
{
	ID3D11Buffer* pVB=0,*pIB=0;
	//Buscar dentro del cache de buffers para verificar la existencia de recursos en GPU/CPU
	auto itCachedVertexBuffer=m_mapBufferCache.find(pVertices);
	auto itCachedIndexBuffer=m_mapBufferCache.find(pIndices);
	if(itCachedVertexBuffer!=m_mapBufferCache.end())
	{
		//Usar buffer preasignado
		pVB=itCachedVertexBuffer->second;
	}
	else
	{
		//Crear recursos en abstracción de dispositivo una vez.
		D3D11_BUFFER_DESC bd;
		HRESULT hr=E_FAIL;
		memset(&bd,0,sizeof(D3D11_BUFFER_DESC));
		bd.BindFlags=D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags=D3D11_CPU_ACCESS_WRITE;
		bd.ByteWidth=ulVerticesCount*ulVertexStride;
		bd.Usage=D3D11_USAGE_DYNAMIC;
		D3D11_SUBRESOURCE_DATA sd;
		sd.pSysMem=pVertices;
		sd.SysMemPitch=0;
		sd.SysMemSlicePitch=0;
		GetDevice()->CreateBuffer(&bd,&sd,&pVB);
		m_mapBufferCache.insert(make_pair(pVertices,pVB));
	}
	if(itCachedIndexBuffer!=m_mapBufferCache.end())
	{
		pIB=itCachedIndexBuffer->second;	
	}
	else
	{
		D3D11_BUFFER_DESC bd;
		HRESULT hr=E_FAIL;
		memset(&bd,0,sizeof(D3D11_BUFFER_DESC));
		bd.BindFlags=D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags=D3D11_CPU_ACCESS_WRITE;
		bd.ByteWidth=sizeof(unsigned short)*ulIndicesCount;
		bd.Usage=D3D11_USAGE_DYNAMIC;
		D3D11_SUBRESOURCE_DATA sd;
		sd.pSysMem=pIndices;
		sd.SysMemPitch=0;
		sd.SysMemSlicePitch=0;
		hr=GetDevice()->CreateBuffer(&bd,&sd,&pIB);
		m_mapBufferCache.insert(make_pair(pIndices,pIB));
	}
	ID3D11DeviceContext* pCtx=GetContext();
	unsigned int Zero=0;
	unsigned int Stride=ulVertexStride;
	pCtx->IASetVertexBuffers(0,1,&pVB,&Stride,&Zero);
	pCtx->IASetIndexBuffer(pIB,DXGI_FORMAT_R16_UINT,0);
	pCtx->DrawIndexed(ulIndicesPrimitiveCount,ulStartIndexLocation,ulStartVertexLocation);
}

void CDXGIManager::CachedDraw(void * pVertices,unsigned long ulVertexStride,unsigned long ulVerticesCount,unsigned long ulStartVertexLocation)
{
	ID3D11Buffer* pVB=0;
	auto itCachedVertexBuffer=m_mapBufferCache.find(pVertices);
	if(itCachedVertexBuffer!=m_mapBufferCache.end())
	{
		//Usar buffer preasignado
		pVB=itCachedVertexBuffer->second;
	}
	else
	{
		//Crear recursos en abstracción de dispositivo una vez.
		D3D11_BUFFER_DESC bd;
		HRESULT hr=E_FAIL;
		memset(&bd,0,sizeof(D3D11_BUFFER_DESC));
		bd.BindFlags=D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags=D3D11_CPU_ACCESS_WRITE;
		bd.ByteWidth=ulVerticesCount*ulVertexStride;
		bd.Usage=D3D11_USAGE_DYNAMIC;
		D3D11_SUBRESOURCE_DATA sd;
		sd.pSysMem=pVertices;
		sd.SysMemPitch=0;
		sd.SysMemSlicePitch=0;
		GetDevice()->CreateBuffer(&bd,&sd,&pVB);
		m_mapBufferCache.insert(make_pair(pVertices,pVB));
	}
	ID3D11DeviceContext* pCtx=GetContext();
	unsigned int Zero=0;
	unsigned int Stride=ulVertexStride;
	pCtx->IASetVertexBuffers(0,1,&pVB,&Stride,&Zero);
	pCtx->Draw(ulVerticesCount,ulStartVertexLocation);
}
void CDXGIManager::InvalidateBuffer(void* pBuffer)
{
	auto it=m_mapBufferCache.find(pBuffer);
	if(it!=m_mapBufferCache.end())
	{
		SAFE_RELEASE(it->second);
		m_mapBufferCache.erase(it);
	}
}
void CDXGIManager::ClearCache(void)
{
	for(auto it=m_mapBufferCache.begin();it!=m_mapBufferCache.end();it++)
		SAFE_RELEASE(it->second);
	m_mapBufferCache.clear();
}


//LoadTexture by Cornejo: returns a ID3D11Texture2D bindable as Render Target View and Shader Resorce View
ID3D11Texture2D* CDXGIManager::LoadTexture(
	char* pszFileName,		//The file name
	unsigned long MipMapLevels, //Number of mipmaps to generate, -1 automatic (int)log2(min(sizeX,sizeY))+1 levels
	ID3D11ShaderResourceView** ppSRV, //The Shader Resource View
	float (*pAlphaFunction)(float r,float g,float b),  //Operates pixel's rgb channels to build an alpha channel (normalized), can be null
	VECTOR4D (*pColorFunction)(VECTOR4D& Color))
{
	ID3D11Device* pDev=GetDevice();
	ID3D11DeviceContext* pCtx=GetContext();
	printf("Loading %s...\n",pszFileName);
	fstream bitmap(pszFileName,ios::in|ios::binary);
	if(!bitmap.is_open())
	{
		printf("Error: Unable to open file %s\n",pszFileName);
		return NULL;
	}
	//Verificar el numeo magico de un bmp
	BITMAPFILEHEADER bfh;
	bitmap.read((char*)&bfh.bfType,sizeof(bfh.bfType));
	if(!(bfh.bfType=='MB'))
	{
		printf("Error: Not a DIB File\n");
		return NULL;
	}
	//Leerel resto de los datos
	bitmap.read((char*)&bfh.bfSize,sizeof(bfh)-sizeof(bfh.bfType));

	BITMAPINFOHEADER bih;
	bitmap.read((char*)&bih.biSize,sizeof(bih.biSize));
	if(bih.biSize!=sizeof(BITMAPINFOHEADER))
	{
		printf("Error: Unsupported DIB file format.");
		return NULL;
	}
	bitmap.read((char*)&bih.biWidth,sizeof(bih)-sizeof(bih.biSize));
	RGBQUAD Palette[256];
	unsigned long ulRowlenght=0;
	unsigned char* pBuffer=NULL;

	ID3D11Texture2D* pTemp; //CPU memory
	D3D11_TEXTURE2D_DESC dtd;
	memset(&dtd,0,sizeof(D3D11_TEXTURE2D_DESC));
	dtd.Format=DXGI_FORMAT_R8G8B8A8_UNORM;
	dtd.ArraySize=1;
	dtd.BindFlags=0;
	dtd.CPUAccessFlags=D3D11_CPU_ACCESS_READ|D3D11_CPU_ACCESS_WRITE;
	dtd.Height=bih.biHeight;
	dtd.Width=bih.biWidth;
	dtd.Usage=D3D11_USAGE_STAGING;
	dtd.MipLevels=min(MipMapLevels,1+(unsigned long)floor(log(min(dtd.Width,dtd.Height))/log(2)));
	dtd.SampleDesc.Count=1;
	dtd.SampleDesc.Quality=0;

	printf("Width %d, Height:%d, %dbpp\n",bih.biWidth,bih.biHeight,bih.biBitCount);
	fflush(stdout);
	pDev->CreateTexture2D(&dtd,NULL,&pTemp);
	struct PIXEL
	{
		unsigned char r,g,b,a;
	};
	D3D11_MAPPED_SUBRESOURCE ms;
	pCtx->Map(pTemp,0,D3D11_MAP_READ_WRITE,0,&ms);
	char *pDestStart=(char*)ms.pData+(bih.biHeight-1)*ms.RowPitch;
	PIXEL *pDest=(PIXEL*)pDestStart;
	switch(bih.biBitCount)
	{
	case 1: //Tarea 1bpp 2 colores
		if(bih.biClrUsed==0)//Si se usan todos los colores, ese miembro es cero
			bitmap.read((char*)Palette, 2*sizeof(RGBQUAD));
		else
			bitmap.read((char*)Palette, bih.biClrUsed*sizeof(RGBQUAD));
		ulRowlenght=4*((bih.biBitCount*bih.biWidth + 31)/32);
		pBuffer= new unsigned char[ulRowlenght];
		for(int y=(bih.biHeight-1);y>=0; y--)
		{
			bitmap.read((char*)pBuffer,ulRowlenght);
			int x=0;
			for(unsigned long iByte=0;iByte<ulRowlenght;iByte++)
			{
				unsigned long iColorIndex;
				unsigned char c=pBuffer[iByte];
				for(int iBit=0;iBit<8;iBit++)
				{
					iColorIndex=((c & 0x80)!=0);
					c<<=1;
					pDest->r=Palette[iColorIndex].rgbRed;
					pDest->g=Palette[iColorIndex].rgbGreen;
					pDest->b=Palette[iColorIndex].rgbBlue;	
					if(pAlphaFunction)
						pDest->a=(unsigned char)max(0,min(255,(int)(255*pAlphaFunction(pDest->r/255.0f,pDest->g/255.0f,pDest->b/255.0f))));
					else
						pDest->a=0xff;
					if(pColorFunction)
					{
						VECTOR4D Color(pDest->r*(1.0f/255),pDest->g*(1.0f/255),pDest->b*(1.0f/255),pDest->a*(1.0f/255));
						VECTOR4D Result=pColorFunction(Color);
						pDest->r=(unsigned char)max(0,min(255,(int)(Result.r*255)));
						pDest->g=(unsigned char)max(0,min(255,(int)(Result.g*255)));
						pDest->b=(unsigned char)max(0,min(255,(int)(Result.b*255)));
						pDest->a=(unsigned char)max(0,min(255,(int)(Result.a*255)));
					}
					x++;
					pDest++;
					if(x<bih.biWidth)
						continue;		
				}
			}
			pDestStart-=ms.RowPitch;
			pDest=(PIXEL*)pDestStart;
		}
		delete [] pBuffer;
		break;
	case 4: //Aquí 4 bpp 16 colores
		if(bih.biClrUsed==0)//Si se usan todos los colores, ese miembro es cero
			bitmap.read((char*)Palette, 16*sizeof(RGBQUAD));
		else
			bitmap.read((char*)Palette, bih.biClrUsed*sizeof(RGBQUAD));
		//Leer el bitmap
		ulRowlenght=4*((bih.biBitCount*bih.biWidth +31)/32);
		pBuffer= new unsigned char[ulRowlenght];
		for(int y=(bih.biHeight-1);y>=0; y--)
		{
			bitmap.read((char*)pBuffer,ulRowlenght);
			for(int x=0;x<bih.biWidth;x++)
			{
				//Desempacar pixeles así
				unsigned char nibble=(x&1)?(pBuffer[x>>1]&0x0f):(pBuffer[x>>1]>>4);
				pDest->r=Palette[nibble].rgbRed;
				pDest->b=Palette[nibble].rgbBlue;
				pDest->g=Palette[nibble].rgbGreen;
				if(pAlphaFunction)
					pDest->a=(unsigned char)max(0,min(255,(int)(255*pAlphaFunction(pDest->r/255.0f,pDest->g/255.0f,pDest->b/255.0f))));
				else
					pDest->a=0xff;
				if(pColorFunction)
				{
					VECTOR4D Color(pDest->r*(1.0f/255),pDest->g*(1.0f/255),pDest->b*(1.0f/255),pDest->a*(1.0f/255));
					VECTOR4D Result=pColorFunction(Color);
					pDest->r=(unsigned char)max(0,min(255,(int)(Result.r*255)));
					pDest->g=(unsigned char)max(0,min(255,(int)(Result.g*255)));
					pDest->b=(unsigned char)max(0,min(255,(int)(Result.b*255)));
					pDest->a=(unsigned char)max(0,min(255,(int)(Result.a*255)));
				}
				pDest++;
			}
			pDestStart-=ms.RowPitch;
			pDest=(PIXEL*)pDestStart;
		}
		delete [] pBuffer;
		break;
	case 8: //Tarea 8 bpp 256 colores
		if(bih.biClrUsed==0)//Si se usan todos los colores, ese miembro es cero
			bitmap.read((char*)Palette, 256*sizeof(RGBQUAD));
		else
			bitmap.read((char*)Palette, bih.biClrUsed*sizeof(RGBQUAD));

		ulRowlenght=4*((bih.biBitCount*bih.biWidth +31)/32);
		pBuffer= new unsigned char[ulRowlenght];

		for(int y=(bih.biHeight-1);y>=0; y--)
		{
			bitmap.read((char*)pBuffer,ulRowlenght);
			for(int x=0;x<bih.biWidth;x++)
			{
				//Desempacar pixeles así
				unsigned char nibble=(x&1)?(pBuffer[x]&0xff):(pBuffer[x]);
				pDest->r=Palette[nibble].rgbRed;
				pDest->b=Palette[nibble].rgbBlue;
				pDest->g=Palette[nibble].rgbGreen;
				if(pAlphaFunction)
					pDest->a=(unsigned char)max(0,min(255,(int)(255*pAlphaFunction(pDest->r/255.0f,pDest->g/255.0f,pDest->b/255.0f))));
				else
					pDest->a=0xff;
				if(pColorFunction)
				{
					VECTOR4D Color(pDest->r*(1.0f/255),pDest->g*(1.0f/255),pDest->b*(1.0f/255),pDest->a*(1.0f/255));
					VECTOR4D Result=pColorFunction(Color);
					pDest->r=(unsigned char)max(0,min(255,(int)(Result.r*255)));
					pDest->g=(unsigned char)max(0,min(255,(int)(Result.g*255)));
					pDest->b=(unsigned char)max(0,min(255,(int)(Result.b*255)));
					pDest->a=(unsigned char)max(0,min(255,(int)(Result.a*255)));
				}
				pDest++;
			}
			pDestStart-=ms.RowPitch;
			pDest=(PIXEL*)pDestStart;
		}
		delete [] pBuffer;
		break;
	case 24://Aquí 16777216 colores (True Color)
		//Leer el bitmap
		ulRowlenght=4*((bih.biBitCount*bih.biWidth +31)/32);
		pBuffer= new unsigned char[ulRowlenght];
		for(int y=(bih.biHeight-1);y>=0; y--)
		{
			bitmap.read((char*)pBuffer,ulRowlenght);
			for(int x=0;x<bih.biWidth;x++)
			{
				//Desempacar pixeles así
				pDest->b=pBuffer[3*x+0];
				pDest->g=pBuffer[3*x+1];
				pDest->r=pBuffer[3*x+2];
				if(pAlphaFunction)
					pDest->a=(unsigned char)max(0,min(255,(int)(255*pAlphaFunction(pDest->r/255.0f,pDest->g/255.0f,pDest->b/255.0f))));
				else
					pDest->a=0xff;
				if(pColorFunction)
				{
					VECTOR4D Color(pDest->r*(1.0f/255),pDest->g*(1.0f/255),pDest->b*(1.0f/255),pDest->a*(1.0f/255));
					VECTOR4D Result=pColorFunction(Color);
					pDest->r=(unsigned char)max(0,min(255,(int)(Result.r*255)));
					pDest->g=(unsigned char)max(0,min(255,(int)(Result.g*255)));
					pDest->b=(unsigned char)max(0,min(255,(int)(Result.b*255)));
					pDest->a=(unsigned char)max(0,min(255,(int)(Result.a*255)));
				}
				pDest++;
			}
			pDestStart-=ms.RowPitch;
			pDest=(PIXEL*)pDestStart;
		}
		delete [] pBuffer;
		break;
	case 32:
		ulRowlenght=4*((bih.biBitCount*bih.biWidth +31)/32);
		pBuffer= new unsigned char[ulRowlenght];
		for(int y=(bih.biHeight-1);y>=0; y--)
		{
			bitmap.read((char*)pBuffer,ulRowlenght);
			for(int x=0;x<bih.biWidth;x++)
			{
				//Desempacar pixeles así
				pDest->b=pBuffer[4*x+0];
				pDest->g=pBuffer[4*x+1];
				pDest->r=pBuffer[4*x+2];
				if(pAlphaFunction)
					pDest->a=(unsigned char)max(0,min(255,(int)(255*pAlphaFunction(pDest->r/255.0f,pDest->g/255.0f,pDest->b/255.0f))));
				else
					pDest->a=0xff;
				if(pColorFunction)
				{
					VECTOR4D Color(pDest->r*(1.0f/255),pDest->g*(1.0f/255),pDest->b*(1.0f/255),pDest->a*(1.0f/255));
					VECTOR4D Result=pColorFunction(Color);
					pDest->r=(unsigned char)max(0,min(255,(int)(Result.r*255)));
					pDest->g=(unsigned char)max(0,min(255,(int)(Result.g*255)));
					pDest->b=(unsigned char)max(0,min(255,(int)(Result.b*255)));
					pDest->a=(unsigned char)max(0,min(255,(int)(Result.a*255)));
				}
				pDest++;
			}
			pDestStart-=ms.RowPitch;
			pDest=(PIXEL*)pDestStart;
		}
		delete [] pBuffer;
		break;
	}
	//transfer cpu mem to gpu memory
	pCtx->Unmap(pTemp,0);
	//Crear buffer en GPU
	dtd.BindFlags=D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_RENDER_TARGET;
	dtd.MiscFlags=D3D11_RESOURCE_MISC_GENERATE_MIPS;
	dtd.Usage=D3D11_USAGE_DEFAULT;
	dtd.CPUAccessFlags=0;
	ID3D11Texture2D* pTexture;
	pDev->CreateTexture2D(&dtd,NULL,&pTexture);
	//copy gpu mem to gpu mem for RW capable surface
	pCtx->CopyResource(pTexture,pTemp);
	if(ppSRV)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
		srvd.Texture2D.MipLevels=dtd.MipLevels;
		srvd.Texture2D.MostDetailedMip=0;
		srvd.Format=dtd.Format;
		srvd.ViewDimension=D3D11_SRV_DIMENSION_TEXTURE2D;
		pDev->CreateShaderResourceView(pTexture,&srvd,ppSRV);
		printf("Generating %d mipmaps levels... ",dtd.MipLevels);
		fflush(stdout);
		pCtx->GenerateMips(*ppSRV);
		printf("done.\n");
		fflush(stdout);
	}
	SAFE_RELEASE(pTemp);
	printf("Load success.\n");
	return pTexture;
}