#ifndef __GAMEPROCGRIDMAPEVOLUTION_H_123123asdasdasd
#define __GAMEPROCGRIDMAPEVOLUTION_H_123123asdasdasd

#include "WDefine.h"

#include "CGameProc.h"
#include "CWGridMap.h"

#include "CWPathMoveUnit.h"

#define GENOMES_NUM (70*2)  //这个值必须为偶数
#define GENOMES_LEN 70

typedef enum
{
	T_T_UP = 0,
	T_T_RIGHT = 1,
	T_T_DOWN = 2,
	T_T_LEFT = 3,
}T_T_DIRECT;

class CGameProcGridMapEvolution :public CGameProc
{
public:
	CGameProcGridMapEvolution( ) 
	{ 		
		memset( m_aGenomes, 0, sizeof( m_aGenomes ) );
		memset( m_aGenScores, 0, sizeof( m_aGenScores ) );
		m_iGeneration = 0;
		m_fBestScore = 0.0f;
		m_iBestGenIndex = 0;
		m_fTotalScore = 0.0f;
		m_bIsNeedEpoch = true;
	}

	virtual int GameInit_i( );
	virtual int GameRun_i( float fDeltaTime );

	virtual void HandleNumKeyDown( uint8_t ucNumValue );

private:
	void ResetMapData();
	float EvalPath( uint8_t * pucPath, int iPathLen, int * pOutLen );
	int Epoch();
	void MarkPathInMap( uint8_t * pucPath, int iPathLen );

private:
	int SelectParentIndex();
	void CrossOver( int iGenLen, uint8_t * pucParentGensA, uint8_t * pucParentGensB, uint8_t * pucBabyA, uint8_t * pucBabyB );
	void Mutate( int iGenLen, uint8_t * pucGens );

private:
	uint8_t m_aGenomes[GENOMES_NUM][GENOMES_LEN];
	int m_aGenLen[GENOMES_NUM];
	float m_aGenScores[GENOMES_NUM];

	bool m_bIsNeedEpoch;
	int m_iGeneration;
	float m_fBestScore;
	int m_iBestGenIndex;
	float m_fTotalScore;

	CWGridMap m_oGridMap;
};

#endif
