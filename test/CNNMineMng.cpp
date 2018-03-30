#include "WDefine.h"
#include "CNNMineMng.h"
#include "CNNCfg.h"
#include "CWGlobalConf.h"

void CNNMineMng::RandInitOneMine( CWUnit * pMine )
{
	pMine->Init( 0, CW_RES_FILE_PATH( "mine_1.bmp" ), CWVector2<float>( CWMathSim::RandFloatIn01( ) * 10240, CWMathSim::RandFloatIn01( ) * 7680 ), 20, WCOLOR_DEEPBLUE );
	pMine->SetIsShowID( false );
}

void CNNMineMng::Init( int iMineNum )
{
	assert( iMineNum < NN_MAX_MINE_NUM );
	m_iMineNum = iMineNum;

	for( int i = 0; i < m_iMineNum; i++ )
	{
		RandInitOneMine( &(m_aMine[i]) );
	}
}

CWUnit * CNNMineMng::GetClosestMine( const CWVector2<float> & rPos )
{
	CWUnit * pMine = NULL;
	double fMinSqrDist = 999999999.0f;
	for( int i = 0; i < m_iMineNum; i++ )
	{
		double fTmp = rPos.SqrDistance( m_aMine[i].GetPosition( ) );
		if( fTmp < fMinSqrDist )
		{
			fMinSqrDist = fTmp;
			pMine = &( m_aMine[i] );
		}
	}

	return pMine;
}

int CNNMineMng::MineBeSwept( const CWVector2<float> & rSweeperPos )
{
	int iSweptNum = 0;
	double fSweptSqrDist = 5000.0f;

	for( int i = 0; i < m_iMineNum; i++ )
	{
		CWUnit * pMine = &( m_aMine[i] );
		double fTmp = rSweeperPos.SqrDistance( pMine->GetPosition( ) );
		if( fTmp < fSweptSqrDist )
		{
			iSweptNum++;
			RandInitOneMine( pMine );

			//int iNeedMoveCellNum = m_iMineNum - 1 - i;
			//memmove( &( m_aMine[i] ), &( m_aMine[i + 1] ), iNeedMoveCellNum * sizeof( m_aMine[0] ) );
			//m_iMineNum--;
			//i--; //索引后退一位,
		}
	}

	return iSweptNum;
}

void CNNMineMng::RenderOut()
{
	for( int i = 0; i < m_iMineNum; i++ )
	{
		m_aMine[i].RenderOut();
	}
}