#include "CGameProcGridMapEvolution.h"
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



#define T_MAP_HEIGHT 10
#define T_MAP_WIDTH  15

#define T_MAP_START_X 0
#define T_MAP_START_Y 2
#define T_MAP_END_X 14
#define T_MAP_END_Y 7


void CGameProcGridMapEvolution::ResetMapData()
{
	uint8_t aucMapData[T_MAP_HEIGHT][T_MAP_WIDTH] =
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1,
	8, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1,
	1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1,
	1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1,
	1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, 1,
	1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 5,
	1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };

	for( int y = 0; y < T_MAP_HEIGHT; y++ )
	{
		for( int x = 0; x < T_MAP_WIDTH; x++ )
		{
			if( aucMapData[y][x] == 1 )
			{
				m_oGridMap.SetOneGridBlockVal( x, y, STATIC_BLOCK );
			}
			else
			{
				m_oGridMap.SetOneGridBlockVal( x, y, FREE_FOR_ALL );
			}
		}
	}

}


int CGameProcGridMapEvolution::GameInit_i( )
{
	WGCAMERA->SetZoom( 10 );

	m_oGridMap.Init( T_MAP_WIDTH, T_MAP_HEIGHT );

	ResetMapData();
	//init gens
	for( int i = 0; i < GENOMES_NUM; i++ )
	{
		for( int j = 0; j < GENOMES_LEN; j++ )
		{
			m_aGenomes[i][j] = CWMathSim::RangeRandInt( T_T_UP, T_T_LEFT );
		}
	}

	return 0;
}




void CGameProcGridMapEvolution::MarkPathInMap( uint8_t * pucPath, int iPathLen )
{
	int iCurX = T_MAP_START_X;
	int iCurY = T_MAP_START_Y;
	m_oGridMap.SetOneGridBlockVal( iCurX, iCurY, UNIT_T_BLOCK );

	for( int i = 0; i < iPathLen; i++ )
	{
		int iNextX = iCurX;
		int iNextY = iCurY;

		uint8_t ucCurDirect = *( pucPath + i );
		switch( ucCurDirect )
		{
			case T_T_UP:
				iNextY++;
				break;

			case T_T_RIGHT:
				iNextX++;
				break;

			case T_T_DOWN:
				iNextY--;
				break;

			case T_T_LEFT:
				iNextX--;
				break;
		}

		if( m_oGridMap.GetGridBlockVal( iNextX, iNextY ) != STATIC_BLOCK )
		{
			iCurX = iNextX;
			iCurY = iNextY;

			m_oGridMap.SetOneGridBlockVal( iCurX, iCurY, UNIT_T_BLOCK );
		}
	}
}



float CGameProcGridMapEvolution::EvalPath( uint8_t * pucPath, int iPathLen, int * pOutLen )
{
	int iCurX = T_MAP_START_X;
	int iCurY = T_MAP_START_Y;	
	
	int iDiffX = abs( iCurX - T_MAP_END_X );
	int iDiffY = abs( iCurY - T_MAP_END_Y );

	for( int i = 0; i < iPathLen; i++ )
	{
		int iNextX = iCurX;
		int iNextY = iCurY;

		uint8_t ucCurDirect = *( pucPath + i );
		switch( ucCurDirect )
		{
			case T_T_UP:
				iNextY++;
				break;

			case T_T_RIGHT:
				iNextX++;
				break;

			case T_T_DOWN:
				iNextY--;
				break;

			case T_T_LEFT:
				iNextX--;
				break;
		}

		if( m_oGridMap.GetGridBlockVal( iNextX, iNextY ) != STATIC_BLOCK )
		{
			iCurX = iNextX;
			iCurY = iNextY;
		}

		iDiffX = abs( iCurX - T_MAP_END_X );
		iDiffY = abs( iCurY - T_MAP_END_Y );

		*pOutLen = i;

		if( iDiffY == 0 && iDiffX == 0 )
		{			
			break;
		}
	}

	return ( 1 / ( float )( iDiffX + iDiffY + 1 ) ) + ( GENOMES_LEN / (float) ( *pOutLen ) ) * 10;
}


int CGameProcGridMapEvolution::SelectParentIndex()
{
	int iIndexOut = 0;

	float fRand = (float)CWMathSim::RandFloat( ) * m_fTotalScore;
	
	float fTmpTotal = 0;
	for( int i = 0; i < GENOMES_NUM; i++ )
	{
		fTmpTotal += m_aGenScores[i];

		if( fRand <= fTmpTotal )
		{
			return i;
		}
	}

	return 0;
}


void CGameProcGridMapEvolution::CrossOver( int iGenLen, uint8_t * pucParentGensA, uint8_t * pucParentGensB, uint8_t * pucBabyA, uint8_t * pucBabyB )
{
	for( int i = 0; i < iGenLen; i++ )
	{
		if( i < iGenLen/2 )
		{
			*( pucBabyA + i ) = *( pucParentGensA + i );
			*( pucBabyB + i ) = *( pucParentGensB + i );
		}
		else
		{
			*( pucBabyB + i ) = *( pucParentGensA + i );
			*( pucBabyA + i ) = *( pucParentGensB + i );
		}
	}

	return;
}

void CGameProcGridMapEvolution::Mutate( int iGenLen, uint8_t * pucGens )
{
	for( int i = 0; i < iGenLen; i++ )
	{
		int iRand = CWMathSim::RangeRandInt( 1, 1000 );
		if( iRand <= 1 ) //1/1000 概率 发生变异.
		{
			*( pucGens + i ) = CWMathSim::RangeRandInt( T_T_UP, T_T_LEFT );
		}
	}
	return;
}



int CGameProcGridMapEvolution::Epoch()
{
	if( m_iGeneration > 0 )
	{
		uint8_t aGenomesNew[GENOMES_NUM][GENOMES_LEN];
		memset( aGenomesNew, 0, sizeof( aGenomesNew ) );
		for( int i = 0; i < GENOMES_NUM; i += 2 ) //每次产生2个baby;
		{
			int iDadIndex = SelectParentIndex();
			int iMumIndex = SelectParentIndex();

			CrossOver( GENOMES_LEN, m_aGenomes[iDadIndex], m_aGenomes[iMumIndex], aGenomesNew[i], aGenomesNew[i + 1] );

			Mutate( GENOMES_LEN, aGenomesNew[i] );
			Mutate( GENOMES_LEN, aGenomesNew[i + 1] );
		}
		memcpy( m_aGenomes, aGenomesNew, sizeof( m_aGenomes ) );
	}


	m_fBestScore = 0;
	m_fTotalScore = 0;
	bool bIsBestChanged = true;
	for( int i = 0; i < GENOMES_NUM; i++ )
	{
		int iGenLen = GENOMES_LEN;
		uint8_t * pucGen = m_aGenomes[i];
		float fScore = EvalPath( pucGen, iGenLen, &( m_aGenLen[i] ) );
		m_aGenScores[i] = fScore;

		m_fTotalScore += fScore;

		if( fScore > m_fBestScore )
		{
			m_fBestScore = fScore;
			m_iBestGenIndex = i;
			bIsBestChanged = true;
		}
	}
		
	if( bIsBestChanged )
	{
		ResetMapData( );
		MarkPathInMap( m_aGenomes[m_iBestGenIndex], m_aGenLen[m_iBestGenIndex] );
	}

	m_iGeneration++;

	return 0;
}


void CGameProcGridMapEvolution::HandleNumKeyDown( uint8_t ucNumValue )
{
	m_bIsNeedEpoch = true;
}

int CGameProcGridMapEvolution::GameRun_i( float fDeltaTime )
{
	if( m_bIsNeedEpoch )
	{
		for( int i = 0; i < 1000; i++ )
		{
			Epoch();
		}
		//m_bIsNeedEpoch = false;
	}

	m_oGridMap.RenderOut();

	/*
	int iRunTime = 0;
	while( m_fBestScore != 1.0f )
	{
		iRunTime ++;
		Epoch();
		if( iRunTime == 10000 )
		{
			break;
		}
	}
	*/

	WGFONT->ShowString( WGDRAW, "%s", "Press Any Num Key for Next Generation" );
	WGFONT->ShowString( WGDRAW, "Generation:%d BestScore:%f TotalScore:%f GenLen:%d", m_iGeneration, m_fBestScore, m_fTotalScore, m_aGenLen[m_iBestGenIndex] );

	//GameInit_i();
	return 0;
}