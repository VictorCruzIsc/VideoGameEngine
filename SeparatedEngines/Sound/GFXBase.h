#pragma once
#include <d3d11.h>
#include "DXGIManager.h"
#include "ObjectAttributes.h"
class CGFXBase:
	public CObjectAttributes
{
protected:
	static bool CompileShader(LPCWSTR pszHLSLFileName,LPCSTR pszProfile, LPCSTR pszEntryPoint,ID3D10Blob** ppBlobCode);
	static bool CompileVertexShader(CDXGIManager* pManager, LPCWSTR pszVertexShaderFile,LPCSTR pszEntryPoint,ID3D10Blob** ppBlobCode,ID3D11VertexShader** ppOutVS);
	static bool CompilePixelShader(CDXGIManager* pManager, LPCWSTR pszPixelShaderFile,LPCSTR pszEntryPoint,ID3D10Blob** ppBlobCode,ID3D11PixelShader** ppOutPS);
	static bool CompileComputeShader(CDXGIManager* pManager, LPCWSTR pszPixelShaderFile,LPCSTR pszEntryPoint,ID3D10Blob** ppBlobCode,ID3D11ComputeShader** ppOutCS);
	CDXGIManager* m_pManager;
	TCHAR* m_pszHLSLFileName;
public:
	CGFXBase(CDXGIManager* pManager);
	virtual bool Initialize()=0;
	virtual void Uninitialize()=0;
	virtual void Resize(int cx,int cy){};
	virtual void Pass(int nPass)=0;
	virtual int  PassCount()=0;
	virtual ~CGFXBase(void);
};

