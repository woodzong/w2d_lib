#ifndef __GAMEPROC_H_123123asdasdasd
#define __GAMEPROC_H_123123asdasdasd

#include <stdio.h>
#include "WDefine.h"

class CGameProc
{
public:
	CGameProc( )
	{
		s_fLastTickTime = 0.0f;
	}

	int GameInit();
	int GameRun();
	int GameShutDown();
	int HandleKeyDown( EN_WGKEY enKey );

	virtual void HandleNumKeyDown( uint8_t ucNumValue ) {};

protected:
	virtual int GameInit_i( ) = 0;
	virtual int GameRun_i( float fDeltaTime ) = 0;

protected:
	float s_fLastTickTime;
};


#endif
