#ifndef __GAMEPROCMAPTSP_H_123123asdasdasd
#define __GAMEPROCMAPTSP_H_123123asdasdasd

#include "WDefine.h"

#include "CGameProc.h"
#include "CWPathMoveUnit.h"

#define MAPTSP_MAX_CITY_NUM 20

#define MAPTSP_GEN_LEN MAPTSP_MAX_CITY_NUM
#define MAPTSP_POP_SIZE (MAPTSP_MAX_CITY_NUM*2)

#define MAPTSP_CROSSOVER_RATE_1000 750 // 750/1000
#define MAPTSP_MUTATION_RATE_1000 200 // 200/1000

class CMapTSPGen
{
public:
	CMapTSPGen();
	void RandInit();

	void SwapGen( int iGenA, int iGenB );
	void MuteEM();

	float m_fScore;
	float m_fFitScore;
	int m_aiGen[MAPTSP_GEN_LEN];
};

class CGameProcMapTSP :public CGameProc
{
public:
	virtual int GameInit_i( );
	virtual int GameRun_i( float fDeltaTime );
	
private:
	int Epoch( );
	void EvalOneGenScore( CMapTSPGen * pGen );
	int SelectParentIndex();
	void CrossoverPMX( CMapTSPGen * pGenParentA, CMapTSPGen * pGenParentB, CMapTSPGen * pGenBabyA, CMapTSPGen * pGenBabyB );

private:
	CWPathMoveUnit m_aCity[MAPTSP_MAX_CITY_NUM];

	int m_iGeneration;
	int m_iBestGenIndex;
	int m_iBadestGenIndex;
	float m_fTotalFitScore;
	float m_fBestScore;
	float m_fBadestScore;	
	CMapTSPGen m_aGen[MAPTSP_POP_SIZE];
};

#endif
