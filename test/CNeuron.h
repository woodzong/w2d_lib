#ifndef __CNEURON_H_123123asdasdasd
#define __CNEURON_H_123123asdasdasd

#include "WDefine.h"
#include "CNNCfg.h"

typedef struct stNeuronRet
{
	int m_iNum;
	double m_afRet[NN_MAX_INPUT_NUM_PER_NEURON];

	stNeuronRet()
	{
		m_iNum = 0;
	}
}ST_NEURON_RET;

class CNeuronRetUtil
{
public:
	static void AddRet( ST_NEURON_RET * pstRet, double fRet );
};

class CNeuron
{
public:
	CNeuron() { m_iInputNum = 0; }

	CNeuron( int iInputNum )
	{
		assert( ( iInputNum + 1 ) < NN_MAX_INPUT_NUM_PER_NEURON );
		RandInit( iInputNum );
	}

	int GetInputNum() const { return m_iInputNum; }

	double GetWeightByIndex( int iIndex );

	int GetWeightNum() const { return m_iInputNum; }
	void SetWeightByIndex( int iIndex, double fWeight );

	void RandInit( int iInputNum );

private:
	int m_iInputNum;
	double m_afWeight[NN_MAX_INPUT_NUM_PER_NEURON];
};

#endif
