#pragma once
#include <Windows.h>
#include <DXGI.h>
#include <D3D11.h>
#include <map>
#include "Matrix4D.h"
using namespace std;

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x) {if((x)){ (x)->Release();(x)=NULL;}}
#endif


class CCanvas;

class CDXGIManager
{
public:
	//Descriptor de valores iniciales de la cadena de superficies
	DXGI_SWAP_CHAIN_DESC m_dscd;
	TCHAR* m_pszApplicationTitle;
protected:
	//Permitirá organizar la cadena de superficies
	IDXGISwapChain* m_pISwapChain;
	//Device
	ID3D11Device*   m_pID3D11Dev;
	//Contexto inmediato de Direct3D
	ID3D11DeviceContext* m_pID3D11DevContext;
	const char* m_pszVSProfile;
	const char* m_pszPSProfile;
	const char* m_pszCSProfile;
public:
	//Feature Level Actual
	D3D_FEATURE_LEVEL m_FeatureLevel;
	//Configuración de calidad y número de muestras por pixel
	DXGI_SAMPLE_DESC m_MultiSampleConfig;
protected:
	//Referencia al Backbuffer
	ID3D11Texture2D* m_pID3D11BackBuffer;
	//Referencia al DepthStencil (BufferZ)
	ID3D11Texture2D* m_pID3D11DepthStencil;
	ID3D11DepthStencilView* m_pID3D11PresentDSV;
	ID3D11ShaderResourceView* m_pID3D11DepthSRV;
	//Vista de Objetivo de Presentación del BackBuffer
	ID3D11RenderTargetView* m_pID3D11BackBufferRTV;
	//Textura de síntesis multisample o singlesample
	ID3D11Texture2D*		  m_pID3D11PresentBuffer;
	ID3D11ShaderResourceView* m_pID3D11PresentSRV;
	ID3D11RenderTargetView*   m_pID3D11PresentRTV;
	
	//Memoria de Sistema para transferencias y propósito general
	ID3D11Texture2D* m_pID3D11SystemMem;
	virtual void PerFeatureLevel();

	//Samplers
	ID3D11SamplerState*			m_vSamplerStates[16];
	ID3D11RasterizerState*		m_vRasterizerStates[16];
	ID3D11DepthStencilState*	m_vDepthStencilStates[16];
	ID3D11BlendState*			m_vBlendStates[16];
	
	//Registro de cache de buffers ID3D11Buffer*
	map<void*,ID3D11Buffer*>	m_mapBufferCache;

public:
	DXGI_ADAPTER_DESC m_AdapterDesc;
public:
	CDXGIManager(void);
	~CDXGIManager(void);
	virtual bool				InitializeSwapChain(HWND hWnd,bool bFullScreen=false,bool bUseGPU=true,bool bEnableAntialiasing=false,unsigned int uiNumberOfSamplesPerPixel=4,unsigned int uiSampleQuality=0,unsigned int uiFeatureLevel=0);
	virtual bool				InitCommonStates(void);
	virtual void				Uninitialize(void);
	virtual void				Resize(int cx,int cy);
	CCanvas*					BeginDraw();
	void						EndDraw(CCanvas* pCanvas);
	ID3D11Device*				GetDevice(void){return m_pID3D11Dev;};
	ID3D11DeviceContext*		GetContext(void){return m_pID3D11DevContext;};
	IDXGISwapChain*				GetSwapChain(void){return m_pISwapChain;};
	ID3D11Texture2D*			GetBackBuffer(void){return m_pID3D11BackBuffer;}
	ID3D11Texture2D*			GetSystemBuffer(void){return m_pID3D11SystemMem;};
	ID3D11Texture2D*			GetPresentBuffer(void){return m_pID3D11PresentBuffer;};
	ID3D11Texture2D*			GetPresentDepthStencil(void){return m_pID3D11DepthStencil;};
	ID3D11RenderTargetView*&	GetBackBufferRTV(void){return m_pID3D11BackBufferRTV;};
	ID3D11ShaderResourceView*&	GetPresentSRV(void){return m_pID3D11PresentSRV;};
	ID3D11RenderTargetView*&	GetPresentRTV(void){return m_pID3D11PresentRTV;};
	ID3D11DepthStencilView*&	GetPresentDSV(void){return m_pID3D11PresentDSV;};
	ID3D11ShaderResourceView*&	GetDepthSRV(void){return m_pID3D11DepthSRV;};

	ID3D11SamplerState*&		GetSamplerState(int nIndex){return m_vSamplerStates[nIndex];}
	ID3D11RasterizerState*&		GetRasterizerState(int nIndex){return m_vRasterizerStates[nIndex];}
	ID3D11DepthStencilState*&	GetDepthStencilState(int nIndex){return m_vDepthStencilStates[nIndex];}
	ID3D11BlendState*&			GetBlendState(int nIndex){return m_vBlendStates[nIndex];}
	ID3D11Texture2D* CDXGIManager::LoadTexture(
		char* pszFileName,		//The file name
		unsigned long MipMapLevels, //Number of mipmaps to generate, -1 automatic (int)log2(min(sizeX,sizeY))+1 levels
		ID3D11ShaderResourceView** ppSRV, //The Shader Resource View
		float (*pAlphaFunction)(float r,float g,float b)=0,
		VECTOR4D(*pColorFunction)(VECTOR4D& Input)=0);
	//Funciones de trazo con cacheo de recursos en abstracción de dispositivos (Reduciendo transferencias innecesarias).
	//Cuando el recurso es dibujado por primera vez, se crea un buffer en abstracción de dispositivo (CPU o GPU Context) y se reutiliza en futuras invocaciones.
	//con el objetivo de amortiguar asignación de recursos en CPU/GPU de manera redundante.
	void						CachedDrawIndexed(void * pVertices,unsigned long ulVertexStride,unsigned long  ulVerticesCount,unsigned long* pIndices,unsigned long ulIndicesCount,unsigned long ulStartVertexLocation,unsigned long ulStartIndexLocation,unsigned long ulIndicesPrimitiveCount);
	void						CachedDrawIndexed16(void * pVertices,unsigned long ulVertexStride,unsigned long  ulVerticesCount,unsigned short* pIndices,unsigned long ulIndicesCount,unsigned long ulStartVertexLocation,unsigned long ulStartIndexLocation,unsigned long ulIndicesPrimitiveCount);
	
	void						CachedDraw(void * pVertices,unsigned long ulVertexStride,unsigned long  ulVerticesCount,unsigned long ulStartVertexLocation);
	//Remueve un buffer de vértices o índices del cache
	void						InvalidateBuffer(void* pBuffer);
	//Limpiar el caché de buffers de vértices e indices
	void						ClearCache(void);
	//Crea una consola y conecta el flujo de salida estándar con ésta consola. (Permite el uso de cout y printf para notificar sucesos y errores).
	void						OpenDebugAndInformationConsole(void);
	//Copia el buffer de salida a la consola y vacía el buffer de salida. 
	void						FlushConsole(void);

	const char* GetVSProfile(){return m_pszVSProfile;}
	const char* GetPSProfile(){return m_pszPSProfile;}
	const char* GetCSProfile(){return m_pszCSProfile;}

};

