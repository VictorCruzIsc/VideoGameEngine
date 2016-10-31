#pragma once
#include "filter.h"

class CLowPassFilter :
	public CFilter
{
public:
	float m_LastOutput;
	float m_Alpha;     
public:
	virtual void SetFilter(float SamplingFreq,float CutFreq);
	virtual float NextOutput(float Input);
	CLowPassFilter(void);
	~CLowPassFilter(void);
};
