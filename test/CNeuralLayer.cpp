#include "WDefine.h"
#include "CNeuralLayer.h"

void CNeuralLayer::RandInit( int iNeuronNum, int iInputNumPerNeuron )
{
	assert( iNeuronNum < NN_MAX_NEURON_NUM_PER_LAYER );
	m_iNeuronNum = iNeuronNum;
	for( int i = 0; i < m_iNeuronNum; i++ )
	{
		m_aoNeuron[i].RandInit( iInputNumPerNeuron );
	}
}

CNeuron * CNeuralLayer::GetNeuronByIndex( int iIndex )
{
	if( iIndex < 0 || iIndex >= m_iNeuronNum || iIndex >= NN_MAX_NEURON_NUM_PER_LAYER )
	{
		return NULL;
	}

	return &(m_aoNeuron[iIndex]);
}

int CNeuralLayer::GetWeightNum() const
{
	int iWeightNum = 0;
	for( int i = 0; i < m_iNeuronNum; i++ )
	{
		iWeightNum += m_aoNeuron[i].GetInputNum();
	}
	return iWeightNum;
}