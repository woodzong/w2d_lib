#include "CGameProc3D.h"
#include "CWGlobalConf.h"
#include "CWoodLog.h"
#include "CWDraw.h"
#include "CWFont.h"
#include "CWTimer.h"
#include "CWTimePass.h"
#include "CWGlobalUtil.h"
#include "CWVector2.h"
#include "CWCamera.h"
#include "CWMath.h"
#include "CWUtil.h"
#include "w3d_comm.h"


int CGameProc3D::GameInit_i()
{
	return 0;
}

int CGameProc3D::GameRun_i( float fDeltaTime )
{
	//a cube;
	W_VECTOR3D Location;
	W_VECTOR3D Rotation;
	W_VECTOR3D Scale;

	//init cube;  right hand rule; ÓÒÊÖ×ø±êÏµ
	Location.x = 1.0f; //back-front-axi
	Location.y = 0.5f; //left-right-axi
	Location.z = 1.5f; //down-up-axi

	Rotation.x = 0.0f; //roll
	Rotation.y = 0.0f; //pitch
	Rotation.z = 0.0f; //yaw

	Scale.x = 1.0f;
	Scale.y = 1.0f;
	Scale.z = 1.0f;

	//real point in for cube
	W_VECTOR4D PointWorldLocation[2];
	W_VECTOR4D_INITXYZ(&(PointWorldLocation[0]), 10.0f, 20.0f, 30.0f);
	W_VECTOR4D_INITXYZ(&(PointWorldLocation[1]), 100.0f, -40.0f, 80.0f);
	
	//define camera;
	W_CAM4DV1 Camera3D;
	W_VECTOR4D_INITXYZ(&Camera3D.pos, -100.0, 0.0f, 0.0f);

	//transform world to camera 
	//Move matrix
	W_MATRIX4X4 TWorldToCamera = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		Camera3D.pos.x, Camera3D.pos.y, Camera3D.pos.z, 1.0f };
	

	float AngX = Camera3D.dir.x;
	float AngY = Camera3D.dir.y;
	float AngZ = Camera3D.dir.z;

	W_MATRIX4X4 RCamX_inv = W_MAKE_ROTATE_ANG_X_MATRIX4X4(AngX);
	W_MATRIX4X4 RCamY_inv = W_MAKE_ROTATE_ANG_X_MATRIX4X4(AngY);
	W_MATRIX4X4 RCamZ_inv = W_MAKE_ROTATE_ANG_Z_MATRIX4X4(AngZ);

	W_MATRIX4X4 Mtemp1;
	W_MATRIX4X4 Mtemp2;

	W_Mat_Mul_MATRIX_4X4( &TWorldToCamera, &RCamY_inv, &Mtemp1 ); // CamT x CamY x CamX x CamZ
	W_Mat_Mul_MATRIX_4X4( &RCamX_inv, &RCamZ_inv, &Mtemp2 );
	W_Mat_Mul_MATRIX_4X4( &Mtemp1, &Mtemp2, &TWorldToCamera);

	W_VECTOR4D PointWorldToCameraLocation[2];
	W_Mat_Mul_VECTOR4D_4X4(&(PointWorldLocation[0]), &TWorldToCamera, &(PointWorldToCameraLocation[0]));
	W_Mat_Mul_VECTOR4D_4X4(&(PointWorldLocation[1]), &TWorldToCamera, &(PointWorldToCameraLocation[1]));

	CWUtil::DrawPointDebug(CWVector2<float>(100.0, 100.0), WCOLOR_RED, EN_WDRAW_SIZE_4);
	return 0;
}