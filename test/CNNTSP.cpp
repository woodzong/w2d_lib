#include "WDefine.h"
#include "CNNGenome.h"
#include "CWMath.h"
#include "CNNCfg.h"
#include "CNNTSP.h"

CNNPopulation CNNTSP::s_oPopulation;
int CNNTSP::s_iGeneration = 0;
double CNNTSP::s_fUsedTimeInMS = 0.0f;

bool CNNTSP::Update( double fDeltaTimeMS )
{
	s_fUsedTimeInMS += fDeltaTimeMS;

	s_oPopulation.Update();

	if( s_fUsedTimeInMS >= NNMS_GAMECFG->m_fPassedTimeInMSForNextGeneration )
	{
		s_oPopulation.Epoch();
		s_iGeneration++;
		s_fUsedTimeInMS = 0.0f;
		return true;
	}

	return false;
}
