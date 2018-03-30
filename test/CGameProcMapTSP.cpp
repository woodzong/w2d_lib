#include "CGameProcMapTSP.h"
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
#include "CWUtil.h"


CMapTSPGen::CMapTSPGen()
{
	m_fScore = 0.0f;
	m_fFitScore = 0.0f;
	memset( m_aiGen, 0, sizeof( m_aiGen ) );
}

void CMapTSPGen::RandInit()
{
	for( int i = 0; i < MAPTSP_GEN_LEN; i++ )
	{
		m_aiGen[i] = i;
	}

	for( int i = 0; i < MAPTSP_GEN_LEN - 1; i++ ) //随机洗牌
	{
		int iSwapIndex = CWMathSim::RangeRandInt( i, MAPTSP_GEN_LEN - 1 ); // 这种算法更符合随机
		int iTmpValue = m_aiGen[i];
		m_aiGen[i] = m_aiGen[iSwapIndex];
		m_aiGen[iSwapIndex] = iTmpValue;
	}
}

void CMapTSPGen::SwapGen( int iGenA, int iGenB )
{
	int iGenAIndex = -1;
	int iGenBIndex = -1;
	for( int i = 0; i < MAPTSP_GEN_LEN; i++ )
	{
		if( m_aiGen[i] == iGenA )
		{
			iGenAIndex = i;

			if( iGenAIndex != -1 && iGenBIndex != -1 )
			{
				break;
			}
		}
		else if( m_aiGen[i] == iGenB )
		{
			iGenBIndex = i;

			if( iGenAIndex != -1 && iGenBIndex != -1 )
			{
				break;
			}
		}
	}

	if( iGenAIndex != -1 && iGenBIndex != -1 )
	{
		int iTmp = m_aiGen[iGenAIndex];
		m_aiGen[iGenAIndex] = m_aiGen[iGenBIndex];
		m_aiGen[iGenBIndex] = iTmp;
	}
}


void CMapTSPGen::MuteEM()
{
	int iRandParam = CWMathSim::RangeRandInt( 1, 1000 );
	if( iRandParam > MAPTSP_MUTATION_RATE_1000 )
	{
		return;
	}

	int iRandIndexA = CWMathSim::RangeRandInt( 0, MAPTSP_GEN_LEN - 2 );
	int iRandIndexB = CWMathSim::RangeRandInt( iRandIndexA + 1, MAPTSP_GEN_LEN - 1 );
	int iTmp = m_aiGen[iRandIndexA];
	m_aiGen[iRandIndexA] = m_aiGen[iRandIndexB];
	m_aiGen[iRandIndexB] = iTmp;
}


void CGameProcMapTSP::EvalOneGenScore( CMapTSPGen * pGen )
{
	int iPreCityIndex = 0;
	float fTotalScore = 0.0f;
	for( int i = 0; i < MAPTSP_GEN_LEN; i++ )
	{
		CWPathMoveUnit * pPreCity = &( m_aCity[iPreCityIndex] );

		int iCityIndex = pGen->m_aiGen[i];
		if( iCityIndex < 0 || iCityIndex >= MAPTSP_MAX_CITY_NUM )
		{
			continue;
		}

		CWPathMoveUnit * pThisCity = &( m_aCity[iCityIndex] );
		fTotalScore += CWMathSim::GetDistance( pPreCity->GetPosition().GetX(), pPreCity->GetPosition().GetY(), pThisCity->GetPosition().GetX(), pThisCity->GetPosition().GetY() );

		iPreCityIndex = iCityIndex;
	}

	pGen->m_fScore = fTotalScore;
}


void CGameProcMapTSP::CrossoverPMX( CMapTSPGen * pGenParentA, CMapTSPGen * pGenParentB, CMapTSPGen * pGenBabyA, CMapTSPGen * pGenBabyB )
{
	*pGenBabyA = *pGenParentA;
	*pGenBabyB = *pGenParentB;

	int iRandParam = CWMathSim::RangeRandInt( 1, 1000 );
	if( iRandParam > MAPTSP_CROSSOVER_RATE_1000 )
	{
		return;
	}

	int iCrossBeginIndex = CWMathSim::RangeRandInt( 0, MAPTSP_GEN_LEN - 2 );
	int iCrossEndIndex = CWMathSim::RangeRandInt( iCrossBeginIndex + 1, MAPTSP_GEN_LEN - 1 );

	for( int i = iCrossBeginIndex; i <= iCrossEndIndex; i++ )
	{
		int iAGen = pGenBabyA->m_aiGen[i];
		int iBGen = pGenBabyB->m_aiGen[i];

		if( iAGen != iBGen )
		{
			pGenBabyA->SwapGen( iAGen, iBGen );
			pGenBabyB->SwapGen( iAGen, iBGen );
		}
	}
}


int CGameProcMapTSP::SelectParentIndex()
{
	int iIndexOut = 0;

	float fRand = ( float )CWMathSim::RandFloat() * m_fTotalFitScore;

	float fTmpTotal = 0;
	for( int i = 0; i < MAPTSP_POP_SIZE; i++ )
	{
		fTmpTotal += m_aGen[i].m_fFitScore;

		if( fRand <= fTmpTotal )
		{
			return i;
		}
	}

	return 0;
}


int CGameProcMapTSP::Epoch()
{
	//评估
	m_fBadestScore = 0.0f;
	m_fBestScore = 0.0f;
	for( int i = 0; i < MAPTSP_POP_SIZE; i++ )
	{
		CMapTSPGen * pGen = &( m_aGen[i] );
		EvalOneGenScore( pGen );
		if( pGen->m_fScore <= m_fBestScore || m_fBestScore == 0.0f )
		{
			m_fBestScore = pGen->m_fScore;
			m_iBestGenIndex = i;
		}

		if( pGen->m_fScore >= m_fBadestScore || m_fBadestScore == 0.0f )
		{
			m_fBadestScore = pGen->m_fScore;
			m_iBadestGenIndex = i;
		}
	}

	m_fTotalFitScore = 0.0f;
	for( int i = 0; i < MAPTSP_POP_SIZE; i++ )
	{
		CMapTSPGen * pGen = &( m_aGen[i] );
		pGen->m_fFitScore = m_fBadestScore - pGen->m_fScore;
		m_fTotalFitScore += pGen->m_fFitScore;
	}

	//render best gens;
	CMapTSPGen * pGen = &( m_aGen[m_iBestGenIndex] );
	int iPreCityIndex = pGen->m_aiGen[0];
	for( int i = 1; i < MAPTSP_GEN_LEN; i++ )
	{
		int iThisCityIndex = pGen->m_aiGen[i];

		CWPathMoveUnit * pCityPre = &( m_aCity[iPreCityIndex] );
		CWPathMoveUnit * pCityThis = &( m_aCity[iThisCityIndex] );

		CWUtil::DrawLineInWorld( pCityPre->GetPosition( ), pCityThis->GetPosition( ), WCOLOR_DEEPGREEN, EN_WDRAW_SIZE_2 );
						
		iPreCityIndex = iThisCityIndex;
	}


	//杂交与进化
	CMapTSPGen aGenNew[MAPTSP_POP_SIZE];
	const int BEST_PASS_NUM = 2;
	for( int i = 0; i < BEST_PASS_NUM; i++ )
	{
		aGenNew[i] = m_aGen[m_iBestGenIndex];
	}

	for( int i = BEST_PASS_NUM; i < MAPTSP_POP_SIZE - 1; i += 2 )
	{
		int iParentAIndex = SelectParentIndex();
		int iParentBIndex = SelectParentIndex();

		CMapTSPGen * pGenParentA = &( m_aGen[iParentAIndex] );
		CMapTSPGen * pGenParentB = &( m_aGen[iParentBIndex] );
		CMapTSPGen * pGenBabyA = &( aGenNew[i] );
		CMapTSPGen * pGenBabyB = &( aGenNew[i + 1] );

		CrossoverPMX( pGenParentA, pGenParentB, pGenBabyA, pGenBabyB );
		pGenBabyA->MuteEM();
		pGenBabyB->MuteEM();
	}


	//生成新一代
	for( int i = 0; i < MAPTSP_POP_SIZE; i++ )
	{
		m_aGen[i] = aGenNew[i];
	}

	m_iGeneration++;

	return 0;
}


int CGameProcMapTSP::GameInit_i()
{
	m_iGeneration = 0;
	m_iBestGenIndex = 0;
	m_iBadestGenIndex = 0;
	m_fBestScore = 0.0f;
	m_fBadestScore = 0.0f;

	float fRadius = GCONF->m_iWindowsWidth / ( float )5.0;
	float fAngle = 0.0f;
	float fAngleCell = 2 * WMATH_PI / ( float )MAPTSP_MAX_CITY_NUM;

	for( int i = 0; i < MAPTSP_MAX_CITY_NUM; i++ )
	{
		float fInitPosX = ( float )( GCONF->m_iWindowsWidth / 2 ) + fRadius * sin( fAngle );
		float fInitPosY = ( float )( GCONF->m_iWindowsHeight / 2 ) + fRadius * cos( fAngle );

		m_aCity[i].Init( i, CW_RES_FILE_PATH( "unit_1.bmp" ), CWVector2<float>( fInitPosX, fInitPosY ), 20, WCOLOR_RED );
		fAngle += fAngleCell;
	}

	for( int i = 0; i < MAPTSP_POP_SIZE; i++ )
	{
		m_aGen[i].RandInit();
	}

	return 0;
}

int CGameProcMapTSP::GameRun_i( float fDeltaTime )
{
	Epoch();

	for( int i = 0; i < MAPTSP_MAX_CITY_NUM; i++ )
	{
		m_aCity[i].RenderOut();
	}

	WGFONT->ShowString( WGDRAW, "Generation:%d BestScore:%.4f BadestScore:%.4f TotalFitScore:%.4f", m_iGeneration, m_fBestScore, m_fBadestScore, m_fTotalFitScore );

	return 0;
}