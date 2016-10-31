#include "StdAfx.h"
#include "Object3D.h"



CObject3D::CObject3D(void)
{
	m_vPosition=VECTOR4D(0,0,0,1);
	m_vScaling=VECTOR4D(1,1,1,0);
	m_fYaw=m_fPitch=m_fRoll=0;
	MATRIX4D::Identity(m_mLocal);
	MATRIX4D::Identity(m_mPrefix);
	MATRIX4D::Identity(m_mPostfix);
}
CObject3D::~CObject3D(void)
{
}
unsigned long CObject3D::GetClassID() {return CLSID_CObject3D;}
const char* CObject3D::GetClassName()  { return "CObject3D";}
bool CObject3D::Serialize(CStream& io,bool bSave,CObjectFactory& Factory)
{
	if(bSave)
	{
		//Salvar los atributos del objeto
		io.write((char*)&ID(),sizeof(unsigned long));
		io.write((char*)&m_vScaling,sizeof(VECTOR4D));
		io.write((char*)&m_vPosition,sizeof(VECTOR4D));
	
		io.write((char*)&m_fYaw,sizeof(float));
		io.write((char*)&m_fPitch,sizeof(float));
		io.write((char*)&m_fRoll,sizeof(float));

		io.write((char*)&m_mPrefix,sizeof(MATRIX4D));
		io.write((char*)&m_mPostfix,sizeof(MATRIX4D));


		unsigned long ulObjectCount=m_mapObjects.size();
		io.write((char*)&ulObjectCount,sizeof(unsigned long));
		for(auto it=m_mapObjects.begin();it!=m_mapObjects.end();it++)
		{
			//Alamacenar el CLSID del objeto hijo, para posterior recuperación
			unsigned long clsid=it->second->GetClassID();
			io.write((char*)&clsid,sizeof(unsigned long));
			((CObject3D*)(it->second))->Serialize(io,bSave,Factory);
		}
	}
	else
	{
		ReleaseAll();
		io.read((char*)&ID(),sizeof(unsigned long));
		io.read((char*)&m_vScaling,sizeof(VECTOR4D));
		io.read((char*)&m_vPosition,sizeof(VECTOR4D));
		
		io.read((char*)&m_fYaw,sizeof(float));
		io.read((char*)&m_fPitch,sizeof(float));
		io.read((char*)&m_fRoll,sizeof(float));

		io.read((char*)&m_mPrefix,sizeof(MATRIX4D));
		io.read((char*)&m_mPostfix,sizeof(MATRIX4D));

		unsigned long ulObjectCount;
		io.read((char*)&ulObjectCount,sizeof(unsigned long));
		for(unsigned long ulI=0;ulI<ulObjectCount;ulI++)
		{
			CObjectBase* pNewObject;
			unsigned long ulKey;
			unsigned long clsid;
			io.read((char*)&clsid,sizeof(unsigned long));
			pNewObject=Factory.CreateObject(clsid);
			((CObject3D*)pNewObject)->Serialize(io,bSave,Factory);
			ulKey=pNewObject->ID();
			m_mapObjects.insert(make_pair(ulKey,pNewObject));
		}
	}
	return true;
}

void CObject3D::UpdateLocalMatrix(void)
{
		MATRIX4D S,RYaw,RPitch,RRoll,R,T;
		MATRIX4D::Scale(m_vScaling.x,m_vScaling.y,m_vScaling.z,S);
		MATRIX4D::RotationZ(Roll(),RRoll);
		MATRIX4D::RotationX(Pitch(),RPitch);
		MATRIX4D::RotationY(Yaw(),RYaw);
		R=RRoll*RPitch*RYaw;
		MATRIX4D::Translation(m_vPosition.x,m_vPosition.y,m_vPosition.z,T);
		m_mLocal=m_mPrefix*S*R*T*m_mPostfix;
}
VECTOR4D& CObject3D::Position(void)
{
	return m_vPosition;
}
float& CObject3D::Yaw(void)
{
	return m_fYaw;
}
float& CObject3D::Pitch(void)
{
	return m_fPitch;
}
float& CObject3D::Roll(void)
{
	return m_fRoll;
}
MATRIX4D&  CObject3D::LocalMatrix(void)
{
	return m_mLocal;
}
MATRIX4D&  CObject3D::PreMatrix(void)
{
	return m_mPrefix;
}
MATRIX4D&  CObject3D::PostMatrix(void)
{
	return m_mPostfix;
}
VECTOR4D& CObject3D::Scaling(void)
{
	return m_vScaling;
}