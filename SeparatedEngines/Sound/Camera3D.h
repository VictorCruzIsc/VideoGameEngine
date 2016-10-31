#pragma once
#include "Object3D.h"
#define CLSID_CCamera3D 0x32425610
class CCamera3D:public CObject3D
{
protected:
	VECTOR4D m_vRight;
	VECTOR4D m_vUp;
	VECTOR4D m_vViewDir;
	VECTOR4D m_vLookAt;
	MATRIX4D  m_mView;
	MATRIX4D  m_mProjection;
protected:
	float m_fFOV;
	float m_fAspect;
	float m_fZnear;
	float m_fZfar;
	void ExtractCameraVectors(void);
public:
	void SetLookAt(VECTOR4D& vTargetPosition,VECTOR4D &vUp);
	void SetProjection(float fFOVAngle,float fAspect,float fZnear,float fZfar);

	VECTOR4D Up(void){return VECTOR4D(m_mView._12,m_mView._22,m_mView._32,0);}
	VECTOR4D Right(void) {return  VECTOR4D(m_mView._11,m_mView._21,m_mView._31,0);}
	VECTOR4D ViewDir(void){return VECTOR4D(m_mView._13,m_mView._23,m_mView._33,0);}
	
	const VECTOR4D& LookAt(void){return m_vLookAt;}
	float FOV(void){return m_fFOV;}
	float Aspect(void){return m_fAspect;}
	float Znear(void){return m_fZnear;}
	float Zfar(void){return m_fZfar;}
	void GetProjectionMatrix(MATRIX4D& mOutProjectionMatrix);
	void GetViewMatrix(MATRIX4D& mOutViewMatrix);
	void UpdateViewMatrix(void);
	CCamera3D(void);
	virtual ~CCamera3D(void);
	//Persistencia
	virtual bool Serialize(CStream& io,bool bSave,CObjectFactory& Factory);
	virtual unsigned long GetClassID();
	virtual const char* GetClassName();


	void CalcLocalMatrixFromViewMatrix();
	void CalcViewMatrixFromLocalMatrix();

};

