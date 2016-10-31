#include "StdAfx.h"
#include "Stream.h"


CStream::CStream(void)
{
}


CStream::~CStream(void)
{
}

bool CStream::Write(long& l)
{
	 return write(&l,sizeof(long))==sizeof(long); 
}
bool CStream::Read(long& l)
{
	return read(&l,sizeof(long))==sizeof(long);
}
bool CStream::Write(unsigned long& ul)
{
	 return write(&ul,sizeof(unsigned long))==sizeof(unsigned long); 
}
bool CStream::Read(unsigned long& ul)
{
	return read(&ul,sizeof(unsigned long))==sizeof(unsigned long);
}
bool CStream::Write(float &f)
{
	return write(&f,sizeof(float))==sizeof(float);
}
bool CStream::Read(float& f)
{
	return read(&f,sizeof(float))==sizeof(float);
}
bool CStream::Write(VECTOR4D& V)
{
	return write(&V,sizeof(VECTOR4D))==sizeof(VECTOR4D);
}
bool CStream::Read(VECTOR4D& V)
{
	return read(&V,sizeof(VECTOR4D))==sizeof(VECTOR4D);
}
bool CStream::Write(MATRIX4D& M)
{
	return write(&M,sizeof(MATRIX4D))==sizeof(MATRIX4D);
}
bool CStream::Read(MATRIX4D& M)
{
	return read(&M,sizeof(MATRIX4D))==sizeof(MATRIX4D);
}
bool CStream::Write(string& s)
{
	unsigned long length=s.length();
	if(Write(length))
		return write(&s[0],s.length())==s.length();
	return false;
}
bool CStream::Read(string& s)
{
	unsigned long length=0;
	if(Read(length))
	{
		if(length)
		{
			s.resize(length);
			return read(&s[0],length)==length;
		}
		else
		{
			s="";
			return true;
		}
	}
	return false;
}