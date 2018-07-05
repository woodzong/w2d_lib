#ifndef __WMAIN_H_12314asdfas123123
#define __WMAIN_H_12314asdfas123123

#include "CWGlobalConf.h"
#include "CWoodLog.h"
#include "CWDraw.h"
#include "CWFont.h"
#include "CWTimer.h"
#include "CWTimePass.h"
#include "CWGlobalUtil.h"

#include "CGameProc3D.h"
#include "CGameProcUnitMove.h"
#include "CGameProcFlock.h"
#include "CGameProcGridMap.h"

//遗传算法学习
#include "CGameProcGridMapEvolution.h"
#include "CGameProcMapTSP.h"
#include "CGameProcLandingTSP.h"

//神经网络学习
#include "CGameProcNNMineSweeper.h"



typedef struct stGCfg //全局控制配置
{
	bool m_bIsInQuickMode;  //是否在加速模式

	stGCfg()
	{
		m_bIsInQuickMode = false;
	}
} ST_GCFG;

void UpdateMouseCtrl();
int W2D_Init( HWND hWnd, IDirect3DDevice9 **pD3D9Device, void *parms = NULL, int num_parms = 0 );
int W2D_Run( HWND hWnd, IDirect3DDevice9 *pD3D9Device, void *parms = NULL, int num_parms = 0 );
int W2D_Shutdown( void *parms = NULL, int num_parms = 0 );
CGameProc * GetGameProc();

class CGMain
{
public:
	static ST_GCFG s_stGCFG;
};

#define WGAMEPROC ( GetGameProc() )
#define WGCFG (CGMain::s_stGCFG)






#endif

