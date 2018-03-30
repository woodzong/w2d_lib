#ifndef __CNNGENOME_H_123123asdasdasd
#define __CNNGENOME_H_123123asdasdasd

#include "WDefine.h"
#include "CNNCfg.h"

class CNNSweeper;

class CNNGenome
{
public:
	CNNGenome( ) { m_fFitness = 0.0f; }

	void RandInit( int iWeightNum );

	void SetFitness( double fFitness ) { m_fFitness = fFitness; }
	double GetFitness( ) const { return m_fFitness; }

	int GetWeightNum() const { return m_iWeightNum; }
	double GetWeightByIndex( int iIndex );
	void SetWeightByIndex( int iIndex, double fWeight );

	void Mutate();

	void SetBindSweeper( CNNSweeper * pSweeper )
	{
		m_pBindSweeper = pSweeper;
	}

	CNNSweeper * GetBindSweeper() { return m_pBindSweeper; }

private:
	CNNSweeper * m_pBindSweeper;

	double m_fFitness;

	int m_iWeightNum;
	double m_afWeight[NN_MAX_INPUT_NUM_PER_GENOME];
};

#endif
