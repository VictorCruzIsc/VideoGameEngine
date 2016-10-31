#include "StdAfx.h"
#include "MeshBase.h"
#include "Matrix4d.h"
CMeshBase::CMeshBase(void)
{
	m_lIBID=0;
	m_lVBID=0;
}
CMeshBase::~CMeshBase(void)
{
}
bool CMeshBase::RayIntersect(VECTOR4D& vSrc,VECTOR4D& vRay,multimap<float,unsigned int>& mmapOutIntersectedFaces,ATTRIBUTE_RANGE& Range)
{
	VECTOR4D Ray=vRay;
	VECTOR4D Point;
	Normalize(Ray,vRay);
	unsigned long ulFace=Range.ulFaceStart;
	unsigned long ulIndex=Range.ulFaceStart*3;
	VECTOR4D A,B,Plane;
	for(unsigned ulFaceCount=Range.ulFaceCount;ulFaceCount;ulIndex+=3,ulFace++,ulFaceCount--)
	{
		VECTOR4D &V0=m_vecVertices[Range.ulVertexStart+m_vecIndices[ulIndex]].V;
		VECTOR4D &V1=m_vecVertices[Range.ulVertexStart+m_vecIndices[ulIndex+1]].V;
		VECTOR4D &V2=m_vecVertices[Range.ulVertexStart+m_vecIndices[ulIndex+2]].V;
		Sub(V1,V0,A);
		Sub(V2,V0,B);
		Cross3(A,B,Plane);
		Normalize(Plane,Plane);
		Plane.w=-Dot(Plane,V0);
		float fn,fd;
		RayCastOnPlane(Plane,vRay,vSrc,&fn,&fd);
		if(fabs(fd)<0.00001)
			continue;
		fn=-fn/fd;
		if(fn<0)
			continue;
		ComputeIntersectionPoint(vSrc,Ray,fn,Point);
		if(PointInTriangleBarycentric(V0,V1,V2,Point))
			mmapOutIntersectedFaces.insert(make_pair(fn,ulFace));
	}
	return mmapOutIntersectedFaces.size()>0;
}
bool CMeshBase::Serialize(CStream& io,bool bSave,CObjectFactory& Factory)
{
	unsigned long ulCount=0;
	if(bSave)
	{
		ulCount=m_vecVertices.size();
		io.write((char*)&ulCount,sizeof(unsigned long));
		io.write((char*)&m_vecVertices[0],ulCount*sizeof(VERTEX));
		ulCount=m_vecIndices.size();
		io.write((char*)&ulCount,sizeof(unsigned long));
		io.write((char*)&m_vecIndices[0],ulCount*sizeof(unsigned long));
		ulCount=m_vecAttributes.size();
		io.write((char*)&ulCount,sizeof(unsigned long));
		io.write((char*)&m_vecAttributes[0],ulCount*sizeof(ATTRIBUTE_RANGE));
		ulCount=m_vecMaterials.size();
		io.Write(ulCount);
		for(auto itM=m_vecMaterials.begin();itM!=m_vecMaterials.end();itM++)
		{
			io.write(&(*itM),sizeof(MATERIAL));
			io.Write(itM->lShaderID);
			ulCount=itM->vecTexturesFileNames.size();
			io.Write(ulCount);
			for(auto itT=itM->vecTexturesFileNames.begin();itT!=itM->vecTexturesFileNames.end();itT++)
				io.Write(*itT);
		}
	}
	else
	{
		m_vecVertices.clear();
		m_vecIndices.clear();
		m_vecMaterials.clear();
		m_vecAttributes.clear();
		unsigned long ulMaterials;
		m_vecVertices.clear();
		m_vecIndices.clear();

		io.read((char*)&ulCount,sizeof(unsigned long));
		m_vecVertices.resize(ulCount);
		io.read((char*)&m_vecVertices[0],ulCount*sizeof(VERTEX));

		io.read((char*)&ulCount,sizeof(unsigned long));
		m_vecIndices.resize(ulCount);
		io.read((char*)&m_vecIndices[0],ulCount*sizeof(unsigned long));

		io.read((char*)&ulCount,sizeof(unsigned long));
		m_vecAttributes.resize(ulCount);
		io.read((char*)&m_vecAttributes[0],ulCount*sizeof(ATTRIBUTE_RANGE));

		io.Read(ulMaterials);
		m_vecMaterials.resize(ulMaterials);
		for(unsigned long m=0;m<ulMaterials;m++)
		{
			XMATERIAL& M=m_vecMaterials[m];
			io.read(&M,sizeof(MATERIAL));
			io.Read(M.lShaderID);
			io.Read(ulCount);
			M.vecTexturesFileNames.resize(ulCount);
			for(unsigned long i=0;i<ulCount;i++)
			{
				string strTextureFileName;
				io.Read(strTextureFileName);
				M.vecTexturesFileNames[i]=strTextureFileName;
			}
		}
	}
	return true;
}

bool CMeshBase::CalcBoundingBox(vector<ATTRIBUTE_RANGE>& vecInAttributeRanges,VECTOR4D& Min,VECTOR4D& Max)
{
	CMeshBase* pBox=0;
	if(!vecInAttributeRanges.size())
		return false; //No hay atributos que procesar.
	//Extraer el la muestra inicial
	ATTRIBUTE_RANGE* pRange=&vecInAttributeRanges[0]; //Desde el primer atributo en el contenedor tomaremos la muestra inicial de posición
	Min=Max=m_vecVertices[pRange->ulVertexStart+m_vecIndices[pRange->ulFaceStart*3+0]].V;
	//Ya tenemos la muestra ahora podemos comparar en búsqueda de máximos y mínimos.
	for(unsigned int uiAttID=0;uiAttID<vecInAttributeRanges.size();uiAttID++)
	{
		//Por cada cara en el rango
		pRange=&vecInAttributeRanges[uiAttID];
		unsigned int uiBaseVertex=pRange->ulVertexStart;
		unsigned int uiBaseIndex=pRange->ulFaceStart*3;
		for(unsigned int uiFace=pRange->ulFaceCount;uiFace;uiFace--)
		{
			for(int i=0;i<3;i++)
			{
				VECTOR4D& Position=m_vecVertices[uiBaseVertex+m_vecIndices[uiBaseIndex++]].V;
				Max.x=max(Max.x,Position.x);
				Max.y=max(Max.y,Position.y);
				Max.z=max(Max.z,Position.z);
				Min.x=min(Min.x,Position.x);
				Min.y=min(Min.y,Position.y);
				Min.z=min(Min.z,Position.z);
			}
		}
	}
	return true;
}


CMeshBase* CMeshBase::CreateBox(VECTOR4D& Min,VECTOR4D& Max)
{
	CMeshBase* pMesh=new CMeshBase;
	static unsigned long CubeIndex[]=
	{
		0,3,1,
		1,3,2,
		3,7,2,
		2,7,6,
		7,4,6,
		6,4,5,
		2,6,1,
		1,6,5,
		1,5,0,
		0,5,4,
		0,4,3,
		3,4,7
	};
	pMesh->m_vecIndices.resize(3*12);
	memcpy(&pMesh->m_vecIndices[0],CubeIndex,sizeof(CubeIndex));
	pMesh->m_vecVertices.resize(8);
	memset(&pMesh->m_vecVertices[0],0,sizeof(VERTEX)*8);
	pMesh->m_vecVertices[0].V=VECTOR4D(Min.x,Min.y,Min.z,1);
	pMesh->m_vecVertices[1].V=VECTOR4D(Min.x,Min.y,Max.z,1);
	pMesh->m_vecVertices[2].V=VECTOR4D(Max.x,Min.y,Max.z,1);
	pMesh->m_vecVertices[3].V=VECTOR4D(Max.x,Min.y,Min.z,1);
	pMesh->m_vecVertices[4].V=VECTOR4D(Min.x,Max.y,Min.z,1);
	pMesh->m_vecVertices[5].V=VECTOR4D(Min.x,Max.y,Max.z,1);
	pMesh->m_vecVertices[6].V=VECTOR4D(Max.x,Max.y,Max.z,1);
	pMesh->m_vecVertices[7].V=VECTOR4D(Max.x,Max.y,Min.z,1);
	//0.5773f -0.5773f
	pMesh->m_vecVertices[0].N=VECTOR4D(-0.5773f,-0.5773f,-0.5773f,0);
	pMesh->m_vecVertices[1].N=VECTOR4D(-0.5773f,-0.5773f,0.5773f,0);
	pMesh->m_vecVertices[2].N=VECTOR4D(0.5773f,-0.5773f,0.5773f,0);
	pMesh->m_vecVertices[3].N=VECTOR4D(0.5773f,-0.5773f,-0.5773f,0);
	pMesh->m_vecVertices[4].N=VECTOR4D(-0.5773f,0.5773f,-0.5773f,0);
	pMesh->m_vecVertices[5].N=VECTOR4D(-0.5773f,0.5773f,0.5773f,0);
	pMesh->m_vecVertices[6].N=VECTOR4D(0.5773f,0.5773f,0.5773f,0);
	pMesh->m_vecVertices[7].N=VECTOR4D(0.5773f,0.5773f,-0.5773f,0);

	pMesh->m_vecVertices[0].u=0;
	pMesh->m_vecVertices[0].v=0;
	pMesh->m_vecVertices[1].u=1;
	pMesh->m_vecVertices[1].v=0;
	pMesh->m_vecVertices[2].u=1;
	pMesh->m_vecVertices[2].v=1;
	pMesh->m_vecVertices[3].u=0;
	pMesh->m_vecVertices[3].v=1;

	pMesh->m_vecVertices[6].u=0;
	pMesh->m_vecVertices[6].v=0;
	pMesh->m_vecVertices[7].u=1;
	pMesh->m_vecVertices[7].v=0;
	pMesh->m_vecVertices[4].u=1;
	pMesh->m_vecVertices[4].v=1;
	pMesh->m_vecVertices[5].u=0;
	pMesh->m_vecVertices[5].v=1;

	ATTRIBUTE_RANGE Range;
	Range.ulAttribID=0;
	Range.ulFaceCount=12;
	Range.ulFaceStart=0;
	Range.ulVertexStart=0;
	Range.ulVertexCount=8;
	pMesh->m_vecAttributes.push_back(Range);
	XMATERIAL XMat;
	XMat.vAmbient=VECTOR4D(0.7f,0.7f,0.7f,1);
	XMat.vDiffuse=VECTOR4D(0.7f,0.7f,0.7f,1);
	XMat.vEmissive=VECTOR4D(0,0,0,0);
	XMat.vSpecular=VECTOR4D(1,1,1,1);
	XMat.fPower=20;
	pMesh->m_vecMaterials.push_back(XMat);
	return pMesh;
}


void CMeshBase::BuildTangentSpaceFromTexCoordsIndexed(void)
{
	vector<int> Histogram;
	m_vecXVertices.clear();
	m_vecXVertices.resize(m_vecVertices.size());
	Histogram.resize(m_vecVertices.size());
	memset(&Histogram[0],0,sizeof(int)*Histogram.size());

	for(unsigned int i=0;i<m_vecVertices.size();i++)
	{
			memcpy(&m_vecXVertices[i],&m_vecVertices[i],sizeof(VERTEX));
			m_vecXVertices[i].vNormal=VECTOR4D(0,0,0,0);
			m_vecXVertices[i].vTangent=VECTOR4D(0,0,0,0);
			m_vecXVertices[i].vBinormal=VECTOR4D(0,0,0,0);
	}
	for(unsigned long ulSubset=0;ulSubset<m_vecAttributes.size();ulSubset++)
	{
		ATTRIBUTE_RANGE Att=m_vecAttributes[ulSubset];
		XVERTEX * pXVertices=&m_vecXVertices[Att.ulVertexStart];
		unsigned long* pIndices=&m_vecIndices[Att.ulFaceStart*3];
		int* pHistogram=&Histogram[Att.ulVertexStart];
		for(unsigned int i=0;i<Att.ulFaceCount*3;i+=3)
		{
			VECTOR4D V0,V1,V2,T0,T1,T2,T,B;
			MATRIX4D InvS,Mq,Mt;
			MATRIX4D::Identity(InvS);
			MATRIX4D::Identity(Mq);
			//Tomar un triangulo
			V0=pXVertices[pIndices[i]].V;
			V1=pXVertices[pIndices[i+1]].V;
			V2=pXVertices[pIndices[i+2]].V;
			//y sus coordenadas de textura para formar la base ortornormal en espacio de vértice (espacio tangente)
			T0=VECTOR4D(pXVertices[pIndices[i]].u,pXVertices[pIndices[i]].v,0,0);
			T1=VECTOR4D(pXVertices[pIndices[i+1]].u,pXVertices[pIndices[i+1]].v,0,0);
			T2=VECTOR4D(pXVertices[pIndices[i+2]].u,pXVertices[pIndices[i+2]].v,0,0);

			Sub(V1,V0,(VECTOR4D&)Mq.vec[0]);
			Sub(V2,V0,(VECTOR4D&)Mq.vec[1]);
			Sub(T1,T0,(VECTOR4D&)InvS.vec[0]);
			Sub(T2,T0,(VECTOR4D&)InvS.vec[1]);
			float det=MATRIX4D::Inverse(InvS,InvS);
			Mt=InvS*Mq;
			for(int j=0;j<3;j++)
			{
				VECTOR4D temp1;
				VECTOR4D N=(VECTOR4D&)pXVertices[pIndices[i+j]].N; //Normal de vértice
				VECTOR4D T=(VECTOR4D&)Mt.vec[0];
				VECTOR4D B=(VECTOR4D&)Mt.vec[1];
				//Ortogonalización con respecto a la normal de vértice
				Normalize(N,N);
				Sub(T,MulS(N,Dot(N,T),temp1),T);
				Normalize(T,T);
				VECTOR4D temp2;
				Sub(B,MulS(N,Dot(N,B),temp1),temp2);
				Sub(temp2,MulS(T,Dot(T,B),temp1),B);
				Normalize(B,B);
				Add(pXVertices[pIndices[i+j]].vNormal,N,pXVertices[pIndices[i+j]].vNormal);
				Add(pXVertices[pIndices[i+j]].vBinormal,B,pXVertices[pIndices[i+j]].vBinormal);
				Add(pXVertices[pIndices[i+j]].vTangent,T,pXVertices[pIndices[i+j]].vTangent);
				pHistogram[pIndices[i+j]]++;
			}
		}
		for(unsigned int i=0;i<Att.ulVertexCount;i++)
		{
				float invFreq=1.0f/pHistogram[i];
				MulS(pXVertices[i].vNormal,invFreq,pXVertices[i].vNormal);
				MulS(pXVertices[i].vTangent,invFreq,pXVertices[i].vTangent);
				MulS(pXVertices[i].vBinormal,invFreq,pXVertices[i].vBinormal);
				Normalize(pXVertices[i].vNormal,pXVertices[i].vNormal);
				Normalize(pXVertices[i].vTangent,pXVertices[i].vTangent);
				Normalize(pXVertices[i].vBinormal,pXVertices[i].vBinormal);
		}
	}
}


CMeshBase* CMeshBase::Join(MATRIX4D& XA,CMeshBase* pMeshA,MATRIX4D& XB, CMeshBase* pMeshB)
{
	CMeshBase* pC=new CMeshBase;
	//Collect Materials

	pC->m_vecMaterials.resize(pMeshA->m_vecMaterials.size()+pMeshB->m_vecMaterials.size());
	pC->m_vecAttributes.resize(pMeshA->m_vecAttributes.size()+pMeshB->m_vecAttributes.size());
	pC->m_vecVertices.resize(pMeshA->m_vecVertices.size()+pMeshB->m_vecVertices.size());
	pC->m_vecIndices.resize(pMeshA->m_vecIndices.size()+pMeshB->m_vecIndices.size());

	unsigned long ulVertexOffset;
	unsigned long ulIndexOffset;
	unsigned long ulMaterialOffset;
	unsigned long ulAttributeOffset;
	ulVertexOffset=pMeshA->m_vecVertices.size();
	ulIndexOffset=pMeshA->m_vecIndices.size();
	ulMaterialOffset=pMeshA->m_vecMaterials.size();
	ulAttributeOffset=pMeshA->m_vecAttributes.size();
	
	//Transfer Attrubute sets
	for(unsigned long i=0;i<pMeshA->m_vecAttributes.size();i++)
		pC->m_vecAttributes[i]=pMeshA->m_vecAttributes[i];
	for(unsigned long i=0;i<pMeshB->m_vecAttributes.size();i++)
	{
		ATTRIBUTE_RANGE Attribute=pMeshB->m_vecAttributes[i];
		Attribute.ulAttribID+=ulMaterialOffset;
		Attribute.ulFaceStart+=ulIndexOffset/3;
		Attribute.ulVertexStart+=ulVertexOffset;
		pC->m_vecAttributes[i+ulAttributeOffset]=Attribute;
	}
	//Transfer Materials
	for(unsigned long i=0;i<pMeshA->m_vecMaterials.size();i++)
		pC->m_vecMaterials[i]=pMeshA->m_vecMaterials[i];
	for(unsigned long i=0;i<pMeshB->m_vecMaterials.size();i++)
		pC->m_vecMaterials[i+ulMaterialOffset]=pMeshB->m_vecMaterials[i];
	
	//Transfer geometry
	memcpy(&pC->m_vecIndices[0],&pMeshA->m_vecIndices[0],sizeof(unsigned long)*pMeshA->m_vecIndices.size());
	memcpy(&pC->m_vecIndices[ulIndexOffset],&pMeshB->m_vecIndices[0],sizeof(unsigned long)*pMeshB->m_vecIndices.size());
	memcpy(&pC->m_vecVertices[0],&pMeshA->m_vecVertices[0],sizeof(VERTEX)*pMeshA->m_vecVertices.size());
	memcpy(&pC->m_vecVertices[ulVertexOffset],&pMeshB->m_vecVertices[0],sizeof(VERTEX)*pMeshB->m_vecVertices.size());

	//Transform geometry

	for(unsigned long i=0;i<pMeshA->m_vecVertices.size();i++)
	{
		pC->m_vecVertices[i].V=pC->m_vecVertices[i].V*XA;
		pC->m_vecVertices[i].N=pC->m_vecVertices[i].N*XA;
		Normalize(pC->m_vecVertices[i].N,pC->m_vecVertices[i].N);
	}
	for(unsigned long i=ulVertexOffset ; i<(ulVertexOffset+pMeshB->m_vecVertices.size()); i++)
	{
		pC->m_vecVertices[i].V=pC->m_vecVertices[i].V*XB;
		pC->m_vecVertices[i].N=pC->m_vecVertices[i].N*XB;
		Normalize(pC->m_vecVertices[i].N,pC->m_vecVertices[i].N);
	}
	return pC;
}

void CMeshBase::RemoveTriangle(unsigned long ulFace)
{
	for(unsigned long ulAtt=0;ulAtt<m_vecAttributes.size();ulAtt++)
	{
		ATTRIBUTE_RANGE& Att=m_vecAttributes[ulAtt];
		if(ulFace < Att.ulFaceStart )
		{
			Att.ulFaceStart--;
		}
		else if((ulFace >= Att.ulFaceStart )&&(ulFace < (Att.ulFaceStart+Att.ulFaceCount)))
		{
			Att.ulFaceCount--;
		}
	}
	m_vecIndices.erase(m_vecIndices.begin()+ulFace*3,m_vecIndices.begin()+ulFace*3+3);
}
unsigned long CMeshBase::RemoveNearToZeroTriangles(float fEpsilon)
{

	//Eliminar triángulos cuya area se cercana a cero
	unsigned long ulRemovedFaces=0;
	unsigned long ulIndex=0;
	unsigned long ulFaces=m_vecIndices.size()/3;
	while(ulFaces--)
	{
		VECTOR4D A,B,V0,V1,V2,N;
		unsigned ulVertexStart=GetVertexStart(ulIndex/3);
		V0=m_vecVertices[ulVertexStart+m_vecIndices[ulIndex]].V;
		V1=m_vecVertices[ulVertexStart+m_vecIndices[ulIndex+1]].V;
		V2=m_vecVertices[ulVertexStart+m_vecIndices[ulIndex+2]].V;
		Sub(V1,V0,A);
		Sub(V2,V0,B);
		Cross3(A,B,N);
		if(sqrtf(SquareMag(N))<fEpsilon)
		{
			RemoveTriangle(ulIndex/3);
			ulRemovedFaces++;
		}
		else
			ulIndex+=3;
	}
	m_vecIndices.shrink_to_fit();
	return ulRemovedFaces;
}

unsigned long CMeshBase::GetVertexStart(unsigned long ulFace)
{
	for(unsigned long i=0;i<m_vecAttributes.size();i++)
	{
		ATTRIBUTE_RANGE& Att=m_vecAttributes[i];
		if((ulFace >= Att.ulFaceStart) && (ulFace <( Att.ulFaceStart+Att.ulFaceCount)))
			return Att.ulVertexStart;
	}
	return -1;
}
bool CMeshBase::BuildIndex16Buffer(void)
{
	if(this->m_vecVertices.size()<65534)
	{
		m_vecIndices16.resize(m_vecIndices.size());
		for(unsigned int i=0;i<m_vecIndices16.size();i++)
		{
			m_vecIndices16[i]=(unsigned short)m_vecIndices[i];
		}
		return true;
	}
	return false;
}