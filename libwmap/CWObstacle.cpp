#include "CWObstacle.h"
#include "CWDraw.h"
#include "CWTimer.h"
#include "CWFont.h"
#include "CWMath.h"
#include "CWCamera.h"
#include "CWGlobalUtil.h"


int CWObstacle::s_iTotalNum = 0;
CWObstacle * CWObstacle::s_apList[MAX_OBSTACLE_NUM_IN_WORLD];

void CWObstacle::AddObstacle( CWObstacle * pObstacle )
{
	s_apList[s_iTotalNum] = pObstacle;
	s_iTotalNum++;
}


int CWObstacle::Init( int iID, const char * szResBMPFile, const CWVector2<float> & pos, float fRadius, DWORD dwColor )
{
	SetIsShowID( false );
	return CWUnit::Init( iID, szResBMPFile, pos, fRadius, dwColor );
}


void CWObstacle::RenderOut( )
{
	CWUnit::RenderOut();
}
