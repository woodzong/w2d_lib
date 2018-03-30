#ifndef __CNNTSP_H_123123asdasdasd
#define __CNNTSP_H_123123asdasdasd

#include "WDefine.h"
#include "CNeuron.h"
#include "CNNGenome.h"
#include "CNNPopulation.h"

class CNNTSP
{

public:
	static bool Update( double fDeltaTimeMS );

public:
	static double s_fUsedTimeInMS;
	static int s_iGeneration;
	static CNNPopulation s_oPopulation;
};

#endif
