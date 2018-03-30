#include "CGameProcGridMap.h"
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


void CGameProcGridMap::HandleNumKeyDown( uint8_t ucNumValue )
{
	m_iShowingMapIndex = ucNumValue % WGM_MAP_NUM;
}


int CGameProcGridMap::GameInit_i( )
{
	WGCAMERA->SetZoom( 10 );

	for( int i = 0; i < WGM_MAP_NUM; i++ )
	{
		m_apGridMap[i] = new CWGridMap();
		m_apGridMap[i]->Init( 300, 300 );
		m_apGridMap[i]->RandInitMapBlock( i );


		for( int j = 0; j < WGM_UNIT_NUM; j++ )
		{			
			float fInitPosX = ( float )( 400.0 + j * 100.0 );
			float fInitPosY = ( float )( j * 100.0 + 200.0 );

			m_aPathMoveUnit[i][j].Init( j, CW_RES_FILE_PATH( "unit_1.bmp" ), CWVector2<float>( fInitPosX, fInitPosY ), 20, WCOLOR_DEEPGREEN );
			m_aPathMoveUnit[i][j].SetSpeed( 10.0 );
		}
	}


	for( int i = 0; i < WGM_MAP_NUM; i++ )
	{
		m_aPathMoveUnit[i][0].PathClear();
		m_apGridMap[i]->FindPath( &( m_aPathMoveUnit[i][0] ), CWVector2<float>( 2420.0f, 810.0f + i * 100 ) );
	}

	return 0;
}

int CGameProcGridMap::GameRun_i( float fDeltaTime )
{
	CWGridMap * pGridMap = m_apGridMap[m_iShowingMapIndex];
	pGridMap->RenderOut();
	m_aPathMoveUnit[m_iShowingMapIndex][0].RenderOut( );

	for( int i = 0; i < WGM_MAP_NUM; i++ )
	{
		delete m_apGridMap[i];
	}

	GameInit_i();

	return 0;
}