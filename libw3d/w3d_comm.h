#pragma once
#include <iostream>

#ifndef W3D_COMM_H_12313sdad456464
#define W3D_COMM_H_12313sdad456464

#define W_POLY4DV1_ATTR_2SIDED      0x0001
#define W_POLY4DV1_ATTR_TRANSPARENT 0x0002
#define W_POLY4DV1_ATTR_8BITCOLOR   0x0004
#define W_POLY4DV1_ATTR_RGB16       0x0008
#define W_POLY4DV1_ATTR_RGB24       0x0010

#define W_POLY4DV1_ATTR_SHADE_MODE_PURE    0x0020
#define W_POLY4DV1_ATTR_SHADE_MODE_FLAT    0x0040
#define W_POLY4DV1_ATTR_SHADE_MODE_GOURAUD 0x0080
#define W_POLY4DV1_ATTR_SHADE_MODE_PHONG   0x0100

#define W_POLY4DV1_STATE_ACTIVE   0x0001
#define W_POLY4DV1_STATE_CLIPPED  0x0002
#define W_POLY4DV1_STATE_BACKFACE 0x0004

// defines for objects version 1
#define W_OBJECT4DV1_MAX_VERTICES           1024  // 64
#define W_OBJECT4DV1_MAX_POLYS              1024 // 128

// render list defines
#define W_RENDERLIST4DV1_MAX_POLYS          32768// 16384

// transformation control flags
#define W_TRANSFORM_LOCAL_ONLY       0  // perform the transformation in place on the
// local/world vertex list 
#define W_TRANSFORM_TRANS_ONLY       1  // perfrom the transformation in place on the 
// "transformed" vertex list

#define W_TRANSFORM_LOCAL_TO_TRANS   2  // perform the transformation to the local
// vertex list, but store the results in the
// transformed vertex list

int W3D_Test();


// TYPES //////////////////////////////////////////////////

// 3x3 matrix /////////////////////////////////////////////
typedef struct W_MATRIX3X3_TYP
{
	union
	{
		float M[3][3]; // array indexed data storage

					   // storage in row major form with explicit names
		struct
		{
			float M00, M01, M02;
			float M10, M11, M12;
			float M20, M21, M22;
		}; // end explicit names

	}; // end union
} W_MATRIX3X3, *W_MATRIX3X3_PTR;


// 2x2 matrix /////////////////////////////////////////////
typedef struct W_MATRIX2X2_TYP
{
	union
	{
		float M[2][2]; // array indexed data storage

					   // storage in row major form with explicit names
		struct
		{
			float M00, M01;
			float M10, M11;
		}; // end explicit names

	}; // end union
} W_MATRIX2X2, *W_MATRIX2X2_PTR;

// 4x3 matrix /////////////////////////////////////////////
typedef struct W_MATRIX4X3_TYP
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

} W_MATRIX4X3, *W_MATRIX4X3_PTR;


// 1x4 matrix /////////////////////////////////////////////
typedef struct W_MATRIX1X4_TYP
{
	union
	{
		float M[4]; // array indexed data storage

					// storage in row major form with explicit names
		struct
		{
			float M00, M01, M02, M03;
		}; // end explicit names

	}; // end union
} W_MATRIX1X4, *W_MATRIX1X4_PTR;

// 4x4 matrix /////////////////////////////////////////////
typedef struct W_MATRIX4X4_TYP
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

} W_MATRIX4X4, *W_MATRIX4X4_PTR;


// 4D homogeneous vector, point with w ////////////////////
typedef struct W_VECTOR4D_TYP
{
	union
	{
		float M[4]; // array indexed storage

					// explicit names
		struct
		{
			float x, y, z, w;
		}; // end struct
	}; // end union

} W_VECTOR4D, W_POINT4D, *W_VECTOR4D_PTR, *W_POINT4D_PTR;

typedef struct W_POLY4DV1_TYP
{
	int state;
	int attr;
	int color;

	W_POINT4D_PTR vlist;
	int vert[3];
} W_POLY4DV1, *W_POLY4DV1_PTR;


typedef struct W_POLYF4DV1_TYP
{
	int state;    // state information
	int attr;     // physical attributes of polygon
	int color;    // color of polygon

	W_POINT4D vlist[3];  // the vertices of this triangle
	W_POINT4D tvlist[3]; // the vertices after transformation if needed

	W_POLYF4DV1_TYP *next; // pointer to next polygon in list??
	W_POLYF4DV1_TYP *prev; // pointer to previous polygon in list??

} W_POLYF4DV1, *W_POLYF4DV1_PTR;





// FORWARD REF DEFINES & CONSTANTS ////////////////////////////

// identity matrices

// 4x4 identity matrix
const W_MATRIX4X4 W_IMAT_4X4 = { 1,0,0,0,
0,1,0,0,
0,0,1,0,
0,0,0,1 };

// 4x3 identity matrix (note this is not correct mathematically)
// but later we may use 4x3 matrices with the assumption that 
// the last column is always [0 0 0 1]t
const W_MATRIX4X3 W_IMAT_4X3 = { 1,0,0,
0,1,0,
0,0,1,
0,0,0, };


// 3x3 identity matrix
const W_MATRIX3X3 W_IMAT_3X3 = { 1,0,0,
0,1,0,
0,0,1 };

// 2x2 identity matrix
const W_MATRIX2X2 W_IMAT_2X2 = { 1,0,
0,1 };

// MACROS, SMALL INLINE FUNCS /////////////////////////////////

// matrix macros

// macros to clear out matrices
#define W_MAT_ZERO_2X2(m) {memset((void *)(m), 0, sizeof(W_MATRIX2X2));}
#define W_MAT_ZERO_3X3(m) {memset((void *)(m), 0, sizeof(W_MATRIX3X3));}
#define W_MAT_ZERO_4X4(m) {memset((void *)(m), 0, sizeof(W_MATRIX4X4));}
#define W_MAT_ZERO_4X3(m) {memset((void *)(m), 0, sizeof(W_MATRIX4X3));}

// macros to set the identity matrix
#define W_MAT_IDENTITY_2X2(m) {memcpy((void *)(m), (void *)&W_IMAT_2X2, sizeof(W_MATRIX2X2));}
#define W_MAT_IDENTITY_3X3(m) {memcpy((void *)(m), (void *)&W_IMAT_3X3, sizeof(W_MATRIX3X3));}
#define W_MAT_IDENTITY_4X4(m) {memcpy((void *)(m), (void *)&W_IMAT_4X4, sizeof(W_MATRIX4X4));}
#define W_MAT_IDENTITY_4X3(m) {memcpy((void *)(m), (void *)&W_IMAT_4X3, sizeof(W_MATRIX4X3));}

// matrix copying macros
#define W_MAT_COPY_2X2(src_mat, dest_mat) {memcpy((void *)(dest_mat), (void *)(src_mat), sizeof(W_MATRIX2X2) ); }
#define W_MAT_COPY_3X3(src_mat, dest_mat) {memcpy((void *)(dest_mat), (void *)(src_mat), sizeof(W_MATRIX3X3) ); }
#define W_MAT_COPY_4X4(src_mat, dest_mat) {memcpy((void *)(dest_mat), (void *)(src_mat), sizeof(W_MATRIX4X4) ); }
#define W_MAT_COPY_4X3(src_mat, dest_mat) {memcpy((void *)(dest_mat), (void *)(src_mat), sizeof(W_MATRIX4X3) ); }






typedef struct W_OBJECT4DV1_TYP
{
	int id;
	char name[64];
	int state;
	int attr;
	float avg_radius;
	float max_radius;

	W_POINT4D world_pos;
	W_VECTOR4D dir;
	W_VECTOR4D ux, uy, uz;

	int num_vertices;
	W_POINT4D vlist_local[W_OBJECT4DV1_MAX_VERTICES];
	W_POINT4D vlist_trans[W_OBJECT4DV1_MAX_VERTICES];

	int num_polys;
	W_POLY4DV1 plist[W_OBJECT4DV1_MAX_POLYS];
}W_OBJECT4DV1, *W_OBJECT4DV1_PTR;


typedef struct W_RENDERLIST4DV1_TYP
{
	int state;
	int attr;

	W_POLYF4DV1_PTR poly_ptrs[W_RENDERLIST4DV1_MAX_POLYS];

	W_POLYF4DV1 poly_data[W_RENDERLIST4DV1_MAX_POLYS];

	int num_polys;
}W_RENDERLIST4DV1, *W_RENDERLIST4DV1_PTR;

// 3D vector, point without the w ////////////////////////
typedef struct W_VECTOR3D_TYP
{
	union
	{
		float M[3]; // array indexed storage

					// explicit names
		struct
		{
			float x, y, z;
		}; // end struct

	}; // end union

} W_VECTOR3D, W_POINT3D, *W_VECTOR3D_PTR, *W_POINT3D_PTR;

// 3D plane ///////////////////////////////////////////////////
typedef struct W_PLANE3D_TYP
{
	W_POINT3D p0; // point on the plane
	W_VECTOR3D n; // normal to the plane (not necessarily a unit vector)
} W_PLANE3D, *W_PLANE3D_PTR;


typedef struct W_CAM4DV1_TYP
{
	int state;
	int attr;

	W_POINT4D pos;
	W_VECTOR4D dir;

	W_VECTOR4D u; //unv model
	W_VECTOR4D v;
	W_VECTOR4D n;
	W_POINT4D target;

	float view_dist_h;
	float view_dist_v;

	float fov;

	float near_clip_z;
	float far_clip_z;

	W_PLANE3D rt_clip_plane;
	W_PLANE3D lt_clip_plane;
	W_PLANE3D tt_clip_plane;
	W_PLANE3D bt_clip_plane;

	float viewplane_width;
	float viewplane_height;

	float viewport_width;
	float viewport_height;
	float viewport_center_x;
	float viewport_center_y;

	float aspect_ratio;

	W_MATRIX4X4 mcam;
	W_MATRIX4X4 mper;
	W_MATRIX4X4 mscr;

}W_CAM4DV1, * W_CAM4DV1_PTR;

inline void W_VECTOR4D_COPY(W_VECTOR4D_PTR vdst, W_VECTOR4D_PTR vsrc)
{
	(vdst)->x = (vsrc)->x; 
	(vdst)->y = (vsrc)->y;
	(vdst)->z = (vsrc)->z; 
	(vdst)->w = (vsrc)->w;
}

inline void W_VECTOR4D_Add(W_VECTOR4D_PTR va, W_VECTOR4D_PTR vb, W_VECTOR4D_PTR vsum)
{
	// this function adds va+vb and return it in vsum
	vsum->x = va->x + vb->x;
	vsum->y = va->y + vb->y;
	vsum->z = va->z + vb->z;
	vsum->w = 1;

} // end VECTOR4D_Add

inline void W_Mat_Init_4X4(W_MATRIX4X4_PTR ma,
	float m00, float m01, float m02, float m03,
	float m10, float m11, float m12, float m13,
	float m20, float m21, float m22, float m23,
	float m30, float m31, float m32, float m33)

{
	// this function fills a 4x4 matrix with the sent data in 
	// row major form
	ma->M00 = m00; ma->M01 = m01; ma->M02 = m02; ma->M03 = m03;
	ma->M10 = m10; ma->M11 = m11; ma->M12 = m12; ma->M13 = m13;
	ma->M20 = m20; ma->M21 = m21; ma->M22 = m22; ma->M23 = m23;
	ma->M30 = m30; ma->M31 = m31; ma->M32 = m32; ma->M33 = m33;

} // end Mat_Init_4X4

inline void W_VECTOR4D_INITXYZ(W_VECTOR4D_PTR v, float x, float y, float z)
{
	(v)->x = (x); (v)->y = (y); (v)->z = (z); (v)->w = 1.0;
}

inline void W_VECTOR4D_ZERO(W_VECTOR4D_PTR v)
{
	(v)->x = (v)->y = (v)->z = 0.0; (v)->w = 1.0;
}

void W_Mat_Mul_VECTOR4D_4X4(W_VECTOR4D_PTR  va, W_MATRIX4X4_PTR mb, W_VECTOR4D_PTR  vprod);
void W_Mat_Mul_MATRIX_4X4(W_MATRIX4X4_PTR  ma, W_MATRIX4X4_PTR mb, W_MATRIX4X4_PTR  mprod);

void W_Transform_RENDERLIST4DV1(W_RENDERLIST4DV1_PTR render_list, W_MATRIX4X4_PTR mt, int coord_select);
void W_Transform_OBJECT4DV1( W_OBJECT4DV1_PTR obj, W_MATRIX4X4_PTR mt, int coord_select, int transform_basis );
void W_Model_To_World_OBJECT4DV1(W_OBJECT4DV1_PTR obj, int coord_select = W_TRANSFORM_LOCAL_TO_TRANS );
void W_Build_Model_To_World_MATRIX4X4( W_VECTOR4D_PTR vpos, W_MATRIX4X4_PTR m );
void W_Model_To_World_RENDERLIST4DV1( W_RENDERLIST4DV1_PTR rend_list, W_POINT4D_PTR world_pos, int coord_select = W_TRANSFORM_LOCAL_TO_TRANS );

void W_Init_CAM4DV1( W_CAM4DV1_PTR cam, int cam_attr, W_POINT4D_PTR cam_pos, W_VECTOR4D_PTR cam_dir, W_POINT4D_PTR cam_target, 
	                 float near_clip_z, float far_clip_z, float fov, float viewport_width, float viewport_height );


#define W_MAKE_ROTATE_ANG_X_MATRIX4X4( AngX ) \
	{\
		1,           0,          0, 0, \
		0,  cos(-AngX), sin(-AngX), 0, \
		0, -sin(-AngX), cos(-AngX), 0, \
		0,           0,          0, 1 \
	}

#define W_MAKE_ROTATE_ANG_Y_MATRIX4X4( AngY ) \
	{\
		cos(-AngY),  0,  sin(-AngY),  0,\
		         0,  1,           0,  0,\
		sin(-AngY),  0,  cos(-AngY),  0,\
		         0,  0,           0,  1\
	}

#define W_MAKE_ROTATE_ANG_Z_MATRIX4X4( AngZ ) \
	{\
		  cos(-AngZ), sin(-AngZ),   0,  0,\
		 -sin(-AngZ), cos(-AngZ),   0,  0,\
		           0,          0,   1,  0,\
		           0,          0,   0,  1\
	}


#endif