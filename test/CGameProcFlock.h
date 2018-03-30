#ifndef __GAMEPROCFLOCK_H_123123asdasdasd
#define __GAMEPROCFLOCK_H_123123asdasdasd

#include "WDefine.h"

#include "CGameProc.h"
#include "CWBoid.h"
#include "CWObstacle.h"

class CGameProcFlock :public CGameProc
{
public:
	virtual int GameInit_i( );
	virtual int GameRun_i( float fDeltaTime );

private:
	CWBoid m_aBoid[MAX_BOIDS_NUM_IN_WORLD];
	CWBoid * m_apBoid[MAX_BOIDS_NUM_IN_WORLD];
};

#endif
