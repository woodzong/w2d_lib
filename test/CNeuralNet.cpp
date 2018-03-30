#include "WDefine.h"
#include "CNeuralNet.h"
#include "CNNGenome.h"
#include "CNNCfg.h"
#include "CWMath.h"
#include "CWoodLog.h"

int CNeuralNet::s_iIDIndex = 0;

void CNeuralNet::SetWeight( CNNGenome * pGenome )
{
	int iGenomeWeightIndex = 0;
	for( int i = 0; i < m_iLayerNum; i++ )
	{
		CNeuralLayer * pNeuralLayer = &( m_aoLayer[i] );
		for( int j = 0; j < pNeuralLayer->GetNeuronNum( ); j++ )
		{
			CNeuron * pNeuron = pNeuralLayer->GetNeuronByIndex( j );
			for( int k = 0; k < pNeuron->GetWeightNum(); k++ )
			{
				pNeuron->SetWeightByIndex( k, pGenome->GetWeightByIndex( iGenomeWeightIndex ) );
				iGenomeWeightIndex++;
			}			
		}
	}
}

void CNeuralNet::Init( int iLayerNum, int iNeuronNumPerLayer, int iInputNum, int iOutputNum )
{
	assert( iLayerNum >= 1 );
	assert( iLayerNum < NN_MAX_LAYER_NUM_PER_NET );	

	m_iID = s_iIDIndex + 1;
	s_iIDIndex++;

	m_iWeightNum = 0;

	if( iLayerNum > 1 )
	{
		m_aoLayer[0].RandInit( iNeuronNumPerLayer, iInputNum ); //第一个层
		m_iWeightNum += m_aoLayer[0].GetWeightNum();

		for( int i = 1; i < iLayerNum - 1; i++ )
		{
			m_aoLayer[i].RandInit( iNeuronNumPerLayer, iNeuronNumPerLayer );  //隐藏层里, 输入数和输出数一致
			m_iWeightNum += m_aoLayer[i].GetWeightNum( );
		}

		m_aoLayer[iLayerNum - 1].RandInit( iOutputNum, iNeuronNumPerLayer );  //最后的输出层
		m_iWeightNum += m_aoLayer[iLayerNum - 1].GetWeightNum( );
	}
	else
	{
		m_aoLayer[0].RandInit( iOutputNum, iInputNum ); //直接创建输出层
		m_iWeightNum += m_aoLayer[0].GetWeightNum( );
	}

	m_iOutputNum = iOutputNum;
	m_iInputNum = iInputNum;
	m_iNeuronNumPerLayer = iNeuronNumPerLayer;
	m_iLayerNum = iLayerNum;
}


ST_NEURON_RET * CNeuralNet::Update( ST_NEURON_RET * pstInput )
{
	static ST_NEURON_RET s_stOutput;
	s_stOutput.m_iNum = 0;

	if( pstInput->m_iNum != m_iInputNum )
	{
		//return &s_stOutput;
		assert( 0 );
	}


	ST_NEURON_RET stTmp = *pstInput;
	

	int iWeightIndex = 0;
	for( int i = 0; i < m_iLayerNum; i++ )
	{
		if( m_bIsDebug ) 
		{ 
			CWOOD_LOG_DEBUG( "==== LayerIndex:%d====", i );
		}

		if( i > 0 )
		{
			*pstInput = s_stOutput; //输入就是上一层的输出
		}

		s_stOutput.m_iNum = 0;

		iWeightIndex = 0;

		//for each neuron sum the (inputs * corresponding weights).Throw 
		//the total at our sigmoid function to get the output.
		for( int j = 0; j < m_aoLayer[i].GetNeuronNum(); j++ )
		{
			if( m_bIsDebug )
			{
				CWOOD_LOG_DEBUG( "==== InLayer NeuronIndex:%d ====", j );
			}

			double fNetInput = 0.0;
			double fThisRet = 0.0;
			CNeuron * pNeuron = m_aoLayer[i].GetNeuronByIndex( j );
			if( pNeuron == NULL )
			{
				assert(0);
			}

			//for each weight
			for( int k = 0; k < pNeuron->GetInputNum( ) - 1; k++ )
			{
				//sum the weights x inputs
				fThisRet = pNeuron->GetWeightByIndex( k ) * pstInput->m_afRet[iWeightIndex];
				fNetInput += fThisRet;
				
				if( m_bIsDebug )
				{
					CWOOD_LOG_DEBUG( "I Layer:%3d NeuronI:%3d WIndex:%3d Weight:%8.4f Input:%8.4f TWI:%3d TRet:%8.4f Net:%8.4f",
									 i, j, k, pNeuron->GetWeightByIndex( k ), pstInput->m_afRet[iWeightIndex], iWeightIndex, fThisRet, fNetInput );
				}

				iWeightIndex++;
			}

			fThisRet = pNeuron->GetWeightByIndex( pNeuron->GetInputNum( ) - 1 ) * NNMS_GAMECFG->m_fBias;  //计算最后一个偏移权重
			fNetInput += fThisRet;

			double fOut = CWMathSim::Sigmoid( fNetInput, NNMS_GAMECFG->m_fActivationResponse );
			s_stOutput.m_afRet[s_stOutput.m_iNum] = fOut;
			s_stOutput.m_iNum++;

			if( m_bIsDebug )
			{
				CWOOD_LOG_DEBUG( "E Layer:%d NeuronI:%d Weight:%8.4f TRet:%8.4f Net:%8.4f Out:%8.4f LWeightInNeuron:%8.4f OutputNum:%d",
								 i, j, pNeuron->GetWeightByIndex( pNeuron->GetInputNum() - 1 ), fThisRet, fNetInput, fOut, pNeuron->GetWeightByIndex( pNeuron->GetInputNum() - 1 ), s_stOutput.m_iNum );
			}

			iWeightIndex = 0;
		}
	}

	if( m_bIsDebug )
	{
		CWOOD_LOG_KEY( "===== FINISHED:%d Input:%8.4f %8.4f %8.4f %8.4f Output %8.4f %8.4f %d",
					   m_iID, stTmp.m_afRet[0], stTmp.m_afRet[1], stTmp.m_afRet[2], stTmp.m_afRet[3],
					   s_stOutput.m_afRet[0], s_stOutput.m_afRet[1], ( s_stOutput.m_afRet[0] > s_stOutput.m_afRet[1] ) );
	}

	return &s_stOutput;
}
