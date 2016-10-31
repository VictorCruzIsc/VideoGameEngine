#pragma once

struct SVECTOR4D
{
	union
	{
		struct
		{
			float x,y,z,w;
		};
		struct
		{
			float r,g,b,a;
		};
		struct
		{
			long lx,ly,lz,lh;
		};
		float v[4];
		long  l[4];
	};
};

class VECTOR4D:public SVECTOR4D
{
public:
	VECTOR4D(){};
	VECTOR4D(double X,double Y,double Z,double W=1.0);
	friend float Dot(const VECTOR4D &V1,const VECTOR4D &V2);
	friend VECTOR4D& Cross3(const VECTOR4D &V1,const VECTOR4D& V2,VECTOR4D& Result);
	friend float SquareMag(const VECTOR4D &V); 
	friend VECTOR4D& Normalize(const VECTOR4D& V,VECTOR4D& Result);

	friend VECTOR4D& Mul(const VECTOR4D&,const VECTOR4D&,VECTOR4D& Result);
	friend VECTOR4D& Add(const VECTOR4D&,const VECTOR4D&,VECTOR4D& Result);
	friend VECTOR4D& Sub(const VECTOR4D &V1,const VECTOR4D &V2,VECTOR4D &Result);
	friend VECTOR4D& MulS(const VECTOR4D&,const float s,VECTOR4D& Result);
	
};



class MATRIX4D
{
	friend MATRIX4D operator *(const MATRIX4D& M1,const MATRIX4D& M2);
	friend MATRIX4D operator *(const MATRIX4D& M1,const float s);
	friend VECTOR4D operator *(const VECTOR4D& V,const MATRIX4D& M);
public:
	union
	{
		struct
		{
			float m00,m01,m02,m03;
			float m10,m11,m12,m13;
			float m20,m21,m22,m23;
			float m30,m31,m32,m33;
		};
		struct
		{
			float _11,_12,_13,_14;
			float _21,_22,_23,_24;
			float _31,_32,_33,_34;
			float _41,_42,_43,_44;
		};
		struct
		{
			SVECTOR4D vec0;
			SVECTOR4D vec1;
			SVECTOR4D vec2;
			SVECTOR4D vec3;
		};
		SVECTOR4D vec[4];
		float m[4][4];
		float v[16];
	};
	static void Transpose(MATRIX4D& M,MATRIX4D& T);
	static void Identity(MATRIX4D& I);
	
	static void RotationAxis(const float theta,VECTOR4D& vAxis,MATRIX4D& R);
	static void RotationX(const float theta,MATRIX4D& Rx);
	static void RotationY(const float theta,MATRIX4D& Ry);
	static void	RotationZ(const float theta,MATRIX4D& Rz);
	static float Inverse(MATRIX4D& M,MATRIX4D& R);
	static void ScaleAxis(const float s,VECTOR4D& vAxis,MATRIX4D& S);
	static void Scale(const float sx,const float sy,const float sz,MATRIX4D& S);
	static void Translation(const float dx,const float dy,const float dz,MATRIX4D& T);
	static void PerspectiveProjectionLH(float fFOV,float fAspect,float zNear,float zFar,MATRIX4D&);
	static void OrtographicProjectionLH(float s,float b,float l,float r,float zNear,float zFar,MATRIX4D&);
	static void LookAtLH(VECTOR4D& EyePos,VECTOR4D& LookAt,VECTOR4D& Up,MATRIX4D& Result);
	static void Reflection(VECTOR4D& Plane,MATRIX4D& Reflect);
	static void Orthogonalize(MATRIX4D& M,MATRIX4D& R); //Rotaciones, Traslaciones
	static void MATRIX4D::TransformVertexBuffer(MATRIX4D &M,int nVertexes,
		void *pVertexBufferIn,int nInVertexOffset,int nInVertexStride,
		void *pVertexBufferOut,int nOutVertexOffset, int nOutVertexStride);
	static void TransformVec3Buffer(MATRIX4D &M,int nVertexes,
	void *pVertexBufferIn,int nInVertexOffset,int nInVertexStride,
	void* pVertexBufferOut,int nOutVertexOffset, int nOutVertexStride);
};

bool PointInTriangleBarycentric(VECTOR4D& V0,VECTOR4D& V1, VECTOR4D& V2,VECTOR4D& P);
// t=-pN/pD
void RayCastOnPlane(SVECTOR4D& Plane,SVECTOR4D& RayDir,SVECTOR4D& RayOrigin,float* pN,float* pD);
void ComputeIntersectionPoint(VECTOR4D& O,VECTOR4D& Dir,float s,VECTOR4D& Position);
void RayCastFromPerspectiveProjectionSpaceToWorldSpace(MATRIX4D& mView,MATRIX4D& mProj,
	float x,float y,VECTOR4D& vOutRayOrigin,VECTOR4D& vOutRayDir);
void RayCastFromOrthographicProjectionSpaceToWorldSpace(MATRIX4D& mView,MATRIX4D& mProj,
	float x,float y,VECTOR4D& vOutRayOrigin,VECTOR4D& vOutRayDir);
