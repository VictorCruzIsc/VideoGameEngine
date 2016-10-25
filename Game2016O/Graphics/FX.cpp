#include "stdafx.h"
#include "FX.h"

D3D11_INPUT_ELEMENT_DESC CFX::VERTEX::InputLayout[]=
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{"TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

CFX::CFX(CDXManager* pOwner)
{
	m_pOwner = pOwner;
	m_pCB = NULL;
	m_pIL = NULL;
	m_pSRVInput0 = 0;
	m_pRTVOutput = NULL;
	m_pVS = NULL;
	m_pIB = NULL;
	m_pVB = NULL;
	memset(&m_Params, 0, sizeof(PARAMS));
	VERTEX V0 = { { -1,  1, 0, 1 }, { 0, 0, 0, 1 }, { 1, 1, 1, 1 } };
	VERTEX V1 = { {  1,  1, 0, 1 }, { 1, 0, 0, 1 }, { 1, 1, 1, 1 } };
	VERTEX V2 = { { 1,  -1, 0, 1 }, { 1, 1, 0, 1 }, { 1, 1, 1, 1 } };
	VERTEX V3 = { { -1, -1, 0, 1 }, { 0, 1, 0, 1 }, { 1, 1, 1, 1 } };
	Frame[0] = V0;
	Frame[1] = V1;
	Frame[2] = V2;
	Frame[3] = V3;
}

bool CFX::Initialize()
{
	D3D11_BUFFER_DESC dbd;
	memset(&dbd, 0, sizeof(dbd));
	dbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	dbd.Usage = D3D11_USAGE_DYNAMIC;
	dbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	dbd.ByteWidth = 16 * ((sizeof(PARAMS) + 15) / 16);
	m_pOwner->GetDevice()->CreateBuffer(&dbd, 0, &m_pCB);
	ID3D10Blob* pVSCode=NULL;
	m_pVS=m_pOwner->CompileVertexShader(L"..\\Shaders\\FX.hlsl", "VSMain", &pVSCode);
	m_pOwner->GetDevice()->CreateInputLayout(VERTEX::InputLayout,
		sizeof(VERTEX::InputLayout) / sizeof(D3D11_INPUT_ELEMENT_DESC),
		pVSCode->GetBufferPointer(), pVSCode->GetBufferSize(), &m_pIL);
	SAFE_RELEASE(pVSCode);
	//Compilar efectos
	char* Effects[] = { "PSEdgeDetect", "PSRadialBlur", "PSDirectionalBlur", 
		"PSGaussHorizontalBlur" , "PSGaussVerticalBlur"};
	for (auto FXName : Effects)
	{
		ID3D11PixelShader* pPS=
			m_pOwner->CompilePixelShader(L"..\\Shaders\\FX.hlsl", FXName);
		m_vecFX.push_back(pPS);
	}
	memset(&dbd, 0, sizeof(dbd));
	dbd.ByteWidth = sizeof(VERTEX) * 4;
	dbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	dbd.Usage = D3D11_USAGE_DYNAMIC;
	dbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_pOwner->GetDevice()->CreateBuffer(&dbd, 0, &m_pVB);
	dbd.ByteWidth = sizeof(unsigned long) * 6;
	dbd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	dbd.Usage = D3D11_USAGE_IMMUTABLE;
	dbd.CPUAccessFlags = 0;
	unsigned long Indices[6] = { 0, 1, 3, 3, 1, 2 };
	D3D11_SUBRESOURCE_DATA dsd;
	dsd.pSysMem = Indices;
	dsd.SysMemPitch = 0;
	dsd.SysMemSlicePitch = 0;
	m_pOwner->GetDevice()->CreateBuffer(&dbd, &dsd, &m_pIB);
	return true;
}

void CFX::Process(int iProcess)
{
	auto pCtx = m_pOwner->GetContext();
	pCtx->ClearState();
	pCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pCtx->IASetInputLayout(m_pIL);
	unsigned int Offset = 0, Stride = sizeof(VERTEX);
	pCtx->IASetVertexBuffers(0, 1, &m_pVB, &Stride, &Offset);
	pCtx->IASetIndexBuffer(m_pIB, DXGI_FORMAT_R32_UINT, 0);
	pCtx->VSSetConstantBuffers(0, 1, &m_pCB);
	pCtx->PSSetConstantBuffers(0, 1, &m_pCB);
	pCtx->VSSetShader(m_pVS, 0, 0);
	pCtx->PSSetShader(m_vecFX[iProcess],0,0);
	pCtx->OMSetRenderTargets(1, &m_pRTVOutput, NULL);	
	pCtx->PSSetShaderResources(0, 1, &m_pSRVInput0);
	D3D11_MAPPED_SUBRESOURCE ms;
	pCtx->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
	memcpy(ms.pData, Frame, sizeof(Frame));
	pCtx->Unmap(m_pVB, 0);
	ID3D11Texture2D* pRT = NULL;
	m_pRTVOutput->GetResource((ID3D11Resource**)&pRT);
	D3D11_TEXTURE2D_DESC dtd;
	pRT->GetDesc(&dtd);
	m_Params.Delta.x = 1.0f / dtd.Width;
	m_Params.Delta.y = 1.0f / dtd.Height;
	pCtx->Map(m_pCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
	memcpy(ms.pData, &m_Params, sizeof(PARAMS));
	pCtx->Unmap(m_pCB, 0);
	D3D11_VIEWPORT vp;
	vp.Width = dtd.Width;
	vp.Height = dtd.Height;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	pCtx->RSSetViewports(1, &vp);
	SAFE_RELEASE(pRT);
	pCtx->DrawIndexed(6, 0, 0);	
}
void CFX::Uninitialize()
{
	SAFE_RELEASE(m_pCB);
	SAFE_RELEASE(m_pIB);
	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pVS);
	SAFE_RELEASE(m_pIL);
	for (auto &pPS : m_vecFX)
		SAFE_RELEASE(pPS);
	m_vecFX.clear();
}
CFX::~CFX()
{
}
