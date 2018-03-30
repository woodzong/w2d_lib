#ifndef __CNEURALNET_H_123123asdasdasd
#define __CNEURALNET_H_123123asdasdasd

#include "WDefine.h"
#include "CNeuralLayer.h"

class CNNGenome;

class CNeuralNet
{
public:
	CNeuralNet( )
	{
		m_bIsDebug = false;
	}

	ST_NEURON_RET * Update( ST_NEURON_RET * pstInput );

	void SetWeight( CNNGenome * pGenome );

	void Init( int iLayerNum, int iNeuronNumPerLayer, int iInputNum, int iOutputNum );

	int GetWeightNum() const { return m_iWeightNum; }

private:
	static int s_iIDIndex;

	int m_iID;

	bool m_bIsDebug;

	int m_iWeightNum;

	int m_iOutputNum;
	int m_iInputNum;

	int m_iNeuronNumPerLayer;

	int m_iLayerNum;
	CNeuralLayer m_aoLayer[NN_MAX_LAYER_NUM_PER_NET];
};

#endif
