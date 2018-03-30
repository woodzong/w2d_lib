#include "WDefine.h"
#include "CNNGenome.h"
#include "CWMath.h"
#include "CNNCfg.h"

void CNNGenome::RandInit( int iWeightNum )
{
	m_iWeightNum = iWeightNum;
	for( int i = 0; i < m_iWeightNum; i++ )
	{
		m_afWeight[i] = CWMathSim::RandFloatInMinus1To1();
	}
}

void CNNGenome::SetWeightByIndex( int iIndex, double fWeight )
{
	if( iIndex >= m_iWeightNum || iIndex < 0 || iIndex > NN_MAX_INPUT_NUM_PER_GENOME )
	{
		assert( 0 );
		return;
	}

	m_afWeight[iIndex] = fWeight;
}

double CNNGenome::GetWeightByIndex( int iIndex )
{
	if( iIndex >= m_iWeightNum || iIndex < 0 || iIndex > NN_MAX_INPUT_NUM_PER_GENOME )
	{
		assert( 0 );
	}

	return m_afWeight[iIndex];
}

void CNNGenome::Mutate( )
{
	for( int i = 0; i < m_iWeightNum; i++ )
	{
		if( CWMathSim::RandFloatIn01( ) < NNMS_GAMECFG->m_fTSPMutationRate )
		{
			m_afWeight[i] += ( CWMathSim::RandFloatInMinus1To1( ) * NNMS_GAMECFG->m_fMaxPerturbation );
		}
	}
}