#include "CWGridMap.h"
#include "CWVector2.h"
#include "CWCamera.h"
#include "CWDraw.h"
#include "CWPathMoveUnit.h"

typedef enum
{
	WCOLOR_EMPTY = RGB32BIT( 250, 250, 250 ),
	WCOLOR_LINE = RGB32BIT( 210, 210, 210 ),
	WCOLOR_STATIC_BLOCK = RGB32BIT( 50, 50, 50 ),
	WCOLOR_G_BLOCK = RGB32BIT( 150, 150, 150 ),
	WCOLOR_A_BLOCK = RGB32BIT( 100, 100, 150 ),
	WCOLOR_T_BLOCK = RGB32BIT( 150, 100, 100 ),
}WBLOCK_COLOR;


CWGridMap::~CWGridMap( )
{
	if( m_iMapDataID >= 0 )
	{
		ReleaseMap( m_iMapDataID );
	}
}

int CWGridMap::Init( int iMapGridWidth, int iMapGridHeight )
{
	m_dwMapCorColor = WCOLOR_LINE;
	m_iMapDataID = NewOneMap( iMapGridWidth, iMapGridHeight, 1000 );
	return 0;
}

int CWGridMap::FindPath( CWPathMoveUnit * pPathMoveUnit, const CWVector2<float> & rTargetPos )
{
	Vec2 stPosBegin;
	Vec2 stPosEnd;
	stPosBegin.x = ( int )pPathMoveUnit->GetPosition().GetX();
	stPosBegin.y = ( int )pPathMoveUnit->GetPosition().GetY();

	stPosEnd.x = ( int )rTargetPos.GetX();
	stPosEnd.y = ( int )rTargetPos.GetY();

	ST_RET_PATH stRetPath;
	int iFindRet = FindPathAll( m_iMapDataID, &stPosBegin, &stPosEnd, 100, 2, UNIT_GROUND, &stRetPath );
	for( int i = 0; i < stRetPath.len; i++ )
	{
		pPathMoveUnit->PathAddPoint( ( float )stRetPath.astPoints[i].x, ( float )stRetPath.astPoints[i].y );
	}

	return iFindRet;
}

void CWGridMap::RandInitMapBlock( int iRandIndex )
{
	ST_MAPDATA * pMapData = GetMapData( m_iMapDataID );
	for( int i = 0; i < pMapData->iMapWidth; i++ )
	{
		if( i % 2 == 0 )
		{
			SetMapBlockVal( m_iMapDataID, i + iRandIndex, i + iRandIndex, STATIC_BLOCK );
		}
	}

	for( int i = 0; i < pMapData->iMapHeight; i++ )
	{
		if( i % 3 == 0 )
		{
			SetMapBlockVal( m_iMapDataID, i + iRandIndex, i + iRandIndex, UNIT_G_BLOCK );
		}
	}

	InitTestMapBlock( m_iMapDataID );
}


void CWGridMap::SetOneGridBlockVal( int iGridX, int iGridY, uint8_t ucGridVal )
{
	SetMapBlockVal( m_iMapDataID, iGridX, iGridY, ucGridVal );
}


void CWGridMap::RenderOneGrid( int iGridX, int iGridY, uint8_t ucGridVal )
{
	uint32_t dwColor = 0;
	if( ucGridVal & STATIC_BLOCK )
	{
		dwColor += WCOLOR_STATIC_BLOCK;
	}
	else if( ucGridVal & UNIT_G_BLOCK )
	{
		dwColor += WCOLOR_G_BLOCK;
	}
	else if( ucGridVal & UNIT_A_BLOCK )
	{
		dwColor += WCOLOR_A_BLOCK;
	}
	else if( ucGridVal & UNIT_T_BLOCK )
	{
		dwColor += WCOLOR_T_BLOCK;
	}

	if( dwColor != 0 )
	{
		CWVector2<int> oScreenPosA;
		CWVector2<int> oScreenPosB;
		s_pCamera->ConvWorldPosToScreenPos( CWVector2<float>( ( float )( iGridX * PER_CELL ), ( float )( iGridY * PER_CELL ) ), &oScreenPosA );
		s_pCamera->ConvWorldPosToScreenPos( CWVector2<float>( ( float )( ( iGridX + 1 ) * PER_CELL ), ( float )( ( iGridY + 1 ) * PER_CELL ) ), &oScreenPosB );
		s_pDraw->DrawRectangle32Bit( oScreenPosA.GetX() + 1, oScreenPosA.GetY() - 1, oScreenPosB.GetX() - 1, oScreenPosB.GetY() + 1, dwColor );
	}
}

uint8_t CWGridMap::GetGridBlockVal( int iGridX, int iGridY )
{
	return GetMapBlockVal( m_iMapDataID, iGridX, iGridY );
}

void CWGridMap::RenderOut()
{
	ST_MAPDATA * pMapData = GetMapData( m_iMapDataID );
	if( pMapData == NULL )
	{
		return;
	}

	CWVector2<int> oScreenPosA;
	CWVector2<int> oScreenPosB;
	for( int i = 0; i < pMapData->iMapWidth; i++ )
	{
		s_pCamera->ConvWorldPosToScreenPos( CWVector2<float>( ( float )i * PER_CELL, 0.0f ), &oScreenPosA );
		s_pCamera->ConvWorldPosToScreenPos( CWVector2<float>( ( float )i * PER_CELL, ( float )pMapData->iMapHeight * PER_CELL ), &oScreenPosB );
		s_pDraw->DrawLine32Bit( oScreenPosA.GetX(), oScreenPosA.GetY(), oScreenPosB.GetX(), oScreenPosB.GetY(), m_dwMapCorColor );
	}

	for( int i = 0; i < pMapData->iMapHeight; i++ )
	{
		s_pCamera->ConvWorldPosToScreenPos( CWVector2<float>( 0.0f, ( float )i * PER_CELL ), &oScreenPosA );
		s_pCamera->ConvWorldPosToScreenPos( CWVector2<float>( ( float )pMapData->iMapWidth * PER_CELL, ( float )i * PER_CELL ), &oScreenPosB );
		s_pDraw->DrawLine32Bit( oScreenPosA.GetX(), oScreenPosA.GetY(), oScreenPosB.GetX(), oScreenPosB.GetY(), m_dwMapCorColor );
	}

	for( int i = 0; i < pMapData->iMapWidth; i++ )
	{
		for( int j = 0; j < pMapData->iMapHeight; j++ )
		{
			RenderOneGrid( i, j, pMapData->ppMapBlockData[i][j] );
		}
	}
}



