#pragma once
#include "DXManager.h"
#include "Matrix4D.h"
#include <vector>
using namespace std;
class CFX //técnica
{
	CDXManager* m_pOwner;
	ID3D11InputLayout* m_pIL;
	ID3D11Buffer*      m_pCB;
	ID3D11VertexShader* m_pVS;
	vector<ID3D11PixelShader*> m_vecFX;
	ID3D11Buffer *m_pIB, *m_pVB;
	struct VERTEX
	{
		VECTOR4D Position;
		VECTOR4D TexCoord;
		VECTOR4D Color;
		static D3D11_INPUT_ELEMENT_DESC InputLayout[];
	}Frame[4];
public:
	ID3D11ShaderResourceView* m_pSRVInput0;
	ID3D11RenderTargetView*   m_pRTVOutput;
	struct PARAMS
	{
		VECTOR4D Delta;
		VECTOR4D RadialBlur;
		VECTOR4D DirectionalBlur;
	}m_Params;
	CFX(CDXManager* pOwner);
	bool Initialize();
	void Process(int iProcess);
	void Uninitialize();
	~CFX();
};

