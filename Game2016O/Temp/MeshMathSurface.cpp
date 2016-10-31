#include "StdAfx.h"
#include "MeshMathSurface.h"

CMeshMathSurface::CMeshMathSurface(void)
{
	m_ulVx = 0;
	m_ulVz = 0;
}
CMeshMathSurface::~CMeshMathSurface(void)
{
}

void CMeshMathSurface::TesselateSurface(void)
{
	m_vecIndices.clear();
	m_vecIndices.resize((m_ulVz-1)*(m_ulVx-1)*3*2);
	unsigned long * pIndex = &m_vecIndices[0];
	for(unsigned long ix = 0; ix < (m_ulVx-1); ix++)
	{
		for (unsigned long iz = 0; iz < (m_ulVz-1); iz++)
		{
			pIndex[0] = ix*m_ulVz+iz;
			pIndex[1] = ix*m_ulVz+(iz+1);
			pIndex[2] = (ix+1)*m_ulVz+iz;
			pIndex[3] = pIndex[2];
			pIndex[4] = pIndex[1];
			pIndex[5] = (ix+1)*m_ulVz+(iz+1);
			pIndex+=6;
		}
	}
	//Build Attribute Range

	ATTRIBUTE_RANGE at;
	at.ulAttribID=0;
	at.ulFaceStart=0;
	at.ulVertexStart=0;
	at.ulFaceCount=(m_ulVz-1)*(m_ulVx-1)*2;
	at.ulVertexCount=m_vecVertices.size();
	m_vecAttributes.clear();
	m_vecAttributes.push_back(at);
	XMATERIAL Mat;
	Mat.fPower=20;
	Mat.vAmbient=VECTOR4D(0.7f,0.7f,0.7f,1);
	Mat.vDiffuse=VECTOR4D(0.7f,0.7f,0.7f,1);
	Mat.vEmissive=VECTOR4D(0,0,0,0);
	Mat.vSpecular=VECTOR4D(1,1,0.8f,1);
	Mat.lShaderID=0;
	m_vecMaterials.clear();
	m_vecMaterials.push_back(Mat);
}



void CMeshMathSurface::GenerateTextureCoordinates(float u0, float v0, float du, float dv)
{
	VERTEX * pVertex = &m_vecVertices[0];
	float u, v;
	for (unsigned int ix = 0; ix < m_ulVx; ix++)
	{
		v=v0+ix*dv;
		for( unsigned int iz = 0; iz < m_ulVz; iz++)
		{
			u=u0+iz*du;
			pVertex->u = u;
			pVertex->v = v;
			pVertex++;
		}
	}
}

bool CMeshMathSurface::Serialize(CStream &io,bool bSave,CObjectFactory &Factory)
{
	if(!__super::Serialize(io,bSave,Factory))
		return false;
	if(bSave)
	{
		io.write((char*)&m_ulVz,sizeof(unsigned long));
		io.write((char*)&m_ulVx,sizeof(unsigned long));
	}
	else
	{
		io.read((char*)&m_ulVz,sizeof(unsigned long));
		io.read((char*)&m_ulVx,sizeof(unsigned long));
	}
	return true;
}

bool CMeshMathSurface::BuildSurface(unsigned long ulVz, unsigned long ulVx, float z0, float x0, float dz, float dx, 
		float (*pFn)(float z, float x,void* pParam), VECTOR4D (*pFnGradient)(float x, float y, float z,void* pParams),void* pParam)
{
	if( (ulVz<2) || (ulVx<2))
		return false;
	m_ulVz = ulVz;
	m_ulVx = ulVx;
	m_vecVertices.clear();
	m_vecIndices.clear();
	m_vecVertices.resize(m_ulVz*m_ulVx);
	TesselateSurface();
	float x,y,z;
	VERTEX* pVertex = &m_vecVertices[0];
	for (unsigned int ix = 0; ix < m_ulVx; ix++)
	{
		x=x0+dx*ix;
		for( unsigned int iz = 0; iz < m_ulVz; iz++)
		{
			z = z0+dz*iz;
			y = pFn(z,x,pParam);
			pVertex->V=VECTOR4D(x,y,z,1);
			VECTOR4D Normal = pFnGradient(x,y,z,pParam);
			Normalize(Normal,Normal);
			pVertex->N= Normal;
			pVertex++;
		}
	}
	return true;
}
bool CMeshMathSurface::BuildParametricSurface (unsigned long ulVu, unsigned long ulVv, float u0, float v0, float du, float dv, 
		VECTOR4D (*pFn)(float u, float v,void* pParam), VECTOR4D(*pFnGradient)(float x, float y, float z,void* pParam),void* pParam)
 {
	if( (ulVu<2) || (ulVv<2))
		return false;
	m_ulVx = ulVv;
	m_ulVz = ulVu;
	m_vecVertices.clear();
	m_vecIndices.clear();
	m_vecVertices.resize(m_ulVz*m_ulVx);
	TesselateSurface();
	float u;
	float v;
	VERTEX* pVertex = &m_vecVertices[0];
	for (unsigned int ix = 0; ix < m_ulVx; ix++)
	{
		v = v0+ix*dv;
		for( unsigned int iz = 0; iz < m_ulVz; iz++)
		{
			u=u0+iz*du;
			VECTOR4D Position=pFn(u,v,pParam);
			pVertex->V = Position;
			VECTOR4D Normal= pFnGradient(Position.x,Position.y,Position.z,pParam);
			Normalize(Normal,Normal);
			pVertex->N=Normal;
			pVertex++;
		}
	}
	return true;
 }



VECTOR4D CMeshMathSurface::Sphere(float u,float v,void*)
{
	return VECTOR4D(sin(3.14159265*u)*sin(2*3.14159265*v),
		cos(3.14159265*u),
		sin(3.14159265*u)*cos(2*3.14159265*v),1);
}
VECTOR4D CMeshMathSurface::SphereGradient(float x,float y,float z,void*)
{
	VECTOR4D N=VECTOR4D(x,y,z,0);
	return Normalize(N,N);
}
VECTOR4D CMeshMathSurface::Disc(float u,float v,void* pParam)
{
	DISC_PARAMS* pDisc=(DISC_PARAMS*)pParam;
	MATRIX4D R;
	MATRIX4D::RotationY(2*3.14159265f*v,R);
	VECTOR4D P(0,0,pDisc->rint+(pDisc->rext-pDisc->rint)*u,1);
	return P*R;
}
VECTOR4D CMeshMathSurface::DiscGradient(float x,float y,float z,void *)
{
	return VECTOR4D(0,1,0,0);
}


VECTOR4D CMeshMathSurface::Torus(float u,float v,void* pParams)
{
	TORUS_PARAMS* pTorus=(TORUS_PARAMS*)pParams;
	MATRIX4D R;
	MATRIX4D::RotationY(2*3.14159265f*v,R);
	VECTOR4D P(0,pTorus->r0*cos(2*3.14159265f*u),pTorus->r0*sin(2*3.14159265f*u)+pTorus->r1,1);
	pTorus->u=u;
	pTorus->v=v;
	return P*R;
}
VECTOR4D CMeshMathSurface::TorusGradient(float x,float y,float z,void* pParams)
{
	TORUS_PARAMS* pTorus=(TORUS_PARAMS*)pParams;
	MATRIX4D R;
	MATRIX4D::RotationY(2*3.14159265f*pTorus->v,R);
	VECTOR4D N(0,cos(2*3.14159265f*pTorus->u),sin(2*3.14159265f*pTorus->u),0);
	N=N*R;
	return N;
}
VECTOR4D CMeshMathSurface::Cylinder(float u,float v,void* pParams)
{
	MATRIX4D R;
	MATRIX4D::RotationY(2*3.14159265f,R);
	VECTOR4D P=VECTOR4D(cos(2*3.14159265f*u),2*((1-v) - 0.5f),sin(2*3.14159265f*u),1)*R;
	return P;
}
VECTOR4D CMeshMathSurface::CylinderGradient(float x,float y,float z,void* pParams)
{
	return VECTOR4D(x,0.0f,z,0.0f);
}



