#pragma once
#include "stream.h"
#include <fstream>
using namespace std;
class CStreamFile :
	public CStream
{
	fstream m_fStream;
public:
	virtual unsigned long write(void* pInDataToWrite,unsigned long ulByteCount);
	virtual unsigned long read(void* pOutDataToRead,unsigned long ulByteCount);
	bool OpenFileWrite(const char* pszFileName);
	bool OpenFileRead(const char* pszFileName);
	void Close();
	CStreamFile(void);
	~CStreamFile(void);
};

