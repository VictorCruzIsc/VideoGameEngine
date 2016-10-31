#include "StdAfx.h"
#include "Matrix4D.h"
#include <math.h>


VECTOR4D::VECTOR4D(double X,double Y,double Z,double W)
{
	x=(float)X;
	y=(float)Y;
	z=(float)Z;
	w=(float)W;
}
float Dot(const VECTOR4D &V1,const VECTOR4D &V2)
{
	float result;
	__asm
	{
		mov	   eax,V1
		mov    ebx,V2
		movups xmm1,xmmword ptr[eax]
		movups xmm0,xmmword ptr[ebx]
		mulps  xmm0,xmm1
		haddps xmm0,xmm0
		haddps xmm0,xmm0
		movd   result,xmm0
	}
	return result;
	//return V1.x*V2.x + V1.y*V2.y + V1.z*V2.z + V1.h*V2.h;
}
VECTOR4D& Cross3(const VECTOR4D &V1,const VECTOR4D& V2,VECTOR4D& Result)
{
	Result.z=V1.x*V2.y - V1.y*V2.x;
	Result.x=V1.y*V2.z - V1.z*V2.y;
	Result.y=V1.z*V2.x - V1.x*V2.z;
	Result.w=0.0f;
	return Result;
}
float SquareMag(const VECTOR4D &V)
{
	return Dot(V,V);
}
VECTOR4D& Normalize(const VECTOR4D& V,VECTOR4D& Result)
{
	float Mag;
	Mag=sqrtf(SquareMag(V));
	if(Mag>0.0000001)
	{
		float InvMag=1.0f/Mag;
		return MulS(V,InvMag,Result);
	}
	return Result;
}

MATRIX4D operator *(const MATRIX4D& M1,const MATRIX4D& M2)
{
	MATRIX4D R;
	for(int j=0;j<4;j++)
		for(int i=0;i<4;i++)
		{
			R.m[j][i]=0.0f;
			for(int k=0;k<4;k++)
			{
				R.m[j][i]+=M1.m[j][k]*M2.m[k][i];
			}
		}
	return R;
}
MATRIX4D operator *(const MATRIX4D& M1,const float s)
{
	MATRIX4D R;
	for(int i=0;i<16;i++)
		R.v[i]=M1.v[i]*s;
	return R;
}
VECTOR4D operator *(const VECTOR4D& V,const MATRIX4D& M)
{
	VECTOR4D R;
	for(int i=0;i<4;i++)
	{
		R.v[i]=0.0f;
		for(int j=0;j<4;j++)
			R.v[i]+=V.v[j]*M.m[j][i];
	}
	return R;
}
void MATRIX4D::Identity(MATRIX4D& I)
{
	for(int j=0;j<4;j++)
		for(int i=0;i<4;i++)
			I.m[j][i] = (j==i)?1.0f:0.0f;
}
void MATRIX4D::RotationAxis(const float theta,VECTOR4D& vAxis,MATRIX4D& R)
{
	float _cos=cosf(theta);
	float _1_cos=1.0f-_cos;
	float _sin=sinf(theta);
	
	R.m00=vAxis.x*vAxis.x*_1_cos+_cos;
	R.m01=vAxis.x*vAxis.y*_1_cos+vAxis.z*_sin;
	R.m02=vAxis.x*vAxis.z*_1_cos-vAxis.y*_sin;
	R.m03=0;
	
	R.m10=vAxis.y*vAxis.x*_1_cos-vAxis.z*_sin;
	R.m11=vAxis.y*vAxis.y*_1_cos+_cos;
	R.m12=vAxis.y*vAxis.z*_1_cos+vAxis.x*_sin;
	R.m13=0;
	
	R.m20=vAxis.z*vAxis.x*_1_cos+vAxis.y*_sin;
	R.m21=vAxis.z*vAxis.y*_1_cos-vAxis.x*_sin;
	R.m22=vAxis.z*vAxis.z*_1_cos+_cos;
	R.m23=0;

	R.m30=0;
	R.m31=0;
	R.m32=0;
	R.m33=1;
}
void MATRIX4D::ScaleAxis(const float s,VECTOR4D& vAxis,MATRIX4D &S)
{
	float _s_1=s-1.0f;

	S.m00=vAxis.x*vAxis.x*_s_1+1.0f;
	S.m01=vAxis.x*vAxis.y*_s_1;
	S.m02=vAxis.x*vAxis.z*_s_1;
	S.m03=0;
	
	S.m10=vAxis.y*vAxis.x*_s_1;
	S.m11=vAxis.y*vAxis.y*_s_1+1.0f;
	S.m12=vAxis.y*vAxis.z*_s_1;
	S.m13=0;
	
	S.m20=vAxis.z*vAxis.x*_s_1;
	S.m21=vAxis.z*vAxis.y*_s_1;
	S.m22=vAxis.z*vAxis.z*_s_1+1.0f;
	S.m23=0;

	S.m30=0;
	S.m31=0;
	S.m32=0;
	S.m33=1;
}
void MATRIX4D::LookAtLH(VECTOR4D& EyePos,VECTOR4D& LookAt,VECTOR4D& Up,MATRIX4D& Result)
{
	MATRIX4D::Identity(Result);
	VECTOR4D vRight;
	VECTOR4D vUp;
	VECTOR4D vDir;
	Sub(LookAt,EyePos,vDir);
	Cross3(Up,vDir,vRight);
	Normalize(vDir,vDir);
	Normalize(vRight,vRight);
	Cross3(vDir,vRight,vUp);
	Result.m00=vRight.x;
	Result.m10=vRight.y;
	Result.m20=vRight.z;
	Result.m01=vUp.x;
	Result.m11=vUp.y;
	Result.m21=vUp.z;
	Result.m02=vDir.x;
	Result.m12=vDir.y;
	Result.m22=vDir.z;
	Result.m30=-Dot(vRight,EyePos);
	Result.m31=-Dot(vUp,EyePos);
	Result.m32=-Dot(vDir,EyePos);
}

void MATRIX4D::PerspectiveProjectionLH(float fFOV,float fAspect,float zNear,float zFar,MATRIX4D& Result)
{
	/*
	[ ctg(FOV/2)/fAspect      0                    0                     0 ]
	[     0             ctg(FOV/2)                 0                     0 ]
    [     0                   0          zFar/(zFar-zNear);              1 ]
	[      0                  0        -zFar*zNear /(zFar-zNear);        0 ]
	*/


	Identity(Result);
	Result.m11=1.0f/tan(fFOV/2.0f);
	Result.m00=Result.m11/fAspect;
	// z = Zn+u*(Zf-Zn)
	// u= (z - Zn) / (Zf-Zn) = z/(Zf-Zn) - Zn/(Zf-Zn)
	// u*Zf=zF*(z-Zn)/(Zf-Zn) 
	//cuando z=Zf      u*Zf/Zf=(zF*(z-Zn)/(Zf-Zn))/Zf
	Result.m22= zFar/(zFar-zNear);
	Result.m32= -zFar*zNear /(zFar-zNear);
	Result.m23= 1;
	Result.m33= 0;

	

}
void MATRIX4D::OrtographicProjectionLH(float s,float b,float l,float r,float zNear,float zFar,MATRIX4D& Result)
{
	Identity(Result);
	//y= l + (u/2+0.5)*(r-l)
	//u=-1 -> x = l
    //u= 1 -> x = r
	//u/2+0.5 = (x-l) / (r-l)
	//u=   ((x-l)/(r-l)  - 0.5)*2
	//u=    2*(x-l)/(r-l) - 1 
	//u=    y*2/(r-l) - (2*l)/(r-l) - 1
	//      -2*l/(r-l)-1 = -(2*l/(r-l))-(r-l)/(r-l);
	//                  = -(2*l - r-l)/(r-l) 
	//                    = -(l+r)/(r-l);
	//                      = (l+r)/(l-r)

	/*
	[ 2.0f/(l-r)       0                    0                     0 ]
	[     0         2.0f/(s-b)              0                     0 ]
    [     0            0              1.0f/(zFar-zNear);          0 ]
	[ (l+r)/(l-r)   (s+b)/(b-s)       -zNear/(zFar-zNear);         1 ]
	*/


	Result.m00= 2.0f/(r-l);
	Result.m11= 2.0f/(s-b);
	Result.m22= 1.0f/(zFar-zNear);
	Result.m32=-zNear/(zFar-zNear);
	Result.m30=(l+r)/(l-r);
	Result.m31=(s+b)/(b-s);
}
void MATRIX4D::RotationZ(const float theta,MATRIX4D& Rz)
{
	RotationAxis(theta,VECTOR4D(0,0,1),Rz);
}
void MATRIX4D::RotationX(const float theta,MATRIX4D& Rx)
{
	RotationAxis(theta,VECTOR4D(1,0,0),Rx);
}
void MATRIX4D::RotationY(const float theta,MATRIX4D& Ry)
{
	RotationAxis(theta,VECTOR4D(0,1,0),Ry);
}
float MATRIX4D::Inverse(MATRIX4D& M,MATRIX4D& R)
{
	double inv[16], det;
    int i;

    inv[0] = M.v[5]  * M.v[10] * M.v[15] - 
             M.v[5]  * M.v[11] * M.v[14] - 
             M.v[9]  * M.v[6]  * M.v[15] + 
             M.v[9]  * M.v[7]  * M.v[14] +
             M.v[13] * M.v[6]  * M.v[11] - 
             M.v[13] * M.v[7]  * M.v[10];

    inv[4] = -M.v[4]  * M.v[10] * M.v[15] + 
              M.v[4]  * M.v[11] * M.v[14] + 
              M.v[8]  * M.v[6]  * M.v[15] - 
              M.v[8]  * M.v[7]  * M.v[14] - 
              M.v[12] * M.v[6]  * M.v[11] + 
              M.v[12] * M.v[7]  * M.v[10];

    inv[8] = M.v[4]  * M.v[9] * M.v[15] - 
             M.v[4]  * M.v[11] * M.v[13] - 
             M.v[8]  * M.v[5] * M.v[15] + 
             M.v[8]  * M.v[7] * M.v[13] + 
             M.v[12] * M.v[5] * M.v[11] - 
             M.v[12] * M.v[7] * M.v[9];

    inv[12] = -M.v[4]  * M.v[9] * M.v[14] + 
               M.v[4]  * M.v[10] * M.v[13] +
               M.v[8]  * M.v[5] * M.v[14] - 
               M.v[8]  * M.v[6] * M.v[13] - 
               M.v[12] * M.v[5] * M.v[10] + 
               M.v[12] * M.v[6] * M.v[9];

    inv[1] = -M.v[1]  * M.v[10] * M.v[15] + 
              M.v[1]  * M.v[11] * M.v[14] + 
              M.v[9]  * M.v[2] * M.v[15] - 
              M.v[9]  * M.v[3] * M.v[14] - 
              M.v[13] * M.v[2] * M.v[11] + 
              M.v[13] * M.v[3] * M.v[10];

    inv[5] = M.v[0]  * M.v[10] * M.v[15] - 
             M.v[0]  * M.v[11] * M.v[14] - 
             M.v[8]  * M.v[2] * M.v[15] + 
             M.v[8]  * M.v[3] * M.v[14] + 
             M.v[12] * M.v[2] * M.v[11] - 
             M.v[12] * M.v[3] * M.v[10];

    inv[9] = -M.v[0]  * M.v[9] * M.v[15] + 
              M.v[0]  * M.v[11] * M.v[13] + 
              M.v[8]  * M.v[1] * M.v[15] - 
              M.v[8]  * M.v[3] * M.v[13] - 
              M.v[12] * M.v[1] * M.v[11] + 
              M.v[12] * M.v[3] * M.v[9];

    inv[13] = M.v[0]  * M.v[9] * M.v[14] - 
              M.v[0]  * M.v[10] * M.v[13] - 
              M.v[8]  * M.v[1] * M.v[14] + 
              M.v[8]  * M.v[2] * M.v[13] + 
              M.v[12] * M.v[1] * M.v[10] - 
              M.v[12] * M.v[2] * M.v[9];

    inv[2] = M.v[1]  * M.v[6] * M.v[15] - 
             M.v[1]  * M.v[7] * M.v[14] - 
             M.v[5]  * M.v[2] * M.v[15] + 
             M.v[5]  * M.v[3] * M.v[14] + 
             M.v[13] * M.v[2] * M.v[7] - 
             M.v[13] * M.v[3] * M.v[6];

    inv[6] = -M.v[0]  * M.v[6] * M.v[15] + 
              M.v[0]  * M.v[7] * M.v[14] + 
              M.v[4]  * M.v[2] * M.v[15] - 
              M.v[4]  * M.v[3] * M.v[14] - 
              M.v[12] * M.v[2] * M.v[7] + 
              M.v[12] * M.v[3] * M.v[6];

    inv[10] = M.v[0]  * M.v[5] * M.v[15] - 
              M.v[0]  * M.v[7] * M.v[13] - 
              M.v[4]  * M.v[1] * M.v[15] + 
              M.v[4]  * M.v[3] * M.v[13] + 
              M.v[12] * M.v[1] * M.v[7] - 
              M.v[12] * M.v[3] * M.v[5];

    inv[14] = -M.v[0]  * M.v[5] * M.v[14] + 
               M.v[0]  * M.v[6] * M.v[13] + 
               M.v[4]  * M.v[1] * M.v[14] - 
               M.v[4]  * M.v[2] * M.v[13] - 
               M.v[12] * M.v[1] * M.v[6] + 
               M.v[12] * M.v[2] * M.v[5];

    inv[3] = -M.v[1] * M.v[6] * M.v[11] + 
              M.v[1] * M.v[7] * M.v[10] + 
              M.v[5] * M.v[2] * M.v[11] - 
              M.v[5] * M.v[3] * M.v[10] - 
              M.v[9] * M.v[2] * M.v[7] + 
              M.v[9] * M.v[3] * M.v[6];

    inv[7] = M.v[0] * M.v[6] * M.v[11] - 
             M.v[0] * M.v[7] * M.v[10] - 
             M.v[4] * M.v[2] * M.v[11] + 
             M.v[4] * M.v[3] * M.v[10] + 
             M.v[8] * M.v[2] * M.v[7] - 
             M.v[8] * M.v[3] * M.v[6];

    inv[11] = -M.v[0] * M.v[5] * M.v[11] + 
               M.v[0] * M.v[7] * M.v[9] + 
               M.v[4] * M.v[1] * M.v[11] - 
               M.v[4] * M.v[3] * M.v[9] - 
               M.v[8] * M.v[1] * M.v[7] + 
               M.v[8] * M.v[3] * M.v[5];

    inv[15] = M.v[0] * M.v[5] * M.v[10] - 
              M.v[0] * M.v[6] * M.v[9] - 
              M.v[4] * M.v[1] * M.v[10] + 
              M.v[4] * M.v[2] * M.v[9] + 
              M.v[8] * M.v[1] * M.v[6] - 
              M.v[8] * M.v[2] * M.v[5];

    det = M.v[0] * inv[0] + M.v[1] * inv[4] + M.v[2] * inv[8] + M.v[3] * inv[12];

    if (det == 0)
        return 0.0f;

    double invdet = 1.0 / det;

    for (i = 0; i < 16; i++)
		R.v[i] =(float) (inv[i] * invdet);

    return (float)det;
}
void MATRIX4D::Scale(const float sx,const float sy,const float sz,MATRIX4D& S)
{
	Identity(S);
	S.m00=sx;
	S.m11=sy;
	S.m22=sz;
}
void MATRIX4D::Translation(const float dx,const float dy,const float dz,MATRIX4D& T)
{
	Identity(T);
	T.m30=dx;
	T.m31=dy;
	T.m32=dz;
}
void MATRIX4D::Reflection(VECTOR4D& Plane,MATRIX4D& Reflect)
{
	Identity(Reflect);
	Reflect.m00=1-2*Plane.x*Plane.x;
	Reflect.m01=-2*Plane.x*Plane.y;
	Reflect.m02=-2*Plane.x*Plane.z;
	
	Reflect.m10= -2*Plane.y*Plane.x;
	Reflect.m11=1-2*Plane.y*Plane.y;
	Reflect.m12=-2*Plane.y*Plane.z;
	
	Reflect.m20=-2*Plane.z*Plane.x;
	Reflect.m21=-2*Plane.z*Plane.y;
	Reflect.m22=1-2*Plane.z*Plane.z;

	Reflect.m30=-2*Plane.w*Plane.x;
	Reflect.m31=-2*Plane.w*Plane.y;
	Reflect.m32=-2*Plane.w*Plane.z;
}

void MATRIX4D::Transpose(MATRIX4D& M,MATRIX4D& T)
{
	for(int i=0;i<4;i++)
		for(int j=0;j<4;j++) T.m[i][j]=M.m[j][i];
}
VECTOR4D& Mul(const VECTOR4D& A,const VECTOR4D& B,VECTOR4D& Result)
{
	__asm
	{
		mov	   eax,A
		mov    ebx,B
		movups xmm1,xmmword ptr[eax]
		movups xmm0,xmmword ptr[ebx]
		mov    eax,Result
		mulps  xmm1,xmm0
		movups xmmword ptr[eax],xmm1
	}
	return Result;
}
VECTOR4D& Add(const VECTOR4D& A,const VECTOR4D& B,VECTOR4D& Result)
{
	__asm
	{
		mov	   eax,A
		mov    ebx,B
		movups xmm1,xmmword ptr[eax]
		movups xmm0,xmmword ptr[ebx]
		mov    eax,Result
		addps  xmm1,xmm0
		movups xmmword ptr[eax],xmm1
	
	}
}
VECTOR4D& Sub(const VECTOR4D& A,const VECTOR4D& B,VECTOR4D& Result)
{
	__asm
	{
		mov	   eax,A
		mov    ebx,B
		movups xmm1,xmmword ptr[eax]
		movups xmm0,xmmword ptr[ebx]
		mov    eax,Result
		subps  xmm1,xmm0
		movups xmmword ptr[eax],xmm1
	}
	return Result;
}

VECTOR4D& MulS(const VECTOR4D& A,const float s,VECTOR4D& Result)
{
	__asm
	{
		mov	   eax,A
		movups xmm1,[eax]
		movss  xmm0,s
		pshufd xmm0,xmm0,0x00   
		mov    eax,Result
		mulps  xmm1,xmm0
		movups [eax],xmm1
	}
	return Result;
}

// t=-pN/pD
void RayCastOnPlane(SVECTOR4D& Plane,
					SVECTOR4D& RayDir,
					SVECTOR4D& RayOrigin,
					float* pN,
					float* pD)
{
	// t=-(D+Dot3(Np,R0))/(Dot3(Np,Rd))
	// Plane := (N,D)= [A,B,C,D]
	// t=-(Dot4(Plane,R0)/(Dot4(Plane,Rd))
	// R0:= [x0,y0,z0,1] Punto de referencia
	// Rd:= [dx,dy,dz,0] Vector de dirección del rayo
	__asm
	{
		mov esi,Plane
		mov edi,RayDir
		mov edx,RayOrigin
		movups xmm0,xmmword ptr[esi] // Cargar el plano
		movups xmm1,xmm0             // Copiar el plano
		movups xmm2,xmmword ptr[edx] //Cargar el origen
		movups xmm3,xmmword ptr[edi] //Cargar la dirección
		mulps  xmm0,xmm2             // Dot4(Plane,R0)
		haddps xmm0,xmm0
		haddps xmm0,xmm0
		mulps  xmm1,xmm3             // Dot4(Plane,Rd) 
		haddps xmm1,xmm1
		haddps xmm1,xmm1
		mov esi,pN
		movss dword ptr[esi],xmm0
		mov esi,pD
		movss dword ptr[esi],xmm1
	}
}


void ComputeIntersectionPoint(VECTOR4D& O,VECTOR4D& Dir,float u,VECTOR4D& Position)
{
	//Position=O+u*Dir
	__asm
	{
		mov edi,O
		mov esi,Dir
		mov edx,Position
		movss xmm0,u           //[0,0,0,u]
		pshufd xmm0,xmm0, 0x00 //[u,u,u,u]
		movups xmm1,xmmword ptr[esi]
		movups xmm2,xmmword ptr[edi]
		mulps  xmm0,xmm1       //u*Dir
		addps  xmm0,xmm2       //O+u*Dir
		movups xmmword ptr[edx],xmm0
	}
}


void MATRIX4D::TransformVertexBuffer(MATRIX4D &M,int nVertexes,
		void *pVertexBufferIn,int nInVertexOffset,int nInVertexStride,
		void *pVertexBufferOut,int nOutVertexOffset, int nOutVertexStride)
{
	float *pInput=(float*)(((char*)pVertexBufferIn)+nInVertexOffset);
	float *pOutput=(float*)(((char*)pVertexBufferOut)+nOutVertexOffset);
	for(int i=0;i<nVertexes;i++)
	{

		*(VECTOR4D*)pOutput=*(VECTOR4D*)pInput*M;
		pInput=(float*)(((char*)pInput)+nInVertexStride);
		pOutput=(float*)(((char*)pOutput)+nOutVertexStride);
	}
}





bool PointInTriangle(VECTOR4D& V0,VECTOR4D& V1, VECTOR4D& V2,VECTOR4D& P)
{
	VECTOR4D D0,D1,D2;
	float    m0,m1,m2;
	float    p01,p12,p20;
	float    theta01,theta12,theta20;
	Sub(V0,P,D0);
	Sub(V1,P,D1);
	Sub(V2,P,D2);
	m0=sqrt(SquareMag(D0));
	m1=sqrt(SquareMag(D1));
	m2=sqrt(SquareMag(D2));
	p01=Dot(D0,D1);
	p12=Dot(D1,D2);
	p20=Dot(D2,D0);
	theta01=acos(p01/(m0*m1));
	theta12=acos(p12/(m1*m2));
	theta20=acos(p20/(m2*m0));
	return fabs(2*3.141592-(theta01+theta12+theta20))<0.001;
}


bool PointInTriangleBarycentric(VECTOR4D& V0,VECTOR4D& V1, VECTOR4D& V2,VECTOR4D& P)
{
	float A,B,C,D,/*E,*/F;
	_asm
	{
	mov    eax,V0
	movups xmm0,[eax]
	mov    eax,V1
	movups xmm1,[eax]
	subps  xmm1,xmm0    //V1-V0
    mov    eax,V2
	movups xmm2,[eax]
	subps  xmm2,xmm0    //V2-V0
	mov    eax,P
	movups xmm7,[eax]
	subps  xmm7,xmm0    //P-V0
	/*
	xmm0 = P
	xmm1 = V1-V0
	xmm2 = V2-V0
	xmm7 = P-V0
	*/
	//Pipeline
	//A=Dot((P-V0),(V1-V0)) 
	movaps xmm3,xmm7 
	mulps  xmm3,xmm1
	haddps xmm3,xmm3
	haddps xmm3,xmm3
	movss  A,xmm3
	//B=Dot((V1-V0),(V1-V0))
	movaps xmm4,xmm1
	mulps  xmm4,xmm4
	haddps xmm4,xmm4
	haddps xmm4,xmm4
	movss  B,xmm4
	//C=Dot((V2-V0),(V1-V0)) = E
	movaps xmm5,xmm2
	mulps xmm5,xmm1
	haddps xmm5,xmm5
	haddps xmm5,xmm5
	movss C,xmm5
	//D=Dot((P-V0),(V2-V0))
	movaps xmm6,xmm2
	mulps  xmm6,xmm7
	haddps xmm6,xmm6
	haddps xmm6,xmm6
	movss D,xmm6
/*	//E=Dot((V1-V0),(V2-V0))
	movaps xmm3,xmm1
	mulps  xmm3,xmm2
	haddps xmm3,xmm3
	haddps xmm3,xmm3
	movss  E,xmm3*/
	//F = Dot((V2-V0),(V2-V0))
	movaps xmm6,xmm2
	mulps  xmm6,xmm6
	haddps xmm6,xmm6
	haddps xmm6,xmm6
	movss F,xmm6
	}
	float det=1/(B*F-/*E*/C*C);
	float w1=(F*A-C*D)*det;
	float w2=(B*D-C/*E*/*A)*det;
	return (w1>=0) && (w2>=0) && ((w1+w2) <= 1);
}


void RayCastFromPerspectiveProjectionSpaceToWorldSpace(MATRIX4D& mView,MATRIX4D& mProj,
	float x,float y,VECTOR4D& vOutRayOrigin,VECTOR4D& vOutRayDir)
{
	//Los valores de entrada x,y ya están normalizados. 
	
	//1.- Para convertir del espacio de pantalla normalizada a espacio mundial.
	MATRIX4D mInvVP=mView*mProj;
	MATRIX4D::Inverse(mInvVP,mInvVP);
	//2.- Extraer Posición de la cámara, donde será lanzado el rayo
		//Normalized Screen Space -> Homogeneus World Space
	vOutRayOrigin=VECTOR4D(0,0,1,0)*mInvVP;
	//   Convertir de espacio homogeneo -> espacio euclídeo , que ya están en el espacio mundial
	MulS(vOutRayOrigin,1.0f/vOutRayOrigin.w,vOutRayOrigin);
	//3.-  Extraer la Posición del punto de proyección correspondiente al punto (x,y) pero en espacio mundial
	       //Normalized Screen Space -> Homogeneus World Space
	vOutRayDir=VECTOR4D(x,y,0,1)*mInvVP;
	//   Convertir de espacio homogeneo -> espacio euclídeo , que ya están en el espacio mundial
	MulS(vOutRayDir,1.0f/vOutRayDir.w,vOutRayDir);
	//4.- Computar el vector distancia entre ambos puntos (punto proyectado y el punto fuga) en espacio euclideo
	Sub(vOutRayDir,vOutRayOrigin,vOutRayDir);
	// Normalizar para calcular la pura dirección del rayo
	Normalize(vOutRayDir,vOutRayDir);
}


void MATRIX4D::Orthogonalize(MATRIX4D& M,MATRIX4D& R)
{
	VECTOR4D XAxis,YAxis,ZAxis;
	ZAxis=VECTOR4D(M.m20,M.m21,M.m22,0);
	YAxis=VECTOR4D(M.m10,M.m11,M.m12,0);
	Normalize(ZAxis,ZAxis);
	Cross3(YAxis,ZAxis,XAxis);
	Normalize(XAxis,XAxis);
	Cross3(ZAxis,XAxis,YAxis);
	M.m00=XAxis.x;
	M.m01=XAxis.y;
	M.m02=XAxis.z;
	M.m10=YAxis.x;
	M.m11=YAxis.y;
	M.m12=YAxis.z;
	M.m20=ZAxis.x;
	M.m21=ZAxis.y;
	M.m22=ZAxis.z;
}