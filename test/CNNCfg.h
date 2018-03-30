#ifndef __CNNCFG_H_1231231asdasd123123
#define __CNNCFG_H_1231231asdasd123123

#include "WDefine.h"
#include <string.h>

#define MAX_GENOMENUM_IN_POPULATION 100
#define NN_MAX_INPUT_NUM_PER_NEURON 17
#define NN_MAX_LAYER_NUM_PER_NET 4
#define NN_MAX_NEURON_NUM_PER_LAYER 20
#define NN_MAX_MINE_NUM 200
#define NN_MAX_INPUT_NUM_PER_GENOME ( NN_MAX_NEURON_NUM_PER_LAYER * NN_MAX_INPUT_NUM_PER_NEURON * NN_MAX_LAYER_NUM_PER_NET )
#define NN_MAX_SWEEPER_NUM ( MAX_GENOMENUM_IN_POPULATION )

typedef struct stGameCfg
{
	int m_iTest;
	float m_fBias;
	float m_fActivationResponse;	
	float m_fMaxPerturbation;

	float m_fPassedTimeInMSForNextGeneration;

	int m_iLayerNum;
	int m_iNeuronNumPerLayer;
	int m_iInputNum;  //第一层输入的神经元数量.
	int m_iOutputNum;

	int m_iSweeperNum;
	int m_iMineNum;

	int m_iTSPEliteNum;
	int m_iTSPEliteCopyNum;
	float m_fTSPCrossOverRate;
	float m_fTSPMutationRate;
} ST_NNMS_GAMECFG;

class CNNCfg
{
public:
	static void Init()
	{
		memset( &m_stCfg, 0, sizeof( m_stCfg ) );
	}

	static ST_NNMS_GAMECFG * GetGameCfg() 
	{
		return &m_stCfg;
	}

private:
	static ST_NNMS_GAMECFG m_stCfg;
};

#define NNMS_GAMECFG ( CNNCfg::GetGameCfg() )

#define NNMS_GAMECFG_R ( *(CNNCfg::GetGameCfg()) )

#endif
