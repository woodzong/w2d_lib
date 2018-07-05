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


int CGameProc3D::GameInit_i()
{
	return 0;
}

int CGameProc3D::GameRun_i( float fDeltaTime )
{
	CWUtil::DrawPointDebug(CWVector2<float>(100.0, 100.0), WCOLOR_RED, EN_WDRAW_SIZE_4 );
	return 0;
}