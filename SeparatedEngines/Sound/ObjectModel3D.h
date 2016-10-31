#pragma once
#include "object3d.h"
#define CLSID_CObjectModel3D 0x56fc3a1d
class CObjectModel3D :
	public CObject3D
{
public:
	//Sobrecargas de CObjectBase
	unsigned long GetClassID(){ return CLSID_CObjectModel3D;}
	const char* GetClassName(){ return "CObjectModel3D";};
	long m_lMeshID;  //Identificador de mesh
	CObjectModel3D(void);
	virtual ~CObjectModel3D(void);
};

