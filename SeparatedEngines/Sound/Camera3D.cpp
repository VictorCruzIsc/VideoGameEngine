#include "StdAfx.h"
#include "Camera3D.h"
CCamera3D::CCamera3D(void)
{
	m_fFOV=0.0f;
	m_fAspect=0.0f;
	m_fZnear=0.0f;
	m_fZfar=0.0f;
	MATRIX4D::Identity(m_mView);
	MATRIX4D::Identity(m_mProjection);
	VECTOR4D vEye(1.5f,1.5f,1.8f);
	Position()=vEye;
	VECTOR4D vLookAt(0,0,0);
	VECTOR4D vUp(0,1,0,0);
	SetLookAt(vLookAt,vUp);
	//Camara con una lente de FOV de 60º con proyección en perspectiva
	SetProjection(60.0f*3.151592f/180.0f,4.0f/3.0f,1,1000);
}
CCamera3D::~CCamera3D(void)
{
}
unsigned long CCamera3D::GetClassID()
{
	return CLSID_CCamera3D;
}
const char* CCamera3D::GetClassName()
{
	return "CCamera3D";
}
void CCamera3D::ExtractCameraVectors(void)
{
	m_vRight  = VECTOR4D(m_mView._11,m_mView._21,m_mView._31,0);
	m_vUp     = VECTOR4D(m_mView._12,m_mView._22,m_mView._32,0);
	m_vViewDir= VECTOR4D(m_mView._13,m_mView._23,m_mView._33,0);
}
void CCamera3D::SetLookAt(VECTOR4D& vTargetPosition,VECTOR4D &vUp)
{
	m_vLookAt=vTargetPosition;
	Yaw()=0;
	Pitch()=0;
	Roll()=0;
	UpdateLocalMatrix();
	MATRIX4D::LookAtLH(m_vPosition,vTargetPosition,vUp,m_mView);
	ExtractCameraVectors();
	UpdateViewMatrix();
}

void CCamera3D::SetProjection(float fFOVAngle,float fAspect,float fZnear,float fZfar)
{
	m_fFOV=fFOVAngle;
	m_fAspect=fAspect;
	m_fZnear=fZnear;
	m_fZfar=fZfar;
	MATRIX4D::PerspectiveProjectionLH(fFOVAngle,fAspect,fZnear,fZfar,m_mProjection);
}

void CCamera3D::UpdateViewMatrix(void)
{	
	MATRIX4D O,Rotation,T;
	VECTOR4D N,V,U,Ut;
	
	//Normalización y correción de ortogonalidad
	Normalize(m_vViewDir,N);
	Cross3(m_vUp,N,Ut);
	Normalize(Ut,U);
	Cross3(N,U,V);

	//Transformación de orientación transpuesta
	O._11=U.x; O._12=V.x;O._13=N.x;O._14=0;
	O._21=U.y; O._22=V.y;O._23=N.y;O._24=0;
	O._31=U.z; O._32=V.z;O._33=N.z;O._34=0;
	O._41=0  ; O._42=0  ;O._43=0  ;O._44=1;


	MATRIX4D RRoll,RPitch,RYaw;
	MATRIX4D::RotationZ(Roll(),RRoll);
	MATRIX4D::RotationX(Pitch(),RPitch);
	MATRIX4D::RotationY(Yaw(),RYaw);
	Rotation=RYaw*RPitch*RRoll;
	

	//Hasta aquí la matriz de orientacion de vista ya girada!!!
	MATRIX4D::Translation(-m_vPosition.x,-m_vPosition.y,-m_vPosition.z,T);
	m_mView=T*O*Rotation;
	//Actualización, volver a hacer Yaw,Pitch,Roll a cero para evitar acumulación de giro y extraer vectores
	ExtractCameraVectors();
	Yaw()=Pitch()=Roll()=0;
}
void CCamera3D::GetProjectionMatrix(MATRIX4D& mOutProjectionMatrix)
{
	mOutProjectionMatrix=m_mProjection;
}
void CCamera3D::GetViewMatrix(MATRIX4D& mOutViewMatrix)
{
	//UpdateViewMatrix(); //DESIGN BUG FIXED!!!!
	mOutViewMatrix=m_mView;
}

bool CCamera3D::Serialize(CStream& io,bool bSave,CObjectFactory& Factory)
{
	//en este metodo solo se guarda y carga datos especificos y persistentes de esta especificacion de Object3D
	__super::Serialize(io,bSave,Factory);
	if(bSave)
	{
		io.write((char*)&m_vRight,sizeof(m_vRight));
		io.write((char*)&m_vUp,sizeof(m_vUp));
		io.write((char*)&m_vViewDir,sizeof(m_vViewDir));
		io.write((char*)&m_vLookAt,sizeof(m_vLookAt));
		io.write((char*)&m_fFOV,sizeof(m_fFOV));
		io.write((char*)&m_fAspect,sizeof(m_fAspect));
		io.write((char*)&m_fZnear,sizeof(m_fZnear));
		io.write((char*)&m_fZfar,sizeof(m_fZfar));
		io.write((char*)&m_mView,sizeof(m_mView));
		io.write((char*)&m_mProjection,sizeof(m_mProjection));
	}
	else
	{
		io.read((char*)&m_vRight,sizeof(m_vRight));
		io.read((char*)&m_vUp,sizeof(m_vUp));
		io.read((char*)&m_vViewDir,sizeof(m_vViewDir));
		io.read((char*)&m_vLookAt,sizeof(m_vLookAt));
		io.read((char*)&m_fFOV,sizeof(m_fFOV));
		io.read((char*)&m_fAspect,sizeof(m_fAspect));
		io.read((char*)&m_fZnear,sizeof(m_fZnear));
		io.read((char*)&m_fZfar,sizeof(m_fZfar));
		io.read((char*)&m_mView,sizeof(m_mView));
		io.read((char*)&m_mProjection,sizeof(m_mProjection));
	}
	return true;
}


void CCamera3D::CalcLocalMatrixFromViewMatrix()
{
	MATRIX4D::Inverse(m_mView,m_mLocal);
}
void CCamera3D::CalcViewMatrixFromLocalMatrix()
{
	MATRIX4D::Inverse(m_mLocal,m_mView);
}