#ifndef __CNNPOPULATION_H_123123asdasdasd
#define __CNNPOPULATION_H_123123asdasdasd

#include "WDefine.h"
#include "CNeuron.h"
#include "CNNGenome.h"

class CNNSweeper;

class CNNPopulation
{
public:
	void Init( int iGenomeNum, int iWeightNumPerGenoem );
	int GetGenomeNum() const { return m_iGenomeNum; }
	CNNGenome * GetGenomeByIndex( int iIndex ) 
	{
		assert( iIndex < m_iGenomeNum );
		assert( iIndex < MAX_GENOMENUM_IN_POPULATION );
		assert( iIndex >= 0 );
		return &( m_aGenome[iIndex] );
	}

	CNNSweeper * GetBestSweeper() { return m_apGenome[0]->GetBindSweeper(); }

	double GetBestFitness( ) const { return m_apGenome[0]->GetFitness( ); }
	double GetTotalFitness( ) const { return m_fTotalFitness; }

	void Update();

	void Epoch();

	int SelectParentIndex();

private:
	void RecalTotalFitness();
	void CrossoverPMX( CNNGenome * pGenomeParentA, CNNGenome * pGenomeParentB, CNNGenome * pGenomeChildA, CNNGenome * pGenomeChildB );

private:
	double m_fTotalFitness;
	CNNGenome * m_apGenome[MAX_GENOMENUM_IN_POPULATION];

	int m_iGenomeNum;
	CNNGenome m_aGenome[MAX_GENOMENUM_IN_POPULATION];
};

#endif
