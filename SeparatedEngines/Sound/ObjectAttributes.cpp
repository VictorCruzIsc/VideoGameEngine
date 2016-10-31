#include "StdAfx.h"
#include "ObjectAttributes.h"
#include "Matrix4D.h"
#include "ObjectBase.h"
CObjectAttributes::CObjectAttributes(void)
{
}
CObjectAttributes::~CObjectAttributes(void)
{
}
AttributeType CObjectAttributes::GetAttributeType(const char* pszAttributeName)
{
	auto it=m_mapAttributes.find(pszAttributeName);
	if(it!=m_mapAttributes.end())
		return it->second.m_Type;
	return ATT_INVALID;
}
CObjectAttributes::ATTRIBUTE* CObjectAttributes::GetAttribute(const char* pszAttributeName,AttributeType TypeToCheck)
{
	auto it=m_mapAttributes.find(pszAttributeName);
	if(it!=m_mapAttributes.end())
		if(TypeToCheck==it->second.m_Type)
			return &it->second;
	return NULL;
}
void CObjectAttributes::RegisterAttribute(const char* pszAttributeName,void* pAddress,AttributeType Type)
{
	ATTRIBUTE Attrib;
	Attrib.m_Type=Type;
	Attrib.m_p=pAddress;
	UnregisterAttribute(pszAttributeName);
	m_mapAttributes.insert(make_pair(pszAttributeName,Attrib));
}
void CObjectAttributes::UnregisterAttribute(const char* pszAttributeName)
{
	m_mapAttributes.erase(pszAttributeName);
}
bool CObjectAttributes::Write(const char* pszAttributeName,string& strIn)
{
	ATTRIBUTE* pAtt=GetAttribute(pszAttributeName,ATT_STRING);
	if(pAtt)
	{
		*pAtt->m_pstr=strIn;
		return true;
	}
	return false;
}
bool CObjectAttributes::Read(const char* pszAttributeName,string& strOut)
{
	ATTRIBUTE* pAtt=GetAttribute(pszAttributeName,ATT_STRING);
	if(pAtt)
	{
		strOut=*pAtt->m_pstr;
		return true;
	}
	return false;
}
bool CObjectAttributes::Write(const char* pszAttributeName,float fIn)
{
	ATTRIBUTE* pAtt=GetAttribute(pszAttributeName,ATT_FLOAT);
	if(pAtt)
	{
		*pAtt->m_pf=fIn;
		return true;
	}
	return false;
}
bool CObjectAttributes::Read(const char* pszAttributeName,float& fOut)
{
	ATTRIBUTE* pAtt=GetAttribute(pszAttributeName,ATT_FLOAT);
	if(pAtt)
	{
		fOut=*pAtt->m_pf;
		return true;
	}
	return false;
}
bool CObjectAttributes::Write(const char* pszAttributeName,int nIn)
{
	ATTRIBUTE* pAtt=GetAttribute(pszAttributeName,ATT_INT);
	if(pAtt)
	{
		*pAtt->m_pn=nIn;
		return true;
	}
	return false;
}
bool CObjectAttributes::Read(const char* pszAttributeName,int& nOut)
{
	ATTRIBUTE* pAtt=GetAttribute(pszAttributeName,ATT_INT);
	if(pAtt)
	{
		nOut=*pAtt->m_pn;
		return true;
	}
	return false;
}
bool CObjectAttributes::Write(const char* pszAttributeName,VECTOR4D& vIn)
{
	ATTRIBUTE* pAtt=GetAttribute(pszAttributeName,ATT_VECTOR4D);
	if(pAtt)
	{
		*pAtt->m_pv=vIn;
		return true;
	}
	return false;
}
bool CObjectAttributes::Read(const char* pszAttributeName,VECTOR4D& vOut)
{
	ATTRIBUTE* pAtt=GetAttribute(pszAttributeName,ATT_VECTOR4D);
	if(pAtt)
	{
		vOut=*pAtt->m_pv;
		return true;
	}
	return false;
}
bool CObjectAttributes::Write(const char* pszAttributeName,MATRIX4D& mIn)
{
	ATTRIBUTE* pAtt=GetAttribute(pszAttributeName,ATT_MATRIX4D);
	if(pAtt)
	{
		*pAtt->m_pm=mIn;
		return true;
	}
	return false;
}
bool CObjectAttributes::Read(const char* pszAttributeName,MATRIX4D& mOut)
{
	ATTRIBUTE* pAtt=GetAttribute(pszAttributeName,ATT_MATRIX4D);
	if(pAtt)
	{
		mOut=*pAtt->m_pm;
		return true;
	}
	return false;
}
bool CObjectAttributes::Write(const char* pszAttributeName,CObjectBase* pInObject)
{
	ATTRIBUTE* pAtt=GetAttribute(pszAttributeName,ATT_OBJECT);
	if(pAtt)
	{
		SAFE_RELEASE(*pAtt->m_ppObj);
		*pAtt->m_ppObj=pInObject;
		pInObject->AddRef();
		return true;
	}
	return false;
}
bool CObjectAttributes::Read(const char* pszAttributeName,CObjectBase** ppOutObject)
{
	*ppOutObject=NULL;
	ATTRIBUTE* pAtt=GetAttribute(pszAttributeName,ATT_OBJECT);
	if(pAtt)
	{
		*ppOutObject=*pAtt->m_ppObj;
		(*ppOutObject)->AddRef();
		return true;
	}
	return false;
}

void* CObjectAttributes::operator()(const char* pszAttributeName)
{
	auto it=m_mapAttributes.find(pszAttributeName);
	if(it!=m_mapAttributes.end())
		return m_mapAttributes[pszAttributeName].m_p;
	return 0;
}