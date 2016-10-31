#pragma once
#include <string>
#include <map>
using namespace std;
class CObjectBase;
class VECTOR4D;
class MATRIX4D;
enum AttributeType{ATT_VOID=0,ATT_VECTOR4D,ATT_MATRIX4D,ATT_FLOAT,ATT_INT,ATT_STRING,ATT_OBJECT,ATT_INVALID=-1l};
class CObjectAttributes
{
	struct ATTRIBUTE
	{
		AttributeType m_Type;
		union
		{
			void   *m_p;  
			string *m_pstr;
			float  *m_pf;
			int    *m_pn;
			VECTOR4D* m_pv;
			MATRIX4D* m_pm;
			CObjectBase** m_ppObj;
		};
	};
	map<string,ATTRIBUTE> m_mapAttributes;
	ATTRIBUTE* GetAttribute(const char* pszAttributeName,AttributeType TypeToCheck);	
public:
	void* operator()(const char* pszAttributeName);
	void RegisterAttribute(const char* pszAttributeName,void* pAddress,AttributeType Type);
	void UnregisterAttribute(const char* pszAttributeName);
	AttributeType GetAttributeType(const char* pszAttributeName);
	bool Write(const char* pszAttributeName,string& strIn);
	bool Read(const char* pszAttributeName,string& strOut);
	bool Write(const char* pszAttributeName,float fIn);
	bool Read(const char* pszAttributeName,float& fOut);
	bool Write(const char* pszAttributeName,int nIn);
	bool Read(const char* pszAttributeName,int& nOut);
	bool Write(const char* pszAttributeName,VECTOR4D& vIn);
	bool Read(const char* pszAttributeName,VECTOR4D& vOut);
	bool Write(const char* pszAttributeName,MATRIX4D& mIn);
	bool Read(const char* pszAttributeName,MATRIX4D& mOut);
	bool Write(const char* pszAttributeName,CObjectBase* pInObject);
	bool Read(const char* pszAttributeName,CObjectBase** ppOutObject);
	CObjectAttributes(void);
	~CObjectAttributes(void);
};

