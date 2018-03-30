#include "CWMath.h"



CWMath::CWMath(){
	InitCosSinTable();
}

void CWMath::InitCosSinTable(){

	// create sin/cos lookup table
	// note the creation of one extra element; 360
	// this helps with logic in using the tables

	// generate the tables 0 - 360 inclusive
	for (int ang = 0; ang <= 360; ang++){
		// convert ang to radians
		float theta = (float)ang*WMATH_PI/(float)180;

		// insert next entry into table
		m_fCosTable[ang] = cos(theta);
		m_fSinTable[ang] = sin(theta);

	} // end for ang

}


float CWMath::FastCos(float theta)
{
	// this function uses the cos_look[] lookup table, but
	// has logic to handle negative angles as well as fractional
	// angles via interpolation, use this for a more robust
	// cos computation that the blind lookup, but with with
	// a slight hit in speed

	// convert angle to 0-359
	theta = fmodf(theta,360);

	// make angle positive
	if (theta < 0) theta+=360.0;

	// compute floor of theta and fractional part to interpolate
	int theta_int    = (int)theta;
	float theta_frac = theta - theta_int;

	// now compute the value of sin(angle) using the lookup tables
	// and interpolating the fractional part, note that if theta_int
	// is equal to 359 then theta_int+1=360, but this is fine since the
	// table was made with the entries 0-360 inclusive
	return(m_fCosTable[theta_int] + 
		theta_frac*(m_fCosTable[theta_int+1] - m_fCosTable[theta_int]));

} // end Fast_Cos


float CWMath::FastSin(float theta)
{
	// this function uses the sin_look[] lookup table, but
	// has logic to handle negative angles as well as fractional
	// angles via interpolation, use this for a more robust
	// sin computation that the blind lookup, but with with
	// a slight hit in speed

	// convert angle to 0-359
	theta = fmodf(theta,360);

	// make angle positive
	if (theta < 0) theta+=360.0;

	// compute floor of theta and fractional part to interpolate
	int theta_int    = (int)theta;
	float theta_frac = theta - theta_int;

	// now compute the value of sin(angle) using the lookup tables
	// and interpolating the fractional part, note that if theta_int
	// is equal to 359 then theta_int+1=360, but this is fine since the
	// table was made with the entries 0-360 inclusive
	return(m_fSinTable[theta_int] + 
		theta_frac*(m_fSinTable[theta_int+1] - m_fSinTable[theta_int]));

} // end Fast_Sin


void CWMath::InitRandSeed( int iRandSeed )
{
	srand( iRandSeed );
}


void CWMath::Matrix_X_Vector4D_4X4( const VECTOR4D * pStVector4D,const MATRIX4X4 * pStMatrix4X4,VECTOR4D * pStVetorOut ){

	// this function multiplies a VECTOR4D against a 
	// 4x4 matrix - ma*mb and stores the result in mprod
	// the function makes no assumptions

	for (int col=0; col < 4; col++)
	{
		// compute dot product from row of ma 
		// and column of mb
		float sum = 0; // used to hold result

		for (int row=0; row<4; row++)
		{
			// add in next product pair
			sum += ( pStVector4D->M[row] * pStMatrix4X4->M[row][col] );
		} // end for index

		// insert resulting col element
		pStVetorOut->M[col] = sum;

	} // end for col

}



void CWMath::Vector4D_ADD(VECTOR4D * pStVector4Da,VECTOR4D * pStVector4Db,VECTOR4D * pStVector4DSum)
{
	// this function adds va+vb and return it in vsum
	pStVector4DSum->x = pStVector4Da->x + pStVector4Db->x;
	pStVector4DSum->y = pStVector4Da->y + pStVector4Db->y;
	pStVector4DSum->z = pStVector4Da->z + pStVector4Db->z;
	pStVector4DSum->w = 1;

} // end VECTOR4D_Add


float CWMath::Vector3D_Length(VECTOR3D * pVecA){
	return( (float)sqrtf( pVecA->x*pVecA->x + pVecA->y*pVecA->y + pVecA->z*pVecA->z) );
}



void CWMath::Vector3D_Normalize(VECTOR3D * pVecA,VECTOR3D * pVecNormal){
	// normalizes the sent vector and returns the result in vn

	VECTOR3D_ZERO(pVecNormal);

	// compute length
	float fLength = Vector3D_Length(pVecA);

	// test for zero length vector
	// if found return zero vector
	if (fLength < EPSILON_E5){
		return;
	}

	float fLengthInv = (float)1.0/fLength;

	// compute normalized version of vector
	pVecNormal->x = pVecA->x*fLengthInv;
	pVecNormal->y = pVecA->y*fLengthInv;
	pVecNormal->z = pVecA->z*fLengthInv;

	return;
}

void CWMath::Plane3D_Init(PLANE3D * pPlane, POINT3D * p0, VECTOR3D * pVecNormal, int iNormalize){
	// this function initializes a 3d plane

	// copy the point
	POINT3D_COPY(&pPlane->p0, p0);

	// if normalize is 1 then the normal is made into a unit vector
	if (!iNormalize){
		VECTOR3D_COPY(&pPlane->n, pVecNormal);
	}
	else{
		// make normal into unit vector
		Vector3D_Normalize(pVecNormal,&pPlane->n);
	} // end else
}

void CWMath::Matrix4X4Init(MATRIX4X4 * ma, 
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

}; // end Mat_Init_4X4


void CWMath::Matrix4X4Mul(MATRIX4X4 * ma, 
				 MATRIX4X4 * mb,
				 MATRIX4X4 * mprod)
{
	// this function multiplies two 4x4 matrices together and 
	// and stores the result in mprod
	// note later we will take advantage of the fact that we know
	// that w=1 always, and that the last column of a 4x4 is
	// always 0

	for (int row=0; row<4; row++)
	{
		for (int col=0; col<4; col++)
		{
			// compute dot product from row of ma 
			// and column of mb

			float sum = 0; // used to hold result

			for (int index=0; index<4; index++)
			{
				// add in next product pair
				sum+=(ma->M[row][index]*mb->M[index][col]);
			} // end for index

			// insert resulting row,col element
			mprod->M[row][col] = sum;

		} // end for col

	} // end for row

} // end Mat_Mul_4X4

void CWMath::Build_XYZ_Move_MATRIX4X4(float x,
								  float y, 
								  float z,
								  MATRIX4X4* mmove){
	Matrix4X4Init(mmove,  1,    0,    0,    0,
		                  0,    1,    0,    0,
		                  0,    0,    1,    0,
		                  x,    y,    z,    1);	
}



void CWMath::Build_XYZ_Rotation_MATRIX4X4(float theta_x, // euler angles
								  float theta_y, 
								  float theta_z,
								  MATRIX4X4* mrot) // output 
{
	// this helper function takes a set if euler angles and computes
	// a rotation matrix from them, usefull for object and camera
	// work, also  we will do a little testing in the function to determine
	// the rotations that need to be performed, since there's no
	// reason to perform extra matrix multiplies if the angles are
	// zero!

	MATRIX4X4 mx, my, mz, mtmp;       // working matrices
	float sin_theta=0, cos_theta=0;   // used to initialize matrices
	int rot_seq = 0;                  // 1 for x, 2 for y, 4 for z

	// step 0: fill in with identity matrix
	MAT_IDENTITY_4X4(mrot);

	// step 1: based on zero and non-zero rotation angles, determine
	// rotation sequence
	if (fabs(theta_x) > EPSILON_E5) // x
		rot_seq = rot_seq | 1;

	if (fabs(theta_y) > EPSILON_E5) // y
		rot_seq = rot_seq | 2;

	if (fabs(theta_z) > EPSILON_E5) // z
		rot_seq = rot_seq | 4;

	// now case on sequence
	switch(rot_seq)
	{
	case 0: // no rotation
		{
			// what a waste!
			return;
		} break;

	case 1: // x rotation
		{
			// compute the sine and cosine of the angle
			cos_theta = CW_MATH->FastCos(theta_x);
			sin_theta = CW_MATH->FastSin(theta_x);

			// set the matrix up 
			Matrix4X4Init(&mx, 1,    0,          0,         0,
				0,    cos_theta,  sin_theta, 0,
				0,   -sin_theta, cos_theta, 0,
				0,    0,          0,         1);

			// that's it, copy to output matrix
			MAT_COPY_4X4(&mx, mrot);
			return;

		} break;

	case 2: // y rotation
		{
			// compute the sine and cosine of the angle
			cos_theta = CW_MATH->FastCos(theta_y);
			sin_theta = CW_MATH->FastSin(theta_y);

			// set the matrix up 
			Matrix4X4Init(&my,cos_theta, 0, -sin_theta, 0,  
				0,         1,  0,         0,
				sin_theta, 0, cos_theta,  0,
				0,         0, 0,          1);


			// that's it, copy to output matrix
			MAT_COPY_4X4(&my, mrot);
			return;

		} break;

	case 3: // xy rotation
		{
			// compute the sine and cosine of the angle for x
			cos_theta = CW_MATH->FastCos(theta_x);
			sin_theta = CW_MATH->FastSin(theta_x);

			// set the matrix up 
			Matrix4X4Init(&mx, 1,    0,          0,         0,
				0,    cos_theta,  sin_theta, 0,
				0,   -sin_theta, cos_theta, 0,
				0,    0,          0,         1);

			// compute the sine and cosine of the angle for y
			cos_theta = CW_MATH->FastCos(theta_y);
			sin_theta = CW_MATH->FastSin(theta_y);

			// set the matrix up 
			Matrix4X4Init(&my,cos_theta, 0, -sin_theta, 0,  
				0,         1,  0,         0,
				sin_theta, 0, cos_theta,  0,
				0,         0, 0,          1);

			// concatenate matrices 
			Matrix4X4Mul(&mx, &my, mrot);
			return;

		} break;

	case 4: // z rotation
		{
			// compute the sine and cosine of the angle
			cos_theta = CW_MATH->FastCos(theta_z);
			sin_theta = CW_MATH->FastSin(theta_z);

			// set the matrix up 
			Matrix4X4Init(&mz, cos_theta, sin_theta, 0, 0,  
				-sin_theta, cos_theta, 0, 0,
				0,         0,         1, 0,
				0,         0,         0, 1);


			// that's it, copy to output matrix
			MAT_COPY_4X4(&mz, mrot);
			return;

		} break;

	case 5: // xz rotation
		{
			// compute the sine and cosine of the angle x
			cos_theta = CW_MATH->FastCos(theta_x);
			sin_theta = CW_MATH->FastSin(theta_x);

			// set the matrix up 
			Matrix4X4Init(&mx, 1,    0,          0,         0,
				0,    cos_theta,  sin_theta, 0,
				0,   -sin_theta, cos_theta, 0,
				0,    0,          0,         1);

			// compute the sine and cosine of the angle z
			cos_theta = CW_MATH->FastCos(theta_z);
			sin_theta = CW_MATH->FastSin(theta_z);

			// set the matrix up 
			Matrix4X4Init(&mz, cos_theta, sin_theta, 0, 0,  
				-sin_theta, cos_theta, 0, 0,
				0,         0,         1, 0,
				0,         0,         0, 1);

			// concatenate matrices 
			Matrix4X4Mul(&mx, &mz, mrot);
			return;

		} break;

	case 6: // yz rotation
		{
			// compute the sine and cosine of the angle y
			cos_theta = CW_MATH->FastCos(theta_y);
			sin_theta = CW_MATH->FastSin(theta_y);

			// set the matrix up 
			Matrix4X4Init(&my,cos_theta, 0, -sin_theta, 0,  
				0,         1,  0,         0,
				sin_theta, 0, cos_theta,  0,
				0,         0, 0,          1);

			// compute the sine and cosine of the angle z
			cos_theta = CW_MATH->FastCos(theta_z);
			sin_theta = CW_MATH->FastSin(theta_z);

			// set the matrix up 
			Matrix4X4Init(&mz, cos_theta, sin_theta, 0, 0,  
				-sin_theta, cos_theta, 0, 0,
				0,         0,         1, 0,
				0,         0,         0, 1);

			// concatenate matrices 
			Matrix4X4Mul(&my, &mz, mrot);
			return;

		} break;

	case 7: // xyz rotation
		{
			// compute the sine and cosine of the angle x
			cos_theta = CW_MATH->FastCos(theta_x);
			sin_theta = CW_MATH->FastSin(theta_x);

			// set the matrix up 
			Matrix4X4Init(&mx, 1,    0,         0,         0,
				0,    cos_theta, sin_theta, 0,
				0,   -sin_theta, cos_theta, 0,
				0,    0,         0,         1);

			// compute the sine and cosine of the angle y
			cos_theta = CW_MATH->FastCos(theta_y);
			sin_theta = CW_MATH->FastSin(theta_y);

			// set the matrix up 
			Matrix4X4Init(&my,cos_theta, 0, -sin_theta, 0,  
				0,         1,  0,         0,
				sin_theta, 0,  cos_theta,  0,
				0,         0,  0,          1);

			// compute the sine and cosine of the angle z
			cos_theta = CW_MATH->FastCos(theta_z);
			sin_theta = CW_MATH->FastSin(theta_z);

			// set the matrix up 
			Matrix4X4Init(&mz, cos_theta, sin_theta, 0, 0,  
				-sin_theta, cos_theta, 0, 0,
				0,         0,         1, 0,
				0,         0,         0, 1);

			// concatenate matrices, watch order!
			Matrix4X4Mul(&mx, &my, &mtmp);
			Matrix4X4Mul(&mtmp, &mz, mrot);

		} break;

	default: break;

	} // end switch

} // end Build_XYZ_Rotation_MATRIX4X4      


