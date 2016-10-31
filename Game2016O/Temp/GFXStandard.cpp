#include "stdafx.h"
#include "GFXStandard.h"


D3D11_INPUT_ELEMENT_DESC CGFXStandard::InputLayoutDesc[]=
{
		{"POSITION", 0,DXGI_FORMAT_R32G32B32A32_FLOAT,0, 0 , D3D11_INPUT_PER_VERTEX_DATA,0},
		{"NORMAL",   0,DXGI_FORMAT_R32G32B32A32_FLOAT,0, 16, D3D11_INPUT_PER_VERTEX_DATA,0},
		{"TEXCOORD", 0,DXGI_FORMAT_R32G32_FLOAT,      0, 32, D3D11_INPUT_PER_VERTEX_DATA,0}
};

CGFXStandard::CGFXStandard(CDXGIManager* pManager):CGFXBase(pManager)
{
	if(pManager->m_FeatureLevel>=D3D_FEATURE_LEVEL_10_0)
		m_pszHLSLFileName=L"..\\Shaders\\Standard.hlsl";
	else
		m_pszHLSLFileName=L"..\\Shaders\\Standard9_x.hlsl";
	m_pVS=NULL;
	m_pPS=NULL;
	m_pCB=NULL;
	m_pIL=NULL;
	m_pSAmbient=m_pManager->GetSamplerState(1);
	m_pSEmissive=m_pManager->GetSamplerState(1);
	m_pSDiffuse=m_pManager->GetSamplerState(1);
	m_pSSpecular=m_pManager->GetSamplerState(1);
	m_pSAlpha=m_pManager->GetSamplerState(1);
	m_pSIridiscent=m_pManager->GetSamplerState(1);
	memset(&m_Params,0,sizeof(PARAMS));
	MATRIX4D::Identity(m_Params.mWorld);
	MATRIX4D::Identity(m_Params.mView);
	MATRIX4D::Identity(m_Params.mProj);
	MATRIX4D::Identity(m_Params.mTex);
	//Vertex offset defaults
	m_Params.vTDOS=VECTOR4D(0,0,0,1);
	m_Params.vNormalMapping=VECTOR4D(1,1,0,0);
	m_Params.vEmissiveMapping=VECTOR4D(1,1,0,0);
}
CGFXStandard::~CGFXStandard(void)
{
	Uninitialize();
}

bool CGFXStandard::Initialize(void)
{
	ID3D10Blob *pVSCode=NULL,*pPSCode=NULL;
	if(!CompileVertexShader(m_pManager,m_pszHLSLFileName,"VSMain",&pVSCode,
		&m_pVS))
	{
		wprintf(L"Unable to compile Vertex Shader at %s\n",m_pszHLSLFileName);
		SAFE_RELEASE(pVSCode);
		m_pManager->FlushConsole();
		return false;
	}
	if(!CompilePixelShader(m_pManager,m_pszHLSLFileName ,"PSMain",&pPSCode,
		&m_pPS))
	{
		wprintf(L"Unable to compile Pixel Shader at %s",m_pszHLSLFileName);
		SAFE_RELEASE(pVSCode);
		SAFE_RELEASE(pPSCode);
		m_pManager->FlushConsole();
		return false;
	}
	m_pManager->FlushConsole();
	m_pManager->GetDevice()->CreateInputLayout(InputLayoutDesc,3,pVSCode->GetBufferPointer(),pVSCode->GetBufferSize(),&m_pIL);
	SAFE_RELEASE(pVSCode);
	SAFE_RELEASE(pPSCode);
	HRESULT hr=E_FAIL;
	D3D11_BUFFER_DESC bd;
	memset(&bd,0,sizeof(D3D11_BUFFER_DESC));
	bd.BindFlags=D3D11_BIND_CONSTANT_BUFFER ;
	bd.Usage=D3D11_USAGE_DYNAMIC;
	bd.ByteWidth=16*((sizeof(m_Params)+15)/16);
	bd.CPUAccessFlags =D3D11_CPU_ACCESS_WRITE;
	hr=m_pManager->GetDevice()->CreateBuffer(&bd,NULL,&m_pCB);
	if(FAILED(hr))
		printf("ERROR creating Constant Buffer\n");
	//registrar atributos
	RegisterAttribute("mWorld",&m_Params.mWorld,ATT_MATRIX4D);
	RegisterAttribute("mView",&m_Params.mView,ATT_MATRIX4D);
	RegisterAttribute("mProj",&m_Params.mProj,ATT_MATRIX4D);
	return true;
}
void CGFXStandard::Uninitialize (void)
{
	SAFE_RELEASE(m_pCB);
	SAFE_RELEASE(m_pIL);
	SAFE_RELEASE(m_pPS);
	SAFE_RELEASE(m_pVS);
}

int CGFXStandard::PassCount()
{
	return 1;
}
void CGFXStandard::Pass(int nPass)
{
	switch(nPass)
	{
	case 0:
		{
			ID3D11DeviceContext *pCtx;
			if(!(m_pManager && m_pManager->GetDevice())) break;
			pCtx=m_pManager->GetContext();
			//1.- Set The Input Layout ALWAYS!!!!
			pCtx->IASetInputLayout(m_pIL);
			pCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			
			//2.- Instalar los programas en GPU (VS,PS)
			pCtx->VSSetShader(m_pVS,NULL,0);
			pCtx->PSSetShader(m_pPS,NULL,0);

			//4.- Establecer el viewport

			D3D11_VIEWPORT vp;
			D3D11_TEXTURE2D_DESC dtd;
			m_pManager->GetBackBuffer()->GetDesc(&dtd);
			vp.TopLeftX =vp.TopLeftY=0;
			vp.Width =(float)dtd.Width ;
			vp.Height=(float)dtd.Height ;
			vp.MaxDepth=1.0f;
			vp.MinDepth=0.0f;
			pCtx->RSSetViewports(1,&vp);

			//Actualizar el constant buffer del GPU
			D3D11_MAPPED_SUBRESOURCE ms;
			PARAMS Params;
			m_Params.mWV=m_Params.mWorld*m_Params.mView ;
			m_Params.mWVP=m_Params.mWV*m_Params.mProj;
			Params=m_Params;
			//Transform Lights to view space
			Mul(m_Params.vAmbientLight,m_Params.Material.vAmbient,Params.vAmbientLight);
			for(int i=0;i<8;i++)
			{
				if(m_Params.Lights[i].bLightEnable)
				{
					Params.Lights[i].vDirection=m_Params.Lights[i].vDirection*m_Params.mView;
					Params.Lights[i].vPosition=m_Params.Lights[i].vPosition*m_Params.mView;
					Mul(m_Params.Lights[i].vAmbient,m_Params.Material.vAmbient,Params.Lights[i].vAmbient);
					Mul(m_Params.Lights[i].vDiffuse,m_Params.Material.vDiffuse,Params.Lights[i].vDiffuse);
					Mul(m_Params.Lights[i].vSpecular,m_Params.Material.vSpecular,Params.Lights[i].vSpecular);
				}
			}
			MATRIX4D::Transpose(m_Params.mWorld,Params.mWorld);
			MATRIX4D::Transpose(m_Params.mView,Params.mView);
			MATRIX4D::Transpose(m_Params.mProj,Params.mProj);
			MATRIX4D::Transpose(m_Params.mWV ,Params.mWV);
			MATRIX4D::Transpose(m_Params.mWVP ,Params.mWVP);
			MATRIX4D::Transpose(m_Params.mTex,Params.mTex);
			pCtx->Map(m_pCB,0,D3D11_MAP_WRITE_DISCARD,0,&ms);
			memcpy(ms.pData,&Params,sizeof(PARAMS));
			pCtx->Unmap(m_pCB,0);
			pCtx->VSSetConstantBuffers(0,1,&m_pCB);
			pCtx->PSSetConstantBuffers(0,1,&m_pCB);

			pCtx->PSSetSamplers(0,1,&m_pSAmbient);
			pCtx->PSSetSamplers(1,1,&m_pSEmissive);
			pCtx->PSSetSamplers(2,1,&m_pSDiffuse);
			pCtx->PSSetSamplers(3,1,&m_pSSpecular);
			pCtx->PSSetSamplers(4,1,&m_pSAlpha);
			pCtx->PSSetSamplers(5,1,&m_pSIridiscent);
		}
	}
}
