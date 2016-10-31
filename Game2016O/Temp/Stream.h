#pragma once
#include "Matrix4D.h"
#include <string>
using namespace std;
class CStream
{
public:
	CStream(void);
	virtual ~CStream(void);
	virtual unsigned long write(void* pInDataToWrite,unsigned long ulByteCount)=0;
	virtual unsigned long read(void* pOutDataToRead,unsigned long ulByteCount)=0;
	
	bool Write(long& l);
	bool Read(long& l);
	bool Write(unsigned long& ul);
	bool Read(unsigned long& ul);
	bool Write(float &f);
	bool Read(float& f);
	bool Write(VECTOR4D& V);
	bool Read(VECTOR4D& V);
	bool Write(MATRIX4D& M);
	bool Read(MATRIX4D& M);
	bool Write(string& s);
	bool Read(string& s);
};

