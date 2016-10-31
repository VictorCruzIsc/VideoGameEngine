#pragma once
#include <map>
#include "ObjectBase.h"
#include "ObjectStock.h"
#include "ObjectPersistance.h"
#include "ObjectFactory.h"
#include "Matrix4D.h"
using namespace std;
#define CLSID_CObject3D 0xffff8507
class CObject3D:
	public CObjectBase,
	public CObjectStock,
	public CObjectPersistance
{
protected:
	//Atributos Geométricos
	VECTOR4D m_vScaling; 
	VECTOR4D m_vPosition;
	//Orientación mediante angulos de Euler
	float m_fYaw; //Rotation Y-Axis
	float m_fPitch; //Rotation X-Axis
	float m_fRoll;  //Rotation Z-Axis
	//Pre Matrix
	MATRIX4D m_mPrefix;
	//Post Matrix
	MATRIX4D m_mPostfix;
	//Matriz Local Compuesta 
	MATRIX4D m_mLocal;
	//Matriz de respaldo durante análisis jerárquico.
	MATRIX4D m_mBack;
public:
	CObject3D(void);
	virtual ~CObject3D(void);
	void        UpdateLocalMatrix();
	VECTOR4D&	Position(void);
	VECTOR4D&	Scaling(void);
	float&      Yaw(void);
	float&      Pitch(void);
	float&      Roll(void);
	MATRIX4D&   LocalMatrix(void);
	MATRIX4D&   PostMatrix();
	MATRIX4D&	PreMatrix();
	//Persistencia
	virtual bool Serialize(CStream& io,bool bSave,CObjectFactory& Factory);
	//Identificación
	virtual const char* GetClassName(void);
	virtual unsigned long GetClassID();
};

