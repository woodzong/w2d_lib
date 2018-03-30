#ifndef CW_MATH_H_12314asdfas123123
#define CW_MATH_H_12314asdfas123123

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "CSingletonT.h"

#define WMATH_PI         ((float)3.141592654f)
#define WMATH_SIGN(x)      (((x) < 0) ? -1 : 1)

// defines for small numbers
#define EPSILON_E3 (float)(1E-3) 
#define EPSILON_E4 (float)(1E-4) 
#define EPSILON_E5 (float)(1E-5)
#define EPSILON_E6 (float)(1E-6)


#define CW_PLX_2SIDED_FLAG                     0x1000   // this poly is double sided
#define CW_PLX_1SIDED_FLAG                     0x0000   // this poly is single sided



#define CW_POLY4DV1_ATTR_SHADE_MODE_PURE       0x0020
#define CW_POLY4DV1_ATTR_SHADE_MODE_CONSTANT   0x0020 // (alias)
#define CW_POLY4DV1_ATTR_SHADE_MODE_FLAT       0x0040
#define CW_POLY4DV1_ATTR_SHADE_MODE_GOURAUD    0x0080
#define CW_POLY4DV1_ATTR_SHADE_MODE_PHONG      0x0100
#define CW_POLY4DV1_ATTR_SHADE_MODE_FASTPHONG  0x0100 // (alias)
#define CW_POLY4DV1_ATTR_SHADE_MODE_TEXTURE    0x0200 



// states of polygons and faces
#define CW_POLY4DV1_STATE_ACTIVE             0x0001
#define CW_POLY4DV1_STATE_CLIPPED            0x0002
#define CW_POLY4DV1_STATE_BACKFACE           0x0004

// 4D homogenous vector, point with w ////////////////////
typedef struct VECTOR4D_TYP
{
	union
	{
		float M[4]; // array indexed storage

		// explicit names
		struct
		{
			float x,y,z,w;
		}; // end struct
	}; // end union

} VECTOR4D, POINT4D, *VECTOR4D_PTR, *POINT4D_PTR;



typedef struct POLYF4DV1_TYP
{
	int state;    // state information
	int attr;     // physical attributes of polygon
	int color;    // color of polygon

	POINT4D vlist[3];  // the vertices of this triangle
	POINT4D tvlist[3]; // the vertices after transformation if needed

	POLYF4DV1_TYP *next; // pointer to next polygon in list??
	POLYF4DV1_TYP *prev; // pointer to previous polygon in list??

} POLYF4DV1, *POLYF4DV1_PTR;


// 4x4 matrix /////////////////////////////////////////////
typedef struct MATRIX4X4_TYP
{
	union
	{
		float M[4][4]; // array indexed data storage

		// storage in row major form with explicit names
		struct
		{
			float M00, M01, M02, M03;
			float M10, M11, M12, M13;
			float M20, M21, M22, M23;
			float M30, M31, M32, M33;
		}; // end explicit names

	}; // end union

} MATRIX4X4, *MATRIX4X4_PTR;


// 4x3 matrix /////////////////////////////////////////////
typedef struct MATRIX4X3_TYP
{
union
    {
    float M[4][3]; // array indexed data storage

    // storage in row major form with explicit names
    struct
         {
         float M00, M01, M02;
         float M10, M11, M12;
         float M20, M21, M22;
         float M30, M31, M32;
         }; // end explicit names

    }; // end union

} MATRIX4X3, *MATRIX4X3_PTR;


// 4x4 identity matrix
const MATRIX4X4 IMAT_4X4 = {1,0,0,0, 
                            0,1,0,0, 
                            0,0,1,0, 
                            0,0,0,1};




// 3D vector, point without the w ////////////////////////
typedef struct VECTOR3D_TYP
{
	union
	{
		float M[3]; // array indexed storage

		// explicit names
		struct
		{
			float x,y,z;
		}; // end struct

	}; // end union

} VECTOR3D, POINT3D, *VECTOR3D_PTR, *POINT3D_PTR;

// 3D plane ///////////////////////////////////////////////////
typedef struct PLANE3D_TYP
{
	POINT3D p0; // point on the plane
	VECTOR3D n; // normal to the plane (not necessarily a unit vector)
} PLANE3D, *PLANE3D_PTR;


inline void VECTOR3D_ZERO(VECTOR3D_PTR v) 
{(v)->x = (v)->y = (v)->z = 0.0;}

inline void VECTOR3D_COPY(VECTOR3D_PTR vdst, VECTOR3D_PTR vsrc) 
{(vdst)->x = (vsrc)->x; (vdst)->y = (vsrc)->y;  (vdst)->z = (vsrc)->z; }

inline void VECTOR4D_COPY(VECTOR4D_PTR vdst, VECTOR4D_PTR vsrc) 
{(vdst)->x = (vsrc)->x; (vdst)->y = (vsrc)->y;  
(vdst)->z = (vsrc)->z; (vdst)->w = (vsrc)->w;  }

inline void VECTOR4D_INITXYZ(VECTOR4D_PTR v, float x,float y,float z) 
{(v)->x = (x); (v)->y = (y); (v)->z = (z); (v)->w = 1.0;}

inline void VECTOR3D_INITXYZ(VECTOR3D_PTR v, float x, float y, float z) 
{(v)->x = (x); (v)->y = (y); (v)->z = (z);}

inline void VECTOR4D_ZERO(VECTOR4D_PTR v) 
{(v)->x = (v)->y = (v)->z = 0.0; (v)->w = 1.0;}

inline void POINT3D_COPY(POINT3D_PTR vdst, POINT3D_PTR vsrc) 
{(vdst)->x = (vsrc)->x; (vdst)->y = (vsrc)->y;  (vdst)->z = (vsrc)->z; }


#define MAT_IDENTITY_4X4(m) {memcpy((void *)(m), (void *)&IMAT_4X4, sizeof(MATRIX4X4));}

// some math macros
#define DEG_TO_RAD(ang) ((ang)*WMATH_PI/180.0)
#define RAD_TO_DEG(rads) ((rads)*180.0/WMATH_PI)


// matrix copying macros
#define MAT_COPY_2X2(src_mat, dest_mat) {memcpy((void *)(dest_mat), (void *)(src_mat), sizeof(MATRIX2X2) ); }
#define MAT_COPY_3X3(src_mat, dest_mat) {memcpy((void *)(dest_mat), (void *)(src_mat), sizeof(MATRIX3X3) ); }
#define MAT_COPY_4X4(src_mat, dest_mat) {memcpy((void *)(dest_mat), (void *)(src_mat), sizeof(MATRIX4X4) ); }
#define MAT_COPY_4X3(src_mat, dest_mat) {memcpy((void *)(dest_mat), (void *)(src_mat), sizeof(MATRIX4X3) ); }


class CWMathSim
{
public:
	static double Sigmoid( double netinput, double response )
	{
		return ( 1 / ( 1 + exp( -netinput / response ) ) );
	}

	static float Rand()
	{
		return ( float )( rand() / ( RAND_MAX * 1.0 ) );
	}

	static float RandFloatIn01()
	{
		return ( float )( rand( ) / ( float )RAND_MAX );
	}

	static float RandFloatInMinus1To1()
	{
		return RandFloatIn01( ) - RandFloatIn01( );
	}

	static double RandFloat( )
	{
		return ( rand() ) / ( RAND_MAX + 1.0 );
	}

	static void ClampDouble( double & rValue, const double & rMinValue, const double & rMaxValue )
	{
		if( rValue <= rMinValue )
		{
			rValue = rMinValue;
			return;
		}

		if( rValue >= rMaxValue )
		{
			rValue = rMaxValue;
			return;
		}

		return;
	}

	static int RangeRandInt( int lowerbound, int upperbound )
	{
		//assert( lowerbound <= upperbound );
		return  lowerbound + ( int )( ( upperbound - lowerbound + 1 ) * ( rand() / ( RAND_MAX + 1.0 ) ) );
	}

	static double fast_sqrt_ieee( double number )
	{
		long long i;
		double x, y;
		const double threehalfs = 1.5f;
		y = number;
		x = number * 0.5f;

		//i    = * ( long long * ) &y;
		memcpy( &i, &y, sizeof( i ) );

		i = 0x5fe6ec85e7de30daLL - ( i >> 1 );

		//y    = *(double*)&i;
		memcpy( &y, &i, sizeof( y ) );

		y *= threehalfs - ( x * y * y );
		y *= threehalfs - ( x * y * y );

		return number*y;
	}	

	static double WSqrt( double f )
	{ 
#ifdef _MSC_VER
		return fast_sqrt_ieee( f );
#else
		return sqrt(f); 
#endif
	}

	template<typename T>
	static const T GetDistance( const T & x0, const T & y0, const T & x1, const T & y1 )
	{
		T dx = ( x0 - x1 );
		T dy = ( y0 - y1 );
		return ( T )WSqrt( dx*dx + dy*dy );
	}

	template<typename T>
	static const T GetDistanceSqr( const T & x0, const T & y0, const T & x1, const T & y1 )
	{
		T dx = ( x0 - x1 );
		T dy = ( y0 - y1 );
		return ( T )( dx*dx + dy*dy );
	}

	//沿线移动一段距离,找出最后的点和剩余距离
	template<typename T>
	static const T CaclPointByMoveDisInLine( const T & startX, const T & startY, const T & endX, const T & endY, const T & moveDis, T & outX, T & outY )
	{
		T disAll = GetDistance( startX, startY, endX, endY );

		if( disAll < moveDis || disAll == 0 )
		{
			outX = endX;
			outY = endY;
			return moveDis - disAll;
		}

		outX = ( endX - startX ) * moveDis / disAll + startX;
		outY = ( endY - startY ) * moveDis / disAll + startY;

		return ( T )0;
	}
};





class CWMath{

public:
	CWMath();

	static void InitRandSeed( int iRandSeed );

	float FastCos(float theta);
	float FastSin(float theta);

	static void Matrix_X_Vector4D_4X4( const VECTOR4D * pStVector4D,const MATRIX4X4 * pStMatrix4X4,VECTOR4D * pStVetorOut );

	static void Vector4D_ADD(VECTOR4D * pStVector4Da,VECTOR4D * pStVector4Db,VECTOR4D * pStVector4DSum);

	static float Vector3D_Length(VECTOR3D * pVecA);

	static void Vector3D_Normalize(VECTOR3D * pVecA,VECTOR3D * pVecNormal);

	static void Plane3D_Init(PLANE3D * pPlane, POINT3D * p0, VECTOR3D * pVecNormal, int iNormalize);

	static void Matrix4X4Init(MATRIX4X4 * ma, 
						   float m00, float m01, float m02, float m03,
						   float m10, float m11, float m12, float m13,
						   float m20, float m21, float m22, float m23,
						   float m30, float m31, float m32, float m33);

	static void Matrix4X4Mul( MATRIX4X4 * ma,  MATRIX4X4 * mb, MATRIX4X4 * mprod );

	static void Build_XYZ_Rotation_MATRIX4X4(float theta_x, // euler angles
								  float theta_y, 
								  float theta_z,
								  MATRIX4X4* mrot);

	static void Build_XYZ_Move_MATRIX4X4(float x,
								  float y, 
								  float z,
								  MATRIX4X4* mmove);	

private:
	void InitCosSinTable();


private:
	// storage for our lookup tables
	float m_fCosTable[361]; // 1 extra element so we can store 0-360 inclusive
	float m_fSinTable[361];

};


typedef CSingletonT<CWMath> CWMathSingleton;

#define CW_MATH CWMathSingleton::Instance()




















//wood new define here...
// a polygon based on an external vertex list
typedef struct CW_POLY4DATA_TYP
{
	int iState;    // state information
	int iAttr;     // physical attributes of polygon
	int iColor;    // color of polygon

	POINT4D * vlist; // the vertex list itself
	int iVertexArray[3];       // the indices into the vertex list

} ST_POLY4DATA;




//wood new define here...
// a self contained polygon used for the render list
typedef struct CW_POLYF4REND_TYP
{
	int iState;    // state information
	int iAttr;     // physical attributes of polygon
	int iColor;    // color of polygon

	POINT4D vlist[3];  // the vertices of this triangle
	POINT4D tvlist[3]; // the vertices after transformation if needed

	CW_POLYF4REND_TYP *pNext; // pointer to next polygon in list??
	CW_POLYF4REND_TYP *pPrev; // pointer to previous polygon in list??

}ST_POLY4REND;






#endif