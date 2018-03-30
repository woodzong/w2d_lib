#include "CGameProcFlock.h"
#include "CWGlobalConf.h"
#include "CWoodLog.h"
#include "CWDraw.h"
#include "CWFont.h"
#include "CWTimer.h"
#include "CWTimePass.h"
#include "CWGlobalUtil.h"
#include "CWVector2.h"
#include "CWCamera.h"
#include "CWMath.h"

#define THIS_MAX_OBSTACLE_NUM 0
#define THIS_MAX_BOID_NUM 30

#define BOID_MAX_SPEED  (1.0f * 40)

#define BOID_DESIRED_SPEED (BOID_MAX_SPEED/2)

#define INIT_SPEED_MAGNITUDE (BOID_DESIRED_SPEED + BOID_DESIRED_SPEED * ( rand( ) % 100 ) / 1000)

int CGameProcFlock::GameInit_i()
{
	int BOID_NUM_PER_ROW = ( int )CWMathSim::WSqrt( THIS_MAX_BOID_NUM ) ;
	if( BOID_NUM_PER_ROW == 0 )
	{
		BOID_NUM_PER_ROW = 1;
	}
	for( int i = 0; i < THIS_MAX_BOID_NUM && i < MAX_BOIDS_NUM_IN_WORLD; i++ )
	{
		int iXOffSet = ( i % BOID_NUM_PER_ROW );
		int iYOffSet = i / BOID_NUM_PER_ROW;

		float fInitPosX = ( float )( GCONF->m_iWindowsWidth / 8 + iXOffSet * 15.0f );
		float fInitPosY = ( float )( GCONF->m_iWindowsHeight / 8 + iYOffSet * 15.0f );

		m_aBoid[i].Init( i, CW_RES_FILE_PATH( "unit_1.bmp" ), CWVector2<float>( fInitPosX, fInitPosY ), 5, WCOLOR_DEEPBLUE, 1000.0f );
		m_aBoid[i].InitSpeedCfg( BOID_MAX_SPEED, BOID_DESIRED_SPEED );

		m_aBoid[i].SetSpeed( CWVector2<float>( 1, 1 ) );
		m_aBoid[i].SetSpeedMagnitude( INIT_SPEED_MAGNITUDE );

		m_apBoid[i] = &( m_aBoid[i] );
	}


	int OBSTACLE_NUM_PER_ROW = ( int )CWMathSim::WSqrt( THIS_MAX_OBSTACLE_NUM );
	if( OBSTACLE_NUM_PER_ROW == 0 )
	{
		OBSTACLE_NUM_PER_ROW = 1;
	}
	for( int i = 0; i < THIS_MAX_OBSTACLE_NUM && i < MAX_OBSTACLE_NUM_IN_WORLD; i++ )
	{
		int iXOffSet = ( i % OBSTACLE_NUM_PER_ROW );
		int iYOffSet = i / OBSTACLE_NUM_PER_ROW;

		float fInitPosX = ( float )( GCONF->m_iWindowsWidth / 3  + iXOffSet * 20.0f );
		float fInitPosY = ( float )( GCONF->m_iWindowsHeight / 3 + iYOffSet * 20.0f );

		CWObstacle * pObstacle = new CWObstacle();
		pObstacle->Init( i, CW_RES_FILE_PATH( "unit_1.bmp" ), CWVector2<float>( fInitPosX, fInitPosY ), 5, WCOLOR_GRAY );
		CWObstacle::AddObstacle( pObstacle );
	}


	return 0;
}

int CGameProcFlock::GameRun_i( float fDeltaTime )
{
	CWBoid * ( *paBoidsInWorld )[MAX_BOIDS_NUM_IN_WORLD] = &( m_apBoid );

	if( GCTRL->m_bMouseLeftButtonIsClicked == true )
	{
		for( int i = 0; i < THIS_MAX_BOID_NUM && i < MAX_BOIDS_NUM_IN_WORLD; i++ )
		{
			CWVector2<float> oWorldPos;
			WGCAMERA->ConvScreenPosToWorldPos( CWVector2<int>( GCTRL->m_iMouseScreenX, GCTRL->m_iMouseScreenY ), &oWorldPos );
			m_aBoid[i].SetDirectPos( oWorldPos );
		}
	}

	for( int i = 0; i < THIS_MAX_BOID_NUM && i < MAX_BOIDS_NUM_IN_WORLD; i++ )
	{
		//cacl ai logic
		m_aBoid[i].WAICacl( paBoidsInWorld, THIS_MAX_BOID_NUM );
	}

	for( int i = 0; i < THIS_MAX_BOID_NUM && i < MAX_BOIDS_NUM_IN_WORLD; i++ )
	{
		m_aBoid[i].WUpdate( fDeltaTime );
		m_aBoid[i].RenderOut();
	}

	for( int i = 0; i < CWObstacle::GetTotalNum(); i++ )
	{
		CWObstacle * pObstacle = CWObstacle::GetObstacle( i );
		pObstacle->RenderOut();
	}

	return 0;
}