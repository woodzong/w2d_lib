#include "WDefine.h"
#include "CNeuron.h"
#include "CWMath.h"


void CNeuronRetUtil::AddRet( ST_NEURON_RET * pstRet, double fRet )
{
	assert( pstRet->m_iNum < NN_MAX_INPUT_NUM_PER_NEURON );
	pstRet->m_afRet[pstRet->m_iNum] = fRet;
	pstRet->m_iNum++;
}

void CNeuron::RandInit( int iInputNum )
{
	m_iInputNum = iInputNum + 1;  //这里要加1, 要多加一个输入, 用于保存偏移阀值的权重
	for( int i = 0; i < m_iInputNum; i++ )
	{
		m_afWeight[i] = CWMathSim::RandFloatInMinus1To1();
	}
}

void CNeuron::SetWeightByIndex( int iIndex, double fWeight )
{
	assert( iIndex < m_iInputNum );
	m_afWeight[iIndex] = fWeight;
}

double CNeuron::GetWeightByIndex( int iIndex )
{
	assert( iIndex < m_iInputNum );
	return m_afWeight[iIndex];
}