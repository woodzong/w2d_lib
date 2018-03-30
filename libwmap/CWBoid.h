#ifndef __CWBOID_H_1231823718231263
#define __CWBOID_H_1231823718231263

//flock members
#include "WMapDefine.h"
#include "CWVectorMoveUnit.h"
#include "CWMath.h"
#include "CWObstacle.h"

#define MAX_FRIENDS_NUM 1000
#define MAX_BOIDS_NUM_IN_WORLD 1000

#define BOID_MAX_FLOCK_VEC_CHANGE 10.0f
#define BOID_MIN_URGENCY 0.05f
#define BOID_MAX_URGENCY 0.3f //org is 0.1f

class CWBoid :public CWVectorMoveUnit
{
public:
	CWBoid( )
	{
		ClearFriends();
		ClearObstacles();
		m_fPerceptionRange = 0.0f;
		m_fMaxSpeed = 0.1f;//
		m_fDesiredSpeed = 0.1f;
		m_fSeparationDist = 0.0f;
	}

	int Init( int iID, const char * szResBMPFile, const CWVector2<float> & pos, float fRadius, DWORD dwColor, float fSearchFriendRadius );
	void InitSpeedCfg( float fMaxSpeed, float fDesiredSpeed ) { m_fMaxSpeed = fMaxSpeed; m_fDesiredSpeed = fDesiredSpeed; }

	void WAICacl( CWBoid * ( *paBoidsInWorld )[MAX_BOIDS_NUM_IN_WORLD], int iBoidsNum );

	void WUpdate( float fDeltaTimeMS );

	void SetSpeedMagnitude( float fSpeedMagnitude )
	{
		m_vSpeed.SetMagnitudeOfVector( fSpeedMagnitude );
	}

	virtual void RenderOut();

private:
	void ScanFriends( CWBoid * ( *paBoidsInWorld )[MAX_BOIDS_NUM_IN_WORLD], int iBoidsNum );
	void ClearFriends()
	{
		m_iFriendsNum = 0;
		m_pNearestFriend = NULL;
		m_fDistToNearstFriend = 0.0f;
	}

	void ScanObstacles();
	void ClearObstacles()
	{
		m_iObstacleNum = 0;
		m_pNearestObstacle = NULL;
		m_fDistToNearstObstacle = 0.0f;
	}

private:
	CWVector2<float> FlockAvoidObstacle( );  //避开障碍物

	CWVector2<float> FlockKeepDistance( );  //分离-靠近 保持特定距离
	CWVector2<float> FlockMatchHeading( );  //对齐
	CWVector2<float> FlockSteerToCenter( ); //聚合
	CWVector2<float> FlockCruising( );      //巡航 和群体保持相对速度

protected:
	int m_iFriendsNum;
	CWBoid * m_apFriend[MAX_FRIENDS_NUM];
	CWBoid * m_pNearestFriend;
	float m_fDistToNearstFriend;

	int m_iObstacleNum;
	CWObstacle * m_apObstacle[MAX_OBSTACLE_NUM_IN_WORLD];
	CWObstacle * m_pNearestObstacle;
	float m_fDistToNearstObstacle;

	float m_fPerceptionRange;
	float m_fSeparationDist;

	CWVector2<float> m_vVecNew;

	float m_fMaxSpeed;
	float m_fDesiredSpeed;
};

#endif
