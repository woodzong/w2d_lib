#ifndef __CWOBSTACLE_H_1231823718231263
#define __CWOBSTACLE_H_1231823718231263

#include "CWUnit.h"

#define MAX_OBSTACLE_NUM_IN_WORLD 1000

class CWObstacle :public CWUnit
{
public:
	static int s_iTotalNum;
	static CWObstacle * s_apList[MAX_OBSTACLE_NUM_IN_WORLD];
	static void AddObstacle( CWObstacle * pObstacle );
	static int GetTotalNum() { return s_iTotalNum; }
	static CWObstacle * GetObstacle( int iIndex ) { return s_apList[iIndex]; }


	int Init( int iID, const char * szResBMPFile, const CWVector2<float> & pos, float fRadius, DWORD dwColor = UNIT_DEFAULT_COLOR );
	virtual void RenderOut( );
};

#endif
