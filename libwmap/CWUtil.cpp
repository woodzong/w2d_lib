#include "CWUnit.h"
#include "CWDraw.h"
#include "CWTimer.h"
#include "CWFont.h"
#include "CWMath.h"
#include "CWCamera.h"
#include "CWGlobalUtil.h"
#include "CWUtil.h"
#include "CWVector2.h"

void CWUtil::DrawLineInWorld( const CWVector2<float>& vWorldPosA, const CWVector2<float>& vWorldPosB, DWORD dwColor, EN_WDRAW_SIZE enDrawSize )
{
	CWVector2<int> oScreenPosA;
	CWVector2<int> oScreenPosB;
	WGCAMERA->ConvWorldPosToScreenPos( vWorldPosA, &oScreenPosA );
	WGCAMERA->ConvWorldPosToScreenPos( vWorldPosB, &oScreenPosB );
	WGDRAW->DrawLine32BitWithSize( oScreenPosA.GetX( ), oScreenPosA.GetY( ), oScreenPosB.GetX( ), oScreenPosB.GetY( ), dwColor, enDrawSize );
}


void CWUtil::DrawPointDebug(const CWVector2<float>& vWorldPos, DWORD dwColor, EN_WDRAW_SIZE enDrawSize)
{
	CWVector2<int> oScreenPos;
	WGCAMERA->ConvWorldPosToScreenPos(vWorldPos, &oScreenPos);
	WGDRAW->DrawPoint32BitWithSize(oScreenPos.GetX(), oScreenPos.GetY(), dwColor, enDrawSize);
}

