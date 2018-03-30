#ifndef __GAMEPROCUNITMOVE_H_123123asdasdasd
#define __GAMEPROCUNITMOVE_H_123123asdasdasd

#include "WDefine.h"

#include "CGameProc.h"
#include "CWPathMoveUnit.h"
#include "CWVectorMoveUnit.h"

#define MAX_PATHMOVEUNIT_NUM 1000
#define MAX_VECTORMOVEUNIT_NUM 1000
#define MAX_VECTORMOVEUNIT_INROUND_NUM 100

class CGameProcUnitMove :public CGameProc
{
public:
	virtual int GameInit_i( );
	virtual int GameRun_i( float fDeltaTime );

private:
	CWPathMoveUnit m_aPathMoveUnit[MAX_PATHMOVEUNIT_NUM];
	CWVectorMoveUnit m_aVectorMoveUnit[MAX_VECTORMOVEUNIT_NUM];
	CWVectorMoveUnit m_aVectorMoveUnitInRound[MAX_VECTORMOVEUNIT_INROUND_NUM];
};

#endif
