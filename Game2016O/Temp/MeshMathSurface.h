#pragma once
#include "object3d.h"
#include "MeshBase.h"
class CMeshMathSurface :
	public CMeshBase
{
protected:
	unsigned long m_ulVz;
	unsigned long m_ulVx;
	void TesselateSurface(void);
public:
	virtual bool Serialize(CStream &io,bool bSave,CObjectFactory &Factory);
	unsigned long GetVertexCountZ(){return m_ulVz;};
	unsigned long GetVertexCountX(){return m_ulVx;};
	CMeshMathSurface(void);
	virtual ~CMeshMathSurface(void);
	bool BuildSurface (unsigned long ulVz, unsigned long ulVx, float z0, float x0, float dz, float dx, 
		float (*pFn)(float z, float x,void* pParam), VECTOR4D (*pFnGradient)(float x, float y, float z,void* pParam),void* pParam);
	bool BuildParametricSurface (unsigned long ulVu, unsigned long ulVv, float u0, float v0, float du, float dv, 
		VECTOR4D (*pFn)(float u, float v,void* pParam), VECTOR4D(*pFnGradient)(float x, float y, float z,void* pParam),void* pParam);
	void GenerateTextureCoordinates(float u0, float v0, float du, float dv);

	struct TORUS_PARAMS
	{
		float r0;
		float r1;
		float u,v;
	};
	struct DISC_PARAMS
	{
		float rint;
		float rext;
		float u,v;
	};
	static VECTOR4D Sphere(float u,float v,void* pParams);
	static VECTOR4D SphereGradient(float x,float y,float z,void* pParams);
	static VECTOR4D Torus(float u,float v,void* pParams);
	static VECTOR4D TorusGradient(float x,float y,float z,void* pParams);
	static VECTOR4D Cylinder(float u,float v,void* pParams);
	static VECTOR4D CylinderGradient(float x,float y,float z,void* pParams);
	static VECTOR4D Disc(float u,float v,void* pParams);
	static VECTOR4D DiscGradient(float x,float y,float z,void* pParams);
};