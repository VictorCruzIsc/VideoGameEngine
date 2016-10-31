#include "stdafx.h"
#include "GFXStandardHQ.h"


CGFXStandardHQ::CGFXStandardHQ(CDXGIManager* pManager):CGFXStandard(pManager)
{
	m_pszHLSLFileName=L"..\\Shaders\\StandardHQ.hlsl";
	m_pSAmbient=m_pManager->GetSamplerState(1);
	m_pSEmissive=m_pManager->GetSamplerState(1);
	m_pSDiffuse=m_pManager->GetSamplerState(1);
	m_pSSpecular=m_pManager->GetSamplerState(1);
	m_pSAlpha=m_pManager->GetSamplerState(1);
	m_pSIridiscent=m_pManager->GetSamplerState(1);
}


CGFXStandardHQ::~CGFXStandardHQ(void)
{

}
