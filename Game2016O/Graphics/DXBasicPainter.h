#pragma once
#include "DXManager.h"
#include "Matrix4D.h"

#define LIGHT_ON 0x1
#define LIGHT_DIRECTIONAL 0
#define LIGHT_POINT 1
#define LIGHT_SPOT 2

#define LIGHTING_AMBIENT 0x1
#define LIGHTING_DIFFUSE 0x2
#define LIGHTING_SPECULAR 0x4
#define LIGHTING_EMISSIVE 0x8
#define MAPPING_DIFFUSE  0x10
#define MAPPING_NORMAL  0x20
#define MAPPING_ENVIROMENTAL_FAST 0x40
#define MAPPING_EMISSIVE 0x80
#define MAPPING_SHADOW 0x100
#define VERTEX_PRELIGHTEN 0x200
#define SHADOW_MAP_RESOLUTION 1024
class CDXBasicPainter
{
protected:
	CDXManager* m_pManager;
	ID3D11InputLayout* m_pIL;
	ID3D11VertexShader* m_pVS;
	ID3D11PixelShader* m_pPS;
	ID3D11Buffer*      m_pCB;
	ID3D11RenderTargetView* m_pRTV;
	ID3D11DepthStencilView* m_pDSV;
	ID3D11RasterizerState* m_pDrawLH;
	ID3D11RasterizerState* m_pDrawRH;
	ID3D11DepthStencilState* m_pDSSMark;
	ID3D11DepthStencilState* m_pDSSDrawOnMark;
	ID3D11DepthStencilState* m_pDSSDraw;

	//Soporte para Sombras
	ID3D11ShaderResourceView* m_pSRVShadowMap;
	ID3D11DepthStencilView*   m_pDSVShadowMap;
	ID3D11RenderTargetView*   m_pRTVShadowMap;
	ID3D11VertexShader*       m_pVSShadow;
	ID3D11PixelShader*        m_pPSShadow;

public:
	void ClearShadow();
	void SetRenderTarget(ID3D11RenderTargetView* pRTV,ID3D11DepthStencilView* pDSV=NULL)
	{m_pRTV = pRTV;	m_pDSV = pDSV;}
	ID3D11RasterizerState* GetDrawRHState(){return m_pDrawRH;}
	ID3D11RasterizerState* GetDrawLHState(){return m_pDrawLH;}
	struct MATERIAL
	{
		VECTOR4D Ambient;
		VECTOR4D Diffuse;
		VECTOR4D Specular;
		VECTOR4D Emissive;
		VECTOR4D Power;
	};
	struct LIGHT
	{
		struct{
			unsigned long Flags, Type, notused0, notused1;
		};
		VECTOR4D Ambient;
		VECTOR4D Diffuse;
		VECTOR4D Specular;
		VECTOR4D Attenuation; //  1/(x+y*d+z*d^2)
		VECTOR4D Position;
		VECTOR4D Direction;
		VECTOR4D Factors;
	};
	struct PARAMS
	{
		struct
		{
			unsigned long Flags, notused0, notused1, notused2;
		};
		MATRIX4D World;
		MATRIX4D View;
		MATRIX4D Projection;
		MATRIX4D LightView;
		MATRIX4D LightProjection;
		VECTOR4D Brightness;
		MATERIAL Material;
		LIGHT    Lights[8];
	}m_Params;
	struct VERTEX
	{
		VECTOR4D Position;
		VECTOR4D Normal;
		VECTOR4D Tangent;
		VECTOR4D Binormal;
		VECTOR4D Color;
		VECTOR4D TexCoord;
		static D3D11_INPUT_ELEMENT_DESC InputLayout[];
	};
	CDXBasicPainter(CDXManager* pOwner);
	bool Initialize();
	void Uninitialize();
	void DrawIndexed(VERTEX* pVertices,
		unsigned long nVertices,
		unsigned long *pIndices,
		unsigned long nIndices,bool bShadow=false);
	~CDXBasicPainter();
};

