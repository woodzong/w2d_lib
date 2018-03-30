#ifndef __GAMEPROCLANDINGTSP_H_123123asdasdasd
#define __GAMEPROCLANDINGTSP_H_123123asdasdasd

#include "WDefine.h"

#include "CGameProc.h"
#include "CWVectorMoveUnit.h"

typedef enum
{
	EN_LANDING_ACTION_NULL = 0, //无动作
	EN_LANDING_ACTION_LEFT = 1, //转左
	EN_LANDING_ACTION_RIGHT = 2, //转右
	EN_LANDING_ACTION_FIRE = 3,  //喷射
}EN_LANDING_ACTION;

#define PLANET_GALITY 1.63f //重力常数
//#define PLANET_GALITY 9.87f //重力常数
#define SCALING_FACTOR 500   //缩放因子
#define LANDING_FLOAT_TORLENCE 0.000001f //浮点计算误差
#define LANDING_TURN_SPEED 3.0f //一秒转3.0弧度


#define LADING_TSP_TICK_MS 16.6667f //快速计算时,每tick代表的时间值
#define LANDING_TSP_GEN_LEN 30 //基因长度
#define MAX_ACTION_DURATION_IN_MS 1000 //每个action 最大活动时间, 毫秒数
#define LANDING_TSP_POPULATION_NUM 500 //种群个体数

#define LANDING_TSP_NUM_ELITE 4 //每一代,把前几个精英保存下来, 必须为偶数
#define LANDING_TSP_MUTATION_RATE				0.02f
#define LANDING_TSP_CROSSOVER_RATE				0.7f

#define LANDING_TSP_SAFE_SPEED_M 0.25f


class CWLandingAction
{
public:
	CWLandingAction()
	{
		m_enCurAction = EN_LANDING_ACTION_NULL;
		m_iCurActionKeepTimeInMS = 0;
	}

	void RandAction()
	{
		m_enCurAction = ( EN_LANDING_ACTION )CWMathSim::RangeRandInt( EN_LANDING_ACTION_NULL, EN_LANDING_ACTION_FIRE );
	}

	void MuteDuration()
	{
		m_iCurActionKeepTimeInMS += (int)( CWMathSim::RandFloatInMinus1To1( ) * (float) MAX_ACTION_DURATION_IN_MS );
		if( m_iCurActionKeepTimeInMS <= 0 )
		{
			m_iCurActionKeepTimeInMS = 0;
		}
		if( m_iCurActionKeepTimeInMS >= MAX_ACTION_DURATION_IN_MS )
		{
			m_iCurActionKeepTimeInMS = MAX_ACTION_DURATION_IN_MS;
		}
	}

	void InitByRand()
	{
		m_enCurAction = ( EN_LANDING_ACTION )CWMathSim::RangeRandInt( EN_LANDING_ACTION_NULL, EN_LANDING_ACTION_FIRE );
		m_iCurActionKeepTimeInMS = CWMathSim::RangeRandInt( 1, MAX_ACTION_DURATION_IN_MS );
	}

	bool IsRunOut( int iActionRunnedTimeInMS ) const
	{
		return ( m_iCurActionKeepTimeInMS - iActionRunnedTimeInMS ) <= 0;
	}

	EN_LANDING_ACTION GetAction() const
	{
		return m_enCurAction;
	}

	int GetKeepTime( ) const
	{
		return m_iCurActionKeepTimeInMS;
	}

private:
	EN_LANDING_ACTION m_enCurAction;
	int m_iCurActionKeepTimeInMS;  //当前动作持续时间.
};


class CWLandingGenome
{
public:
	CWLandingGenome()
	{
		Init();
	}

	void Init()
	{
		m_iCurActionIndex = 0;
		m_fFitScore = 0;
		m_iLen = LANDING_TSP_GEN_LEN;
	}

	void InitByRand()
	{
		Init();
		for( int i = 0; i < m_iLen && i < LANDING_TSP_GEN_LEN; i++ )
		{
			m_aActionList[i].InitByRand();
		}
	}

	const char * GetDebugInfo()
	{
		static char s_szDebug[255];
		_snprintf_s( s_szDebug, sizeof( s_szDebug ), "Len:%d 0A:%d 0AT:%d 1A:%d 1AT:%d",
		     	     m_iLen, m_aActionList[0].GetAction( ), m_aActionList[0].GetKeepTime( ), m_aActionList[1].GetAction( ), m_aActionList[1].GetKeepTime( ) );
		return s_szDebug;
	}

	float GetFitScore() const { return m_fFitScore; }

	int GetCurActionIndex() const { return m_iCurActionIndex; }
	void NextActionIndex() { m_iCurActionIndex++; }

	int Len() const { return m_iLen; }

	const CWLandingAction * GetAction( int iIndex )
	{
		if( iIndex >= m_iLen || iIndex >= LANDING_TSP_GEN_LEN || iIndex < 0 )
		{
			return NULL;
		}

		return &( m_aActionList[iIndex] );
	}

	void SetAction( int iIndex, const CWLandingAction * pAction )
	{
		if( iIndex >= LANDING_TSP_GEN_LEN || iIndex < 0 )
		{
			return;
		}

		m_aActionList[iIndex] = *pAction;
	}

	void SetFitScore( float fFitScore ) 
	{ 
		m_fFitScore = fFitScore; 
	}

	void Mutate();

	void ResetStat()
	{
		m_fFitScore = 0.0f;
		m_iCurActionIndex = 0;
	}

private:
	float m_fFitScore;
	int m_iLen;
	int m_iCurActionIndex;
	CWLandingAction m_aActionList[LANDING_TSP_GEN_LEN];
};


class CWLandingShip :public CWVectorMoveUnit
{
public:
	CWLandingShip()
	{
		Reset();
	}

	void Reset()
	{
		m_vSpeed = CWVector2<float>( 0, 0 );
		m_pos = CWVector2<float>( 4000, 4000 );
		m_fInAirTime = 0.0f;
		m_bIsFiring = false;
		m_fForce = 3.5f;
		m_fMass = 1.0f;
		m_fRotation = WMATH_PI / 2;
		m_fTurnSpeed = LANDING_TURN_SPEED;
		m_vDirection.Set( 0.0f, 1.0f ); //朝上
		SetDebugFactor( SCALING_FACTOR );
	}

	float GetRotation() const { return m_fRotation; }

	const CWVector2<float> & GetDirection() const { return m_vDirection; }

	void RunOneAction( const CWLandingAction * pAction, float fDeltaTime );

	void TurnLeft( float fDeltaTime )
	{
		m_fRotation += m_fTurnSpeed * fDeltaTime / 1000;
		if( m_fRotation > 2 * WMATH_PI )
		{
			m_fRotation -= 2 * WMATH_PI;
		}
	}

	void TurnRight( float fDeltaTime )
	{
		m_fRotation -= m_fTurnSpeed * fDeltaTime / 1000;;
		if( m_fRotation < -2 * WMATH_PI )
		{
			m_fRotation += 2 * WMATH_PI;
		}
	}

	void SetIsFiring( bool bIsFiring )
	{
		m_bIsFiring = bIsFiring;
	}

	void SetNullAction()
	{
		SetIsFiring( false );
	}

	float GetInAirTime() const { return m_fInAirTime; }

	virtual void RenderOut();

	void Update( float fDeltaTime );

private:
	bool m_bIsFiring;
	float m_fForce; //喷射力
	float m_fMass; //质量

	float m_fTurnSpeed; //转向速度
	float m_fRotation;  //当前转向

	float m_fInAirTime; //浮空时间

	CWVector2<float> m_vDirection; //当前飞船朝向
};

class CGameProcLandingTSP :public CGameProc
{
public:
	virtual int GameInit_i();
	virtual int GameRun_i( float fDeltaTime );
	virtual void HandleNumKeyDown( uint8_t ucNumValue );

	bool IsShipLanded();     //是否已降落
	bool IsShipInSafeStat(); //是否在安全姿态

	void ManualCtrl( float fDeltaTime );
	void ReplayGenome( CWLandingGenome * pCurRuningGenome, float fDeltaTime );

	bool CaclGenomeScore( CWLandingGenome * pCurRuningGenome, float fTickDeltaTime );
	float CaclShipStatFitScore( );

	bool Epoch( float fDeltaTime );
	int SelectParentIndex();
	void CrossoverPMX( CWLandingGenome * pGenomeParentA, CWLandingGenome * pGenomeParentB, CWLandingGenome * pGenomeChildA, CWLandingGenome * pGenomeChildB );

private:
	CWVector2<float> m_vLandingPos; //合法的降落点
	float m_fLandingPosLength;      //合法的降落点长度
	int m_iSafeLandindIndex;

	float m_fReplayActionRunnedTime;

	int m_iGeneration;
	float m_fBestScore;
	float m_fWorstScore;
	float m_fTotalScore;
	float m_fAvgScore;
	CWLandingGenome m_oBestGenome;	
	CWLandingGenome m_aGenome[LANDING_TSP_POPULATION_NUM];
	CWLandingGenome * m_apGenome[LANDING_TSP_POPULATION_NUM];

	bool m_bIsInManualCtrl; //是否手工控制

	int m_ucKeyDownValue;
	CWLandingShip m_oShip;
};

#endif
