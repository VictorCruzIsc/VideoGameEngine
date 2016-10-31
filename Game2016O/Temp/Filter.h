#pragma once

class CFilter
{
public:
	//Frecuencia de muestreo
	float m_SamplingFreq;
	//Frecuencia de corte
	float m_CutFreq;
public:
	virtual void SetFilter(float SamplingFreq,float CutFreq)=0;
	virtual float NextOutput(float Input)=0;
	CFilter(void);
	~CFilter(void);
};
