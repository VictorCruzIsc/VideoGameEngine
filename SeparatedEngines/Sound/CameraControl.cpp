#include "StdAfx.h"
#include "CameraControl.h"


CCameraControl::CCameraControl(void)
{
	m_fFinalPositionCutOffFreq=1.0f;
	m_fLookAtPositionCutOffFreq=1.0f; 
	m_pCamera=NULL;
	m_vFinalPosition=VECTOR4D(100,100,100,1);
	m_vLookAtPosition=VECTOR4D(0,1,0,1);
	m_pCameraTransitions=NULL;
	m_nCameraTransitionCount=NULL;
	m_nTransitionIndex=NULL;
}


CCameraControl::~CCameraControl(void)
{
	
}


void CCameraControl::SetCamera(CCamera3D* pCamera)
{
	m_pCamera=pCamera;
}
CCamera3D* CCameraControl::GetCamera(void)
{
	return m_pCamera;
}

void CCameraControl::ResetFilters(void)
{
	for(int i=0;i<4;i++)
	{
		m_aFiltersFinalPosition[i].m_LastOutput=m_vFinalPosition.v[i];
		m_aFiltersLookAtPosition[i].m_LastOutput=m_vLookAtPosition.v[i];
		m_aFiltersUpDirection[i].m_LastOutput=m_vUpDirection.v[i];
	}
}

void CCameraControl::Time(float dts)
{
	//Evaluar los parámetros del los filtros según el diferencial de tiempo actual
	CLowPassFilter FilterParamsFinalPosition,FilterParamsLookAtPosition,FilterParamsUpDirection;
	FilterParamsFinalPosition.SetFilter(1.0f/dts,m_fFinalPositionCutOffFreq);
	FilterParamsLookAtPosition.SetFilter(1.0f/dts,m_fLookAtPositionCutOffFreq);
	FilterParamsUpDirection.SetFilter(1.0f/dts,m_fLookAtPositionCutOffFreq);
	for(int i=0;i<4;i++)
	{
		m_aFiltersFinalPosition[i].m_Alpha=FilterParamsFinalPosition.m_Alpha;
		m_aFiltersFinalPosition[i].m_CutFreq=FilterParamsFinalPosition.m_CutFreq;
		m_aFiltersFinalPosition[i].m_SamplingFreq=FilterParamsFinalPosition.m_SamplingFreq;
		m_aFiltersFinalPosition[i].NextOutput(m_vFinalPosition.v[i]);

		m_aFiltersLookAtPosition[i].m_Alpha=		FilterParamsLookAtPosition.m_Alpha;
		m_aFiltersLookAtPosition[i].m_CutFreq=		FilterParamsLookAtPosition.m_CutFreq;
		m_aFiltersLookAtPosition[i].m_SamplingFreq=	FilterParamsLookAtPosition.m_SamplingFreq;
		m_aFiltersLookAtPosition[i].NextOutput(m_vLookAtPosition.v[i]);

		m_aFiltersUpDirection[i].m_Alpha=		FilterParamsUpDirection.m_Alpha;
		m_aFiltersUpDirection[i].m_CutFreq=		FilterParamsUpDirection.m_CutFreq;
		m_aFiltersUpDirection[i].m_SamplingFreq=	FilterParamsUpDirection.m_SamplingFreq;
		m_aFiltersUpDirection[i].NextOutput(this->m_vUpDirection.v[i]);
	}
}

void CCameraControl::Transfer(void)
{
	VECTOR4D vPosition,vLookAt,vUpDir;
	if(m_pCamera)
	{
		for(int i=0;i<4;i++)
		{
			vPosition.v[i]=m_aFiltersFinalPosition[i].m_LastOutput;
			vLookAt.v[i]=m_aFiltersLookAtPosition[i].m_LastOutput;
			vUpDir.v[i]=m_aFiltersUpDirection[i].m_LastOutput;
		}
		m_pCamera->Position()=vPosition;
		m_pCamera->SetLookAt(vLookAt,vUpDir);
	}
}
void CCameraControl::SetCameraTransitions(CCameraControl::CAMERA_TRANSITION* pTransitions,int nTransitionCount)
{
	m_nCameraTransitionCount=nTransitionCount;
	m_pCameraTransitions=pTransitions;
	m_nTransitionIndex=0;
}
int CCameraControl::UpdateCameraSequence(unsigned long t,bool bReset)
{
	if(m_pCameraTransitions)
	{
		if((t >= m_pCameraTransitions[m_nTransitionIndex].t) && (m_nTransitionIndex<m_nCameraTransitionCount))
		{
			FinalPosition()				=m_pCameraTransitions[m_nTransitionIndex].vPosition;
			LookAtPosition()			=m_pCameraTransitions[m_nTransitionIndex].vLookAt;
			UpDirection()			    =m_pCameraTransitions[m_nTransitionIndex].vUpDir;
			FinalPositionCutOffFreq()	=m_pCameraTransitions[m_nTransitionIndex].fFinalPositionCutOffFreq;
			LookAtCutOffFreq()			=m_pCameraTransitions[m_nTransitionIndex].fLookAtPositionCutOffFreq;
			m_nTransitionIndex++;
		}
	}
	if(bReset)
	{
		m_nTransitionIndex=0;
	}
	return m_nTransitionIndex;
}