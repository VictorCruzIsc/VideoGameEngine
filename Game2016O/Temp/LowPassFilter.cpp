#include "StdAfx.h"
#include "LowPassFilter.h"

CLowPassFilter::CLowPassFilter(void)
{
	m_LastOutput=0;
	m_Alpha=0;
}

CLowPassFilter::~CLowPassFilter(void)
{
}
void CLowPassFilter::SetFilter(float SamplingFreq,float CutFreq)
{
	m_SamplingFreq=SamplingFreq;
	m_CutFreq=CutFreq;
	float rc=1/(2*3.14159265f*CutFreq);
	float dt=1/SamplingFreq;
	m_Alpha=dt/(rc+dt);
}
float CLowPassFilter::NextOutput(float Input)
{
	float Output=m_LastOutput+m_Alpha*(Input-m_LastOutput);
	m_LastOutput=Output;
	return Output;
}