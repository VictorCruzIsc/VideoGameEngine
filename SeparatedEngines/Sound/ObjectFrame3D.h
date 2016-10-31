#pragma once
#include "object3d.h"
#define CLSID_CObjectFrame3D 0xf5da3c8d
class CObjectFrame3D :
	public CObject3D
{
public:
	//Sobrecargas de CObjectBase
	unsigned long GetClassID(){ return CLSID_CObjectFrame3D;}
	const char* GetClassName(){ return "CObjectFrame3D";};
	CObjectFrame3D(void);
	virtual ~CObjectFrame3D(void);
};

