#include "StdAfx.h"
#include "StreamFile.h"


CStreamFile::CStreamFile(void)
{
}


CStreamFile::~CStreamFile(void)
{
}
unsigned long CStreamFile::write(void* pInDataToWrite,unsigned long ulByteCount)
{
	if(!m_fStream.is_open())
		return 0;
	m_fStream.write((char*)pInDataToWrite,ulByteCount);
	return ulByteCount;
}
unsigned long CStreamFile::read(void* pOutDataToRead,unsigned long ulByteCount)
{
	if(!m_fStream.is_open())
		return 0;
	m_fStream.read((char*)pOutDataToRead,ulByteCount);
	return ulByteCount;
}

bool CStreamFile::OpenFileRead(const char* pszFileName)
{
	m_fStream.open(pszFileName,ios::in|ios::binary);
	return m_fStream.is_open();
}
bool CStreamFile::OpenFileWrite(const char* pszFileName)
{
	m_fStream.open(pszFileName,ios::out|ios::binary);
	return m_fStream.is_open();
}