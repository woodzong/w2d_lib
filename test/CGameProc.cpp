#include "CGameProc.h"
#include "CWGlobalConf.h"
#include "CWoodLog.h"
#include "CWDraw.h"
#include "CWFont.h"
#include "CWTimer.h"
#include "CWTimePass.h"
#include "CWGlobalUtil.h"
#include "CWVector2.h"
#include "CWPathMoveUnit.h"
#include "CWCamera.h"
#include "CWMath.h"
#include "WMain.h"

int CGameProc::GameInit()
{
	int iRetCode = 0;

	s_fLastTickTime = 0.0f;

	CWGlobalUtil::Init();

	WGCAMERA->InitWindowSize( GCONF->m_iWindowsWidth, GCONF->m_iWindowsHeight );
	WGCAMERA->SetOrginPointType( EN_ORGINPOINT_TYPE_LEFT_BOTTOM );
	//WGCAMERA->SetPos( ( float )GCONF->m_iWindowsWidth / 2, ( float ) GCONF->m_iWindowsHeight / 2 );

	iRetCode = WGFONT->Init( CW_RES_FILE_PATH( FONT_RES_BMP ), FONT_PER_CHAR_WIDTH, FONT_LOAD_CHAR_MARGIN );
	CHECK_ERR( iRetCode, "WGFONT->Init Failed" );
	WGFONT->SetColor( FONT_COLOR );

	//start init small font
	iRetCode = WGFONT_SMALL->Init( CW_RES_FILE_PATH( FONT_SMALL_RES_BMP ), FONT_SMALL_PER_CHAR_WIDTH, FONT_SMALL_LOAD_CHAR_MARGIN );
	CHECK_ERR( iRetCode, "GFONT_SMALL->Init Failed" );
	WGFONT_SMALL->SetColor( WCOLOR_RED );

	iRetCode = GameInit_i();
	CHECK_ERR( iRetCode, "GameInit_i Failed" );

	s_fLastTickTime = WGTIMER->GetTime();

	return 0;
}


#define _DEBUG_FIXE_DETAL_TIME_IN_60FPS_

int CGameProc::GameRun()
{
	float fTimeNow = WGTIMER->GetTime();

#ifdef _DEBUG_FIXE_DETAL_TIME_IN_60FPS_
	float fDeltaTime = 1000 / 60;
#else
	float fDeltaTime = fTimeNow - s_fLastTickTime;
#endif

	int iRetCode = GameRun_i( fDeltaTime );
	CHECK_ERR( iRetCode, "GameRun_i Failed" );

	s_fLastTickTime = fTimeNow;
	return 0;
}


int CGameProc::HandleKeyDown( EN_WGKEY enKey )
{
	float fMoveSpeed = 1.0f;

	switch( enKey )
	{
		case EN_WGKEY_UP:
			WGCAMERA->MoveUp( fMoveSpeed );
			break;

		case EN_WGKEY_DOWN:
			WGCAMERA->MoveDown( fMoveSpeed );
			break;

		case EN_WGKEY_LEFT:
			WGCAMERA->MoveLeft( fMoveSpeed );
			break;

		case EN_WGKEY_RIGHT:
			WGCAMERA->MoveRight( fMoveSpeed );
			break;

		case EN_WGKEY_F11:
			WGCFG.m_bIsInQuickMode = true;
			break;

		case EN_WGKEY_F12:
			WGCFG.m_bIsInQuickMode = false;
			break;

		case EN_WGKEY_NUM0:
			HandleNumKeyDown( 0 );
			break;
		case EN_WGKEY_NUM1:
			HandleNumKeyDown( 1 );
			break;
		case EN_WGKEY_NUM2:
			HandleNumKeyDown( 2 );
			break;
		case EN_WGKEY_NUM3:
			HandleNumKeyDown( 3 );
			break;
		case EN_WGKEY_NUM4:
			HandleNumKeyDown( 4 );
			break;
		case EN_WGKEY_NUM5:
			HandleNumKeyDown( 5 );
			break;
		case EN_WGKEY_NUM6:
			HandleNumKeyDown( 6 );
			break;
		case EN_WGKEY_NUM7:
			HandleNumKeyDown( 7 );
			break;
		case EN_WGKEY_NUM8:
			HandleNumKeyDown( 8 );
			break;
		case EN_WGKEY_NUM9:
			HandleNumKeyDown( 9 );
			break;
	}
	return 0;
}


int CGameProc::GameShutDown()
{
	return 0;
}