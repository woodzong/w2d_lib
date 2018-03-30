#ifndef __GAMEPROCGRIDMAP_H_123123asdasdasd
#define __GAMEPROCGRIDMAP_H_123123asdasdasd

#include "WDefine.h"

#include "CGameProc.h"
#include "CWGridMap.h"

#include "CWPathMoveUnit.h"

#define WGM_UNIT_NUM 10
#define WGM_MAP_NUM 10

class CGameProcGridMap:public CGameProc
{
public:
	CGameProcGridMap() { m_iShowingMapIndex = 0; }
	virtual int GameInit_i( );
	virtual int GameRun_i( float fDeltaTime );

	virtual void HandleNumKeyDown( uint8_t ucNumValue );

private:
	int m_iShowingMapIndex;
	CWGridMap * m_apGridMap[WGM_MAP_NUM];
	CWPathMoveUnit m_aPathMoveUnit[WGM_MAP_NUM][WGM_UNIT_NUM];
};

#endif
