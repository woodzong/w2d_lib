#include "WDefine.h"
#include "CNNGenome.h"
#include "CWMath.h"
#include "CNNCfg.h"
#include "CNNPopulation.h"
#include "CNNSweeper.h"

int NNSortGenomeByFitness( const void *left, const void *right )
{
	const CNNGenome * pGenomeLeft = *( const CNNGenome ** )( left );
	const CNNGenome * pGenomeRight = *( const CNNGenome ** )( right );

	double fDFitScore = pGenomeLeft->GetFitness( ) - pGenomeRight->GetFitness( );

	if( fDFitScore > 0 )
	{
		return -1;
	}
	else
	{
		return 1;
	}
}

void CNNPopulation::RecalTotalFitness()
{
	m_fTotalFitness = 0.0f;
	for( int i = 0; i < m_iGenomeNum; i++ )
	{
		m_fTotalFitness += m_aGenome[i].GetFitness();
	}
}

void CNNPopulation::Init( int iGenomeNum, int iWeightNumPerGenoem )
{
	m_iGenomeNum = iGenomeNum;
	for( int i = 0; i < m_iGenomeNum; i++ )
	{
		m_aGenome[i].RandInit( iWeightNumPerGenoem );
		m_apGenome[i] = &( m_aGenome[i] );
	}

	qsort( m_apGenome, m_iGenomeNum, sizeof( m_apGenome[0] ), NNSortGenomeByFitness );
	RecalTotalFitness();
}

int CNNPopulation::SelectParentIndex()
{
	int iIndexOut = 0;

	double fRand = CWMathSim::RandFloatIn01( ) * m_fTotalFitness;

	double fTmpTotal = 0.0f;
	for( int i = 0; i < m_iGenomeNum; i++ )
	{
		fTmpTotal += m_aGenome[i].GetFitness();

		if( fRand <= fTmpTotal )
		{
			return i;
		}
	}

	return 0;
}


void CNNPopulation::CrossoverPMX( CNNGenome * pGenomeParentA, CNNGenome * pGenomeParentB, CNNGenome * pGenomeChildA, CNNGenome * pGenomeChildB )
{
	double fRand = CWMathSim::RandFloatIn01( );
	if( fRand > NNMS_GAMECFG->m_fTSPCrossOverRate )
	{
		*pGenomeChildA = *pGenomeParentA;
		*pGenomeChildB = *pGenomeParentB;
		return;
	}

	int iSwapIndex = CWMathSim::RangeRandInt( 0, pGenomeChildA->GetWeightNum( ) );
	for( int i = 0; i < iSwapIndex; i++ )
	{
		pGenomeChildA->SetWeightByIndex( i, pGenomeParentA->GetWeightByIndex( i ));
		pGenomeChildB->SetWeightByIndex( i, pGenomeParentB->GetWeightByIndex( i ));
	}

	for( int i = iSwapIndex; i < pGenomeChildA->GetWeightNum( ); i++ )
	{
		pGenomeChildA->SetWeightByIndex( i, pGenomeParentB->GetWeightByIndex( i ) );
		pGenomeChildB->SetWeightByIndex( i, pGenomeParentA->GetWeightByIndex( i ) );
	}

	/*
	double fSwapRate = CWMathSim::RandFloatIn01( ) * pGenomeChildA->GetWeightNum( );
	for( int i = 0; i < pGenomeChildA->GetWeightNum( ); i++ )
	{
		if( CWMathSim::RandFloatIn01() < fSwapRate ) //swap it
		{
			pGenomeChildA->SetWeightByIndex( i, pGenomeParentB->GetWeightByIndex( i ) );
			pGenomeChildB->SetWeightByIndex( i, pGenomeParentA->GetWeightByIndex( i ) );
		}
		else
		{
			pGenomeChildA->SetWeightByIndex( i, pGenomeParentA->GetWeightByIndex( i ) );
			pGenomeChildB->SetWeightByIndex( i, pGenomeParentB->GetWeightByIndex( i ) );
		}
	}
	*/
}

void CNNPopulation::Update()
{
	qsort( m_apGenome, m_iGenomeNum, sizeof( m_apGenome[0] ), NNSortGenomeByFitness );
	RecalTotalFitness();

	int iBestShowNum = NNMS_GAMECFG->m_iTSPEliteNum;
	iBestShowNum = 1;

	for( int i = 0; i < iBestShowNum; i++ )
	{
		m_apGenome[i]->GetBindSweeper( )->SetDrawBold( true );
	}	

	for( int i = iBestShowNum; i < m_iGenomeNum; i++ )
	{
		m_apGenome[i]->GetBindSweeper( )->SetDrawBold( false );
	}
}

void CNNPopulation::Epoch()
{
	//qsort( m_apGenome, m_iGenomeNum, sizeof( m_apGenome[0] ), NNSortGenomeByFitness );
	//RecalTotalFitness();

	CNNGenome aGenomeNext[MAX_GENOMENUM_IN_POPULATION];
	for( int i = 0; i < m_iGenomeNum; i++ )
	{		
		aGenomeNext[i] = m_aGenome[i];
	}

	int iIndex = 0;
	for( int i = 0; i < NNMS_GAMECFG->m_iTSPEliteNum; i++ ) //把排名最高的n个精英, 每个拷贝x份到下一代.  n 是 m_iTSPEliteNum, x 是 m_iTSPEliteCopyNum
	{
		for( int j = 0; j < NNMS_GAMECFG->m_iTSPEliteCopyNum; j++ )
		{
			aGenomeNext[iIndex] = *(m_apGenome[i]);
			iIndex++;

			assert( iIndex <= m_iGenomeNum );
		}
	}

	//开始杂交和变异
	while( iIndex < m_iGenomeNum - 1 )
	{
		int iParentAIndex = SelectParentIndex();
		int iParentBIndex = SelectParentIndex();

		CNNGenome * pGenomeParentA = &( m_aGenome[iParentAIndex] );
		CNNGenome * pGenomeParentB = &( m_aGenome[iParentBIndex] );
		CNNGenome * pGenomeChildA = &( aGenomeNext[iIndex] );
		CNNGenome * pGenomeChildB = &( aGenomeNext[iIndex + 1] );

		CrossoverPMX( pGenomeParentA, pGenomeParentB, pGenomeChildA, pGenomeChildB );

		pGenomeChildA->Mutate();
		pGenomeChildB->Mutate();

		iIndex += 2;
	}

	for( int i = 0; i < m_iGenomeNum; i++ )
	{
		m_apGenome[i] = &( m_aGenome[i] );
		m_aGenome[i] = aGenomeNext[i];
	}
}