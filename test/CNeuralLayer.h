#ifndef __CNEURALLAYER_H_123123asdasdasd
#define __CNEURALLAYER_H_123123asdasdasd

#include "WDefine.h"
#include "CNeuron.h"

class CNeuralLayer
{
public:
	CNeuralLayer(  )
	{
		m_iNeuronNum = 0;
	}

	int GetNeuronNum() const { return m_iNeuronNum; }

	int GetWeightNum() const;

	CNeuron * GetNeuronByIndex( int iIndex );

	void RandInit( int iNeuronNum, int iInputNumPerNeuron );

private:
	int m_iNeuronNum;
	CNeuron m_aoNeuron[NN_MAX_NEURON_NUM_PER_LAYER];
};

#endif
