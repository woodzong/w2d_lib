#ifndef __GAMEPROCMINESWEEPERNN_H_123123asdasdasd
#define __GAMEPROCMINESWEEPERNN_H_123123asdasdasd

#include "WDefine.h"
#include "CGameProc.h"
#include "CWVectorMoveUnit.h"
#include "CNNSweeper.h"
#include "CNNMineMng.h"
#include "CNNTSP.h"

class CGameProcNNMineSweeper :public CGameProc
{
public:
	virtual int GameInit_i();
	virtual int GameRun_i( float fDeltaTime );
	virtual void HandleNumKeyDown( uint8_t ucNumValue );

private:
	void ManualCtrl( float fDeltaTime );

private:
	CNNMineMng m_oMineMng;

	int m_iSweeperNum;
	CNNSweeper m_aSweeper[NN_MAX_SWEEPER_NUM];

	int m_ucKeyDownValue;
	CNNSweeper m_oMineSweeper;

	bool m_bIsInLogicMode;
};

#endif
