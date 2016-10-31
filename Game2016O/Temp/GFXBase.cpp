#include "stdafx.h"
#include "GFXBase.h"
#include <d3dcompiler.h>
CGFXBase::CGFXBase(CDXGIManager* pManager)
{
	m_pManager=pManager;
	m_pszHLSLFileName=L"";
}
CGFXBase::~CGFXBase(void)
{
}

bool CGFXBase::CompileShader(LPCWSTR pszHLSLFileName,LPCSTR pszProfile, LPCSTR pszEntryPoint,ID3D10Blob** ppBlobCode)
{
	HRESULT hr=E_FAIL;
	wprintf(L"Compiling:%s Entry point:%S...\n",pszHLSLFileName,pszEntryPoint);
	fflush(stdout);
	*ppBlobCode=NULL;
#ifdef _DEBUG
	DWORD dwCompileOptions=D3DCOMPILE_ENABLE_STRICTNESS|D3DCOMPILE_SKIP_OPTIMIZATION|D3DCOMPILE_DEBUG;
	dwCompileOptions=D3DCOMPILE_ENABLE_STRICTNESS|D3DCOMPILE_OPTIMIZATION_LEVEL3;
#else
	DWORD dwCompileOptions=D3DCOMPILE_ENABLE_STRICTNESS|D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif
	ID3D10Blob* pErrorMsgs=NULL;
	//Invocación al compilador externo de DirectX 11
	hr=D3DCompileFromFile(pszHLSLFileName,NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE ,pszEntryPoint,pszProfile,dwCompileOptions,0,ppBlobCode,&pErrorMsgs);
	if(pErrorMsgs)
	{
		char * pszBuffer=new char[pErrorMsgs->GetBufferSize()+1];
		memset(pszBuffer,0,pErrorMsgs->GetBufferSize()+1);
		memcpy(pszBuffer,pErrorMsgs->GetBufferPointer(),pErrorMsgs->GetBufferSize());
		printf("%s",pszBuffer);
		fflush(stdout);
		SAFE_RELEASE(pErrorMsgs);
		delete [] pszBuffer;
	}
	fflush(stdout);
	if(FAILED(hr))
	{
		SAFE_RELEASE(pErrorMsgs);
		SAFE_RELEASE(*ppBlobCode);
	
		return false;
	}
	return true;
}

bool CGFXBase::CompileVertexShader(CDXGIManager* pManager, LPCWSTR pszVertexShaderFile,LPCSTR pszEntryPoint,ID3D10Blob** ppBlobCode,ID3D11VertexShader** ppOutVS)
{
	*ppBlobCode=NULL;
	*ppOutVS=NULL;
	if(!CompileShader(pszVertexShaderFile,pManager->GetVSProfile(),pszEntryPoint,ppBlobCode))
		printf("Fatal error compiling vertex shader\n");
	else if(!*ppBlobCode)
		printf("Errors found...\n");
	else
	{
		ID3D11Device* pDevice=pManager->GetDevice();
		pDevice->CreateVertexShader((*ppBlobCode)->GetBufferPointer(),
			                        (*ppBlobCode)->GetBufferSize(),
									NULL,
									ppOutVS);
		return true;
	}
	return false;
}



bool CGFXBase::CompilePixelShader(CDXGIManager* pManager, LPCWSTR pszPixelShaderFile,LPCSTR pszEntryPoint,ID3D10Blob** ppBlobCode,ID3D11PixelShader** ppOutPS)
{
	*ppBlobCode=NULL;
	*ppOutPS=NULL;
	if(!CompileShader(pszPixelShaderFile,pManager->GetPSProfile(),pszEntryPoint,ppBlobCode))
		printf("%s","Fatal error compiling pixel shader\n");
	else if(!*ppBlobCode)
		printf("%s","Errors found.\n");
	else
	{
		ID3D11Device* pDevice=pManager->GetDevice();
		pDevice->CreatePixelShader((*ppBlobCode)->GetBufferPointer(),(*ppBlobCode)->GetBufferSize(),NULL,ppOutPS);
		return true;
	}
	return false;
}
bool CGFXBase::CompileComputeShader(CDXGIManager* pManager, LPCWSTR pszPixelShaderFile,LPCSTR pszEntryPoint,ID3D10Blob** ppBlobCode,ID3D11ComputeShader** ppOutCS)
{
	*ppBlobCode=NULL;
	*ppOutCS=NULL;
	if(!CompileShader(pszPixelShaderFile,pManager->GetCSProfile(),pszEntryPoint,ppBlobCode))
		printf("%s","Fatal error compiling compute shader\n");
	else if(!*ppBlobCode)
		printf("%s","Errors found.\n");
	else
	{
		ID3D11Device* pDevice=pManager->GetDevice();
		pDevice->CreateComputeShader((*ppBlobCode)->GetBufferPointer(),(*ppBlobCode)->GetBufferSize(),NULL,ppOutCS);
		return true;
	}
	return false;
}
