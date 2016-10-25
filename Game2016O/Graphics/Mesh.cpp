#include "stdafx.h"
#include "Mesh.h"
CMesh::CMesh()
{
}
CMesh::~CMesh()
{
}
bool CMesh::RayCast(VECTOR4D& RayOrigin,
	VECTOR4D & RayDir,
	multimap<float,INTERSECTIONINFO>& Faces //Lista de caras que se intersectan
	)
{
	unsigned long nFaces = m_Indices.size() / 3;
	unsigned long nBaseIndex = 0;
	unsigned long nIntersectedFaces = 0;
	for (unsigned long iFace = 0; iFace < nFaces;iFace++)
	{
		VECTOR4D &V0 = m_Vertices[m_Indices[nBaseIndex+0]].Position;
		VECTOR4D &V1 = m_Vertices[m_Indices[nBaseIndex+1]].Position;
		VECTOR4D &V2 = m_Vertices[m_Indices[nBaseIndex+2]].Position;
		VECTOR4D Intersection;
		if (RayCastOnTriangle(V0, V1, V2, RayOrigin, RayDir, Intersection))
		{
			float dist = Magnity(Intersection - RayOrigin);
			INTERSECTIONINFO II;
			II.Face = iFace;
			II.LocalPosition = Intersection;
			Faces.insert(make_pair(dist, II));
			nIntersectedFaces++;
		}
		nBaseIndex += 3;
	}
	return nIntersectedFaces!=0;
}

void CMesh::VertexShade(CDXBasicPainter::VERTEX(*pVS)(CDXBasicPainter::VERTEX V))
{
	for (unsigned long int i = 0; i < m_Vertices.size(); i++)
		m_Vertices[i] = pVS(m_Vertices[i]);
}

bool RaySphereIntersect(
	VECTOR4D& RayOrigin,
	VECTOR4D& RayDirection, VECTOR4D& SphereCenter,
	float r)
{
	VECTOR4D RO = RayOrigin - SphereCenter;
	float a = Dot(RayDirection, RayDirection);
	float b = 2 * Dot(RayDirection, RO);
	float c = Dot(RO, RO) - r*r;
	float disc = b*b - 4*a*c;
	if (disc < 0)
		return false;
	return true;
}
bool CMesh::RayCast(
	VECTOR4D& RayOrigin,
	VECTOR4D& RayDir,
	multimap<float, unsigned long>& Vertices, float radius)
{
	for (unsigned long i = 0; i < m_Vertices.size(); i++)
	{
		if (RaySphereIntersect(RayOrigin, RayDir, m_Vertices[i].Position, radius))
		{
			float dist = Magnity(m_Vertices[i].Position - RayOrigin);
			Vertices.insert(make_pair(dist, i));
		}
	}
	return Vertices.size()!=0;
}
#include <fstream>
using namespace std;
void CMesh::LoadSuzzane()
{
	fstream in;
	m_Indices.clear();
	m_Vertices.clear();
	in.open("..\\Assets\\Monkey.vertex",
		ios::in | ios::binary);
	if (!in.is_open())
		return;
	unsigned long ulVertices;
	in.read((char*)&ulVertices, sizeof(unsigned long));
	m_Vertices.resize(ulVertices);
	for (unsigned long i = 0; i < ulVertices; i++)
	{
		VECTOR4D V;
		in.read((char*)&V, sizeof(VECTOR4D));
		m_Vertices[i].Position = V;
	}
	in.close();
	in.open("..\\Assets\\Monkey.normal",
		ios::in|ios::binary);
	if (!in.is_open())
		return;
	for (unsigned long i = 0; i < ulVertices; i++)
		in.read((char*)&m_Vertices[i].Normal, 
		sizeof(VECTOR4D));
	in.close();
	in.open("..\\Assets\\Monkey.index", 
		ios::in | ios::binary);
	if (!in.is_open())
		return;
	unsigned long ulIndices;
	in.read((char*)&ulIndices, sizeof(unsigned long));
	m_Indices.resize(ulIndices);
	in.read((char*)&m_Indices[0], sizeof(unsigned long)*
		ulIndices);
	in.close();
	//Generar el espacio de coordenadas de textura
	//Estrategia, vamos a generar el espacio de textura 
	//a partir de las posiciones de los vértices.
	for (unsigned long i = 0; i < m_Vertices.size(); i++)
	{
		VECTOR4D TexCoord = { 0, 0, 0, 0 };
		TexCoord.x = m_Vertices[i].Position.x;
		TexCoord.y = m_Vertices[i].Position.z;
		TexCoord.z = m_Vertices[i].Position.y;
		TexCoord = Normalize(TexCoord);
		TexCoord.x = TexCoord.x*0.5f+0.5f;
		TexCoord.y = TexCoord.y*0.5f+0.5f;
		TexCoord.z = 0;
		m_Vertices[i].TexCoord = TexCoord;
	}
	return;
}

void CMesh::BuildTangentSpaceFromTexCoordsIndexed(bool bGenerateNormal)
{
	vector<int> Histogram;
	vector<CDXBasicPainter::VERTEX> Accum;
	Accum.resize(m_Vertices.size());
	memset(&Accum[0], 0,sizeof(CDXBasicPainter::VERTEX)*Accum.size());
	Histogram.resize(m_Vertices.size());
	memset(&Histogram[0], 0, sizeof(int)*Histogram.size());
	CDXBasicPainter::VERTEX* pVertices = &m_Vertices[0];
	unsigned long* pIndices = &m_Indices[0];
	int* pHistogram = &Histogram[0];
	for (unsigned int i = 0; i<m_Indices.size(); i+=3)
	{
			VECTOR4D V0, V1, V2, T0, T1, T2;
			MATRIX4D InvS, Mq, Mt;
			InvS=Identity();
			Mq=Identity();
			//Tomar un triangulo
			V0 = pVertices[pIndices[i]].Position;
			V1 = pVertices[pIndices[i + 1]].Position;
			V2 = pVertices[pIndices[i + 2]].Position;
			//y sus coordenadas de textura para formar la base ortornormal en espacio de vértice (espacio tangente)
			T0 = pVertices[pIndices[i]].TexCoord;
			T1 = pVertices[pIndices[i+1]].TexCoord;
			T2 = pVertices[pIndices[i+2]].TexCoord;
			Mq.vec[0] = V1 - V0;
			Mq.vec[1] = V2 - V0;
			InvS.vec[0] = T1 - T0;
			InvS.vec[1] = T2 - T0;
			Inverse(InvS,InvS);
			Mt = InvS*Mq;
			VECTOR4D T = Normalize(Mt.vec[0]);
			VECTOR4D B = Normalize(Mt.vec[1]);
			for (int j = 0; j<3; j++)
			{
		
				VECTOR4D N = Normalize(bGenerateNormal ? Cross3(V1 - V0, V2 - V0) : m_Vertices[pIndices[i+j]].Normal);
				//Ortogonalización con respecto a la normal de vértice
				T = Normalize(T - N*Dot(N, T));
				B = Normalize(B - N*Dot(N, B) - T*Dot(T, B));
				Accum[pIndices[i + j]].Normal = Accum[pIndices[i + j]].Normal + N;
				Accum[pIndices[i + j]].Tangent = Accum[pIndices[i + j]].Tangent + T;
				Accum[pIndices[i + j]].Binormal = Accum[pIndices[i + j]].Binormal + B;
				pHistogram[pIndices[i + j]]++;
			}
		}
		for (unsigned int i = 0; i<m_Vertices.size(); i++)
		{
			float invFreq = 1.0f / pHistogram[i];
			pVertices[i].Normal = Normalize(bGenerateNormal?(Accum[i].Normal*invFreq):pVertices[i].Normal);
			pVertices[i].Tangent = Normalize(Accum[i].Tangent*invFreq);
			pVertices[i].Binormal = Normalize(Accum[i].Binormal*invFreq);
		}
}

void CMesh::Optimize()
{
	//Remover vértices duplicados, Complejidad temporal O(N^2)
	vector<int> VertexRemoved;
	vector<CDXBasicPainter::VERTEX> VertexOut;
	vector<int> VertexReplacedBy;
	int nVertexOut = 0;
	VertexOut.resize(m_Vertices.size());
	VertexReplacedBy.resize(m_Vertices.size());
	VertexRemoved.resize(m_Vertices.size());
	for (unsigned int i = 0; i < VertexReplacedBy.size(); i++) VertexReplacedBy[i] = i;
	for (auto &x : VertexRemoved) x = 0;
	//Para cada vértice, eliminar si existe otro igual en el resto del buffer
	for (unsigned int j = 0; j < m_Vertices.size()-1; j++)
	{
		if (!VertexRemoved[j]) //Si no se ha removido, entonces, es único
		{
			VertexOut[nVertexOut]=m_Vertices[j];
			VertexReplacedBy[j] = nVertexOut;
			nVertexOut++;
		}
		else
			continue;
		//Para todos los demás vertices, compararlo con el de referencia [j]
		for (unsigned int i = j + 1; i < m_Vertices.size(); i++)
		{
			if (VertexRemoved[i]) continue;
			VECTOR4D dist = m_Vertices[i].Position - m_Vertices[j].Position;
			if (Dot(dist,dist) < 0.000001f)
			{
				VertexRemoved[i] = 1;
				VertexReplacedBy[i] = nVertexOut - 1;
			}
		}
	}
	for (auto &index : m_Indices) index = VertexReplacedBy[index];
	VertexOut.resize(nVertexOut);
	m_Vertices = VertexOut;
	m_Vertices.shrink_to_fit();
	//Eliminar tríangulos cuya área se aproxime a cero.  Complejidad O(N)
	//(Estos triángulos no aportan información consistente para la generación del
	//espacio tangencial y deben ser removidos)
	vector<unsigned long> Indices;
	Indices.resize(m_Indices.size());
	unsigned long IndicesOut = 0;
	for (unsigned int j = 0; j < m_Indices.size(); j+=3)
	{
		//Tomar un triangulo
		VECTOR4D V0, V1, V2;
		V0 = m_Vertices[m_Indices[j + 0]].Position;
		V1 = m_Vertices[m_Indices[j + 1]].Position;
		V2 = m_Vertices[m_Indices[j + 2]].Position;
		if (Magnity(Cross3(V1 - V0, V2 - V0)) > 0.000001f) //Si el doble del área del triangulo es mayor que 0.00001, considerarlo
		{
			Indices[IndicesOut++] = m_Indices[j + 0];
			Indices[IndicesOut++] = m_Indices[j + 1];
			Indices[IndicesOut++] = m_Indices[j + 2];
		}
	}
	Indices.resize(IndicesOut);
	m_Indices = Indices;
	m_Indices.shrink_to_fit();
}
