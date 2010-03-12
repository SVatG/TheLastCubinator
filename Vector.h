#ifndef _VECTOR_H_
#define _VECTOR_H_

#include "tools.h"

#ifndef Scalar
#define Scalar float
#endif

// Vectors

#define VectorEpsilon 0.0001
#define VectorEpsilon2 (VectorEpsilon*VectorEpsilon)

typedef struct Vector { Scalar x,y,z; } Vector;

static inline Vector MakeVector(Scalar x,Scalar y,Scalar z) { Vector v={x,y,z}; return v; }

#define ZeroVector MakeVector(0,0,0)
#define XVector MakeVector(1,0,0)
#define YVector MakeVector(0,1,0)
#define ZVector MakeVector(0,0,1)

static inline Scalar VectorAbs2(Vector v) { return v.x*v.x+v.y*v.y+v.z*v.z; }
static inline Scalar VectorAbs(Vector v) { return sqrtf(VectorAbs2(v)); }

static inline Vector VectorAdd(Vector a,Vector b) { return MakeVector(a.x+b.x,a.y+b.y,a.z+b.z); }
static inline Vector VectorAdd3(Vector a,Vector b,Vector c) { return MakeVector(a.x+b.x+c.x,a.y+b.y+c.y,a.z+b.z+c.z); }
static inline Vector VectorSub(Vector a,Vector b) { return MakeVector(a.x-b.x,a.y-b.y,a.z-b.z); }
static inline Vector VectorMul(Vector a,Scalar b) { return MakeVector(a.x*b,a.y*b,a.z*b); }
static inline Vector VectorDiv(Vector a,Scalar b) { return MakeVector(a.x/b,a.y/b,a.z/b); }
static inline Vector VectorCross(Vector a,Vector b) { return MakeVector(a.y*b.z-a.z*b.y, a.z*b.x-a.x*b.z, a.x*b.y-a.y*b.x); }

static inline Scalar VectorDot(Vector a,Vector b) { return a.x*b.x+a.y*b.y+a.z*b.z; }
static inline Scalar VectorDistance(Vector a,Vector b) { return VectorAbs(VectorSub(a,b)); }

static inline Vector VectorNeg(Vector v) { return MakeVector(-v.x,-v.y,-v.z); }
static inline Vector VectorNorm(Vector v) { return VectorDiv(v,VectorAbs(v)); }

static inline int VectorIsZero(Vector v) { return v.x==0&&v.y==0&&v.z==0; }
static inline int VectorIsNearlyZero(Vector v) { return VectorAbs2(v)<VectorEpsilon2; }
static inline int VectorsAreEqual(Vector a,Vector b) { return a.x==b.x&&a.y==b.y&&a.z==b.z; }
static inline int VectorsAreNearlyEqual(Vector a,Vector b) { return VectorIsNearlyZero(VectorSub(a,b)); }

static inline Vector ProjectVectorToPlane(Vector v,Vector n)
{ return VectorSub(v,VectorMul(n,VectorDot(v,n))); }
static inline Vector PerpendicularVectorForTriangle(Vector v1,Vector v2,Vector v3)
{ return VectorCross(VectorSub(v2,v1),VectorSub(v3,v1)); }
static inline Vector NormalVectorForTriangle(Vector v1,Vector v2,Vector v3)
{ return VectorNorm(PerpendicularVectorForTriangle(v1,v2,v3)); }


#ifndef NO_OPENGL
static inline void VectorGLVertex(Vector v) { glVertex3f(v.x,v.y,v.z); }
static inline void VectorGLNormal(Vector v) { glNormal3f(v.x,v.y,v.z); }
#endif



// Padded vector

typedef struct PaddedVector { Vector v; Scalar pad; } PaddedVector;

static inline PaddedVector PadVector(Vector v) { PaddedVector res={v,0}; return res; }
static inline Vector UnpadVector(PaddedVector v) { return v.v; }



// Quaternions

typedef struct Quaternion { Scalar s; Vector v; } Quaternion;

static inline Quaternion MakeQuaternion(Scalar s,Vector v) { Quaternion q={s,v}; return q; }

static inline Scalar QuaternionAbs2(Quaternion q) { return q.s*q.s+VectorAbs2(q.v); }
static inline Scalar QuaternionAbs(Quaternion q) { return sqrtf(QuaternionAbs2(q)); }

static inline Quaternion QuaternionInverse(Quaternion q);

static inline Quaternion QuaternionAdd(Quaternion a,Quaternion b) { return MakeQuaternion(a.s+b.s,VectorAdd(a.v,b.v)); }
static inline Quaternion QuaternionSub(Quaternion a,Quaternion b) { return MakeQuaternion(a.s-b.s,VectorSub(a.v,b.v)); }
static inline Quaternion QuaternionMul(Quaternion a,Quaternion b)
{ return MakeQuaternion(a.s*b.s-VectorDot(a.v,b.v),VectorAdd3(VectorMul(b.v,a.s),VectorMul(a.v,b.s),VectorCross(a.v,b.v))); }
static inline Quaternion QuaternionDiv(Quaternion a,Quaternion b) { return QuaternionMul(a,QuaternionInverse(b)); }
static inline Quaternion QuaternionScalarMul(Quaternion a,Scalar b) { return MakeQuaternion(a.s*b,VectorMul(a.v,b)); }
static inline Quaternion QuaternionScalarDiv(Quaternion a,Scalar b) { return MakeQuaternion(a.s/b,VectorDiv(a.v,b)); }

static inline Quaternion QuaternionNeg(Quaternion q) { return MakeQuaternion(-q.s,VectorNeg(q.v)); }
static inline Quaternion QuaternionConjugate(Quaternion q) { return MakeQuaternion(q.s,VectorNeg(q.v)); }
static inline Quaternion QuaternionInverse(Quaternion q) { return QuaternionScalarDiv(QuaternionConjugate(q),QuaternionAbs2(q)); }
static inline Quaternion QuaternionNorm(Quaternion q) { return QuaternionScalarDiv(q,QuaternionAbs(q)); }

static inline Quaternion RotationQuaternion(Scalar angle,Vector axis) { return MakeQuaternion(cos(angle/2),VectorMul(axis,sin(angle/2))); }



// Matrices

typedef struct Matrix { Scalar a[16]; } Matrix;

static inline Matrix MakeMatrix(Scalar a00,Scalar a01,Scalar a02,Scalar a03,
                         Scalar a10,Scalar a11,Scalar a12,Scalar a13,
                         Scalar a20,Scalar a21,Scalar a22,Scalar a23,
                         Scalar a30,Scalar a31,Scalar a32,Scalar a33)
{ Matrix m={{a00,a10,a20,a30,a01,a11,a21,a31,a02,a12,a22,a32,a03,a13,a23,a33}}; return m; }

#define ZeroMatrix MakeMatrix(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
#define IdentityMatrix MakeMatrix(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1)

static inline Matrix MakeMatrixFromVectors(Vector x,Vector y,Vector z,Vector r)
{
        return MakeMatrix(x.x,y.x,z.x,r.z,
                          x.y,y.y,z.y,r.y,
                          x.z,y.z,z.z,r.z,
                                            0,  0,  0,  1);
}

static inline Vector MatrixXAxis(Matrix a) { return MakeVector(a.a[0],a.a[1],a.a[2]); }
static inline Vector MatrixYAxis(Matrix a) { return MakeVector(a.a[4],a.a[5],a.a[6]); }
static inline Vector MatrixZAxis(Matrix a) { return MakeVector(a.a[8],a.a[9],a.a[10]); }
static inline Vector MatrixOrigin(Matrix a) { return MakeVector(a.a[12],a.a[13],a.a[14]); }

Matrix MatrixMul(Matrix a,Matrix b);
Matrix FastMatrixMul(Matrix a,Matrix b);
//Matrix DirectionalMatrixMul(Matrix a,Matrix b);

static inline Vector TransformVector(Matrix m,Vector v)
{
        return MakeVector(v.x*m.a[0]+v.y*m.a[4]+v.z*m.a[8]+m.a[12],
                          v.x*m.a[1]+v.y*m.a[5]+v.z*m.a[9]+m.a[13],
                          v.x*m.a[2]+v.y*m.a[6]+v.z*m.a[10]+m.a[14]);
}

static inline Vector TransformVectorDirection(Matrix m,Vector v)
{
        return MakeVector(v.x*m.a[0]+v.y*m.a[4]+v.z*m.a[8],
                          v.x*m.a[1]+v.y*m.a[5]+v.z*m.a[9],
                          v.x*m.a[2]+v.y*m.a[6]+v.z*m.a[10]);
}

static inline Matrix MatrixTranspose(Matrix m)
{
        return MakeMatrix(m.a[0],m.a[1],m.a[2],m.a[3],
                          m.a[4],m.a[5],m.a[6],m.a[7],
                          m.a[8],m.a[9],m.a[10],m.a[11],
                          m.a[12],m.a[13],m.a[14],m.a[15]);
}

//Matrix MatrixInverse(Matrix m);
Matrix FastMatrixInverse(Matrix m);

static inline Matrix TranslationMatrix(Scalar x,Scalar y,Scalar z) { return MakeMatrix(1,0,0,x,0,1,0,y,0,0,1,z,0,0,0,1); }
static inline Matrix TranslationMatrixFromVector(Vector v) { return TranslationMatrix(v.x,v.y,v.z); }
static inline Matrix ScalingMatrix(Scalar x,Scalar y,Scalar z) { return MakeMatrix(x,0,0,0,0,y,0,0,0,0,z,0,0,0,0,1); }

static inline Matrix XAxisRotationMatrix(Scalar angle)
{
        return MakeMatrix(1,         0,          0,0,
                          0,cos(angle),-sin(angle),0,
                          0,sin(angle), cos(angle),0,
                          0,         0,          0,1);
}

static inline Matrix YAxisRotationMatrix(Scalar angle)
{
        return MakeMatrix(cos(angle),0,sin(angle),0,
                                   0,1,         0,0,
                         -sin(angle),0,cos(angle),0,
                                   0,0,         0,1);
}

static inline Matrix ZAxisRotationMatrix(Scalar angle)
{
        return MakeMatrix(cos(angle),-sin(angle),0,0,
                          sin(angle), cos(angle),0,0,
                                  0,           0,1,0,
                                  0,           0,0,1);
}

static inline Matrix RotationMatrixFromQuaternion(Quaternion q)
{
        Scalar s2=q.s*q.s,x2=q.v.x*q.v.x,y2=q.v.y*q.v.y,z2=q.v.z*q.v.z;
        return MakeMatrix(              s2+x2-y2-z2,2*(q.v.x*q.v.y-q.s*q.v.z),2*(q.v.x*q.v.z+q.s*q.v.y),0,
                          2*(q.v.x*q.v.y+q.s*q.v.z),              s2-x2+y2-z2,2*(q.v.y*q.v.z-q.s*q.v.x),0,
                          2*(q.v.x*q.v.z-q.s*q.v.y),2*(q.v.y*q.v.z+q.s*q.v.x),              s2-x2-y2+z2,0,
                                                  0,                        0,                        0,1);
}

static inline Matrix RotationMatrix(Scalar angle,Vector axis)
{ return RotationMatrixFromQuaternion(RotationQuaternion(angle,axis)); }


static inline Matrix PerspectiveMatrix(Scalar degrees,Scalar aspect,Scalar near,Scalar far)
{
        Scalar f=1.0/tan(degrees*PI/180.0/2.0);

        return MakeMatrix(f/aspect,   0,                    0,                      0,
                                 0,   f,                    0,                      0,
                                 0,   0,(far+near)/(near-far),2.0*far*near/(near-far),
                                 0,   0,                   -1,                      0);
}

static inline Matrix SphericalTransformMatrix(Vector r)
{
        Vector out=VectorNorm(r);
        Vector up=VectorNorm(ProjectVectorToPlane(MakeVector(0,1,0),out));
        Vector side=VectorNorm(VectorCross(out,up));
        return MakeMatrixFromVectors(side,out,up,r);
}

#ifndef NO_OPENGL
static inline void MatrixGLMult(Matrix m) { glMultMatrixf(m.a); }
static inline void MatrixGLLoad(Matrix m) { glLoadMatrixf(m.a); }

static inline Matrix GLModelviewMatrix() { Matrix m; glGetFloatv(GL_MODELVIEW_MATRIX,m.a); return m; }
static inline Matrix GLProjectionwMatrix() { Matrix m; glGetFloatv(GL_PROJECTION_MATRIX,m.a); return m; }
static inline Matrix GLTextureMatrix() { Matrix m; glGetFloatv(GL_TEXTURE_MATRIX,m.a); return m; }
#endif

#endif
