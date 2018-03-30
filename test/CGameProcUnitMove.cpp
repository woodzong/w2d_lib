#include "CGameProcUnitMove.h"
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

#define MASS_PATHMOVEUNIT_NUM 1
#define MASS_VECTORMOVEUNIT_NUM 720
#define MASS_VECTORMOVEUNIT_INROUND_NUM 1

#define INROUND_A_SPEED 0.1f
#define INROUND_CENTER_X 0
#define INROUND_CENTER_Y 0
#define INROUND_R 200

int CGameProcUnitMove::GameInit_i( )
{
	for( int i = 0; i < MASS_PATHMOVEUNIT_NUM && i < MAX_PATHMOVEUNIT_NUM; i++ )
	{
		int j = i;
		float fInitPosX = ( float )( 0.0 + j * 1.0 );
		float fInitPosY = ( float )( j * 1.0 );

		m_aPathMoveUnit[i].Init( 1, CW_RES_FILE_PATH( "unit_1.bmp" ), CWVector2<float>( fInitPosX, fInitPosY ), 20, WCOLOR_DEEPGREEN );
		m_aPathMoveUnit[i].PathAddPoint( ( float )( 31 + j * 1.0 ), 240.0 );
		m_aPathMoveUnit[i].PathAddPoint( ( float )( 31 + j * 1.0 ), 250.0 );
		m_aPathMoveUnit[i].PathAddPoint( ( float )( 31 + j * 1.0 ), 640.0 );
		m_aPathMoveUnit[i].PathAddPoint( ( float )( 31 + j * 1.0 ), 1640.0 );
		m_aPathMoveUnit[i].SetSpeed( 30.0 );
	}

	for( int i = 0; i < MASS_VECTORMOVEUNIT_NUM && i < MAX_VECTORMOVEUNIT_NUM; i++ )
	{
		float fSpeed = 30.0f * ( rand() % 100 ) / 100;
		int j = i;
		float fInitPosX = ( float )( GCONF->m_iWindowsWidth / 2 );
		float fInitPosY = ( float )( GCONF->m_iWindowsHeight / 2 );
		
		m_aVectorMoveUnit[i].Init( 1, CW_RES_FILE_PATH( "unit_1.bmp" ), CWVector2<float>( fInitPosX, fInitPosY ), 20, WCOLOR_DEEPBLUE );
		m_aVectorMoveUnit[i].SetSpeed( fSpeed * cos( j * 2 * WMATH_PI / 360 ), fSpeed * sin( j * 2 * WMATH_PI / 360 ) );
	}

	for( int i = 0; i < MASS_VECTORMOVEUNIT_INROUND_NUM && i < MAX_VECTORMOVEUNIT_INROUND_NUM; i++ )
	{
		float fSpeed = 30.0f * ( rand() % 100 ) / 100;
		int j = i;
		float fInitPosX = ( float )( INROUND_CENTER_X + INROUND_R );
		float fInitPosY = ( float )( INROUND_CENTER_Y );

		m_aVectorMoveUnitInRound[i].Init( 1, CW_RES_FILE_PATH( "unit_1.bmp" ), CWVector2<float>( fInitPosX, fInitPosY ), 20, WCOLOR_DEEPGREEN );
		m_aVectorMoveUnitInRound[i].SetSpeed( 0, INROUND_R * INROUND_A_SPEED );
	}


	
	return 0;
}

int CGameProcUnitMove::GameRun_i( float fDeltaTime )
{
	for( int i = 0; i < MASS_PATHMOVEUNIT_NUM && i < MAX_PATHMOVEUNIT_NUM; i++ )
	{
		m_aPathMoveUnit[i].WUpdate( fDeltaTime );
		m_aPathMoveUnit[i].RenderOut( );
	}

	for( int i = 0; i < MASS_VECTORMOVEUNIT_NUM && i < MAX_VECTORMOVEUNIT_NUM; i++ )
	{
		m_aVectorMoveUnit[i].WUpdate( fDeltaTime );
		m_aVectorMoveUnit[i].RenderOut( );
	}

	for( int i = 0; i < MASS_VECTORMOVEUNIT_INROUND_NUM && i < MAX_VECTORMOVEUNIT_INROUND_NUM; i++ )
	{
		CWVector2<float> vecCurSpeed = m_aVectorMoveUnitInRound[i].GetSpeed( );

		vecCurSpeed.SetX( vecCurSpeed.GetX( ) + ( -INROUND_A_SPEED * INROUND_A_SPEED * ( m_aVectorMoveUnitInRound[i].GetPosition( ).GetX( ) - INROUND_CENTER_X ) ) * fDeltaTime / 1000 );
		vecCurSpeed.SetY( vecCurSpeed.GetY( ) + ( -INROUND_A_SPEED * INROUND_A_SPEED * ( m_aVectorMoveUnitInRound[i].GetPosition( ).GetY( ) - INROUND_CENTER_Y ) ) * fDeltaTime / 1000 );
		m_aVectorMoveUnitInRound[i].SetSpeed( vecCurSpeed );

		m_aVectorMoveUnitInRound[i].WUpdate( fDeltaTime );
		m_aVectorMoveUnitInRound[i].RenderOut( );
	}

	return 0;
}