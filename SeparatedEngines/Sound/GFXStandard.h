#pragma once
#include "GFXBase.h"
#include "MeshBase.h"

#define GFX_AMBIENT_LIGHTING  0x01
#define GFX_DIFFUSE_LIGHTING  0x02
#define GFX_SPECULAR_LIGHTING 0x04
#define GFX_AMBIENT_MAPPING   0x08
#define GFX_EMISSIVE_MAPPING  0x10
#define GFX_DIFFUSE_MAPPING   0x20
#define GFX_SPECULAR_MAPPING  0x40
#define GFX_ALPHA_MAPPING     0x80
#define GFX_IRIDISCENT_MAPPING 0x100
#define GFX_NORMAL_MAPPING	  0x200
class CGFXStandard:public CGFXBase
{
protected:
	static D3D11_INPUT_ELEMENT_DESC InputLayoutDesc[];
	ID3D11InputLayout	*m_pIL;
	ID3D11VertexShader	*m_pVS;
	ID3D11PixelShader	*m_pPS;
	ID3D11Buffer		*m_pCB;
public:
	struct LIGHT
	{
		union
		{
			struct
			{
				long nLightType;
				long bLightEnable;
			};
			SVECTOR4D LightTypeAndSwitches;
		};
		VECTOR4D vPosition;
		VECTOR4D vDirection;
		VECTOR4D vDiffuse;
		VECTOR4D vSpecular;
		VECTOR4D vAmbient;
		VECTOR4D vAttenuation;
		union
		{
			struct
			{
				float fPowerSpot;
				float fRange;
			};
			SVECTOR4D LightPowerAndRange;
		};
	};
	struct PARAMS
	{
		VECTOR4D vFlags;
		MATRIX4D mWorld;
		MATRIX4D mView;
		MATRIX4D mProj;
		MATRIX4D mTex;   
		MATRIX4D mWV;
		MATRIX4D mWVP;
		VECTOR4D vAmbientLight;
		VECTOR4D vIridiscentBlend;
		VECTOR4D vNormalMapping;
		VECTOR4D vEmissiveMapping;
		LIGHT Lights[8];
		VECTOR4D vTDOS;
		struct
		{
			CMeshBase::MATERIAL Material;
			float padding[3];
		};
	} m_Params;
	ID3D11SamplerState  *m_pSAmbient;
	ID3D11SamplerState	*m_pSEmissive;
	ID3D11SamplerState  *m_pSDiffuse;
	ID3D11SamplerState  *m_pSSpecular;
	ID3D11SamplerState  *m_pSAlpha;
	ID3D11SamplerState  *m_pSIridiscent;
public:
	CGFXStandard(CDXGIManager* pManager);
	virtual bool Initialize();
	virtual void Uninitialize();
	virtual void Pass(int nPass);
	virtual int  PassCount();
	virtual ~CGFXStandard(void);
};

