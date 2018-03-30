#include "CWUnit.h"
#include "CWDraw.h"
#include "CWTimer.h"
#include "CWFont.h"
#include "CWMath.h"
#include "CWCamera.h"
#include "CWGlobalUtil.h"
#include "CWBoid.h"
#include "CWObstacle.h"

#define BOID_KEEP_DISTANCE_URGENCY 1.0f
#define BOID_MATCH_HEADING_URGENCY 1.0f
#define BOID_STEER_CENTER_URGENCY  1.0f
#define BOID_CURSING_URGENCY       1.0f
#define BOID_AVOID_OBSTACLE_URGENCY 1.0f

int CWBoid::Init( int iID, const char * szResBMPFile, const CWVector2<float> & pos, float fRadius, DWORD dwColor, float fSearchFriendRadius )
{
	ClearFriends();
	m_fPerceptionRange = fSearchFriendRadius;
	m_fSeparationDist = fRadius * 3.0f;

	return CWVectorMoveUnit::Init( iID, szResBMPFile, pos, fRadius, dwColor );
}

void CWBoid::RenderOut()
{
	CWVectorMoveUnit::RenderOut();
}


void CWBoid::WUpdate( float fDeltaTimeMS )
{
	m_vSpeed = m_vVecNew;
	CWVectorMoveUnit::WUpdate( fDeltaTimeMS );
}

void CWBoid::WAICacl( CWBoid * ( *paBoidsInWorld )[MAX_BOIDS_NUM_IN_WORLD], int iBoidsNum )
{
	ScanFriends( paBoidsInWorld, iBoidsNum );
	ScanObstacles( );

	m_vVecNew = m_vSpeed;

	if( m_iFriendsNum > 0 )
	{
		CWVector2<float> vFlockChange;
		vFlockChange += FlockAvoidObstacle( );
		vFlockChange += FlockKeepDistance();
		vFlockChange += FlockMatchHeading();
		vFlockChange += FlockSteerToCenter();
		vFlockChange += FlockCruising();

		if( vFlockChange.Length() >= BOID_MAX_FLOCK_VEC_CHANGE )
		{
			vFlockChange.SetMagnitudeOfVector( BOID_MAX_FLOCK_VEC_CHANGE );
		}

		m_vVecNew += vFlockChange;
	}
}


void CWBoid::ScanObstacles()
{
	ClearObstacles();

	for( int i = 0; i < CWObstacle::GetTotalNum(); i++ )
	{
		CWObstacle * pObstacle = CWObstacle::GetObstacle( i );

		float fSqrDistToOther = m_pos.SqrDistance( pObstacle->GetPosition( ) );
		if( fSqrDistToOther >= m_fPerceptionRange * m_fPerceptionRange ) //超出视野范围
		{
			continue;
		}

		m_apObstacle[m_iObstacleNum] = pObstacle;
		m_iObstacleNum++;

		if( m_fDistToNearstObstacle == 0.0f || m_fDistToNearstObstacle > fSqrDistToOther )
		{
			m_fDistToNearstObstacle = fSqrDistToOther;
			m_pNearestObstacle = pObstacle;
		}
	}

	m_fDistToNearstObstacle = ( float )CWMathSim::WSqrt( m_fDistToNearstObstacle );

	return;
}


void CWBoid::ScanFriends( CWBoid * ( *paBoidsInWorld )[MAX_BOIDS_NUM_IN_WORLD], int iBoidsNum )
{
	ClearFriends();

	for( int i = 0; i < iBoidsNum; i++ )
	{
		CWBoid * pBoid = ( *paBoidsInWorld )[i];
		if( m_iID == pBoid->GetID() )
		{
			continue;
		}

		float fSqrDistToOther = m_pos.SqrDistance( pBoid->GetPosition() );
		if( fSqrDistToOther >= m_fPerceptionRange * m_fPerceptionRange ) //超出视野范围
		{
			continue;
		}

		m_apFriend[m_iFriendsNum] = pBoid;
		m_iFriendsNum++;

		if( m_fDistToNearstFriend == 0.0f || m_fDistToNearstFriend > fSqrDistToOther )
		{
			m_fDistToNearstFriend = fSqrDistToOther;
			m_pNearestFriend = pBoid;
		}
	}

	m_fDistToNearstFriend = ( float )CWMathSim::WSqrt( m_fDistToNearstFriend );

	return;
}



CWVector2<float> CWBoid::FlockCruising()
{
	CWVector2<float> vecChange = m_vSpeed;

	float fDiff = ( m_vSpeed.Length() - m_fDesiredSpeed ) / m_fMaxSpeed;
	float fUrgency = ( float )fabs( fDiff ) * BOID_CURSING_URGENCY;

	//加入扰动
	if( fUrgency < BOID_MIN_URGENCY ) { fUrgency = BOID_MIN_URGENCY; }
	if( fUrgency > BOID_MAX_URGENCY ) { fUrgency = BOID_MAX_URGENCY; }

	float fJitter = CWMathSim::Rand();
	if( fJitter < 0.45f )
	{
		vecChange.SetX( vecChange.GetX() + BOID_MIN_URGENCY * WMATH_SIGN( fDiff ) );
	}
	else
	{
		vecChange.SetY( vecChange.GetY() + BOID_MIN_URGENCY * WMATH_SIGN( fDiff ) );
	}

	vecChange.SetMagnitudeOfVector( fUrgency * ( fDiff > 0 ? -1 : 1 ) );

	return vecChange;
}

CWVector2<float> CWBoid::FlockMatchHeading()
{
	if( m_pNearestFriend == NULL )
	{
		return CWVector2<float>();
	}

	CWVector2<float> vecChange = m_pNearestFriend->GetSpeed();
	vecChange.SetMagnitudeOfVector( BOID_MIN_URGENCY * BOID_MATCH_HEADING_URGENCY );

	return vecChange;
}


CWVector2<float> CWBoid::FlockSteerToCenter()
{
	if( m_iFriendsNum == 0 )
	{
		return CWVector2<float>();
	}

	CWVector2<float> vecCenter;
	CWVector2<float> vecChange;

	for( int i = 0; i < m_iFriendsNum; i++ )
	{
		if( m_apFriend[i] != NULL )
		{
			vecCenter += m_apFriend[i]->GetPosition();
		}
	}

	/*
	vecCenter += m_pos; //再加上自己
	vecCenter /= ( float )(m_iFriendsNum + 1 );
	*/
	vecCenter /= ( float )m_iFriendsNum;


	//if debug, show flock center
	CWVector2<int> oScreenPos;
	s_pCamera->ConvWorldPosToScreenPos( vecCenter, &oScreenPos );
	s_pDraw->DrawPoint32BitWithSize( oScreenPos.GetX(), oScreenPos.GetY(), WCOLOR_DEEPGREEN, EN_WDRAW_SIZE_3 );

	vecChange = vecCenter - m_pos;
	vecChange.SetMagnitudeOfVector( BOID_MIN_URGENCY * BOID_STEER_CENTER_URGENCY );
	return vecChange;
}



CWVector2<float> CWBoid::FlockAvoidObstacle()
{
	if( m_pNearestObstacle == NULL || m_fDistToNearstObstacle == 0.0f )
	{
		return CWVector2<float>();
	}


	float AVOID_DIST = m_fSeparationDist * 3;
	if( m_fDistToNearstObstacle >= AVOID_DIST )
	{
		return CWVector2<float>();
	}

	//float fRatio = m_fDistToNearstFriend / AVOID_DIST;
	float fRatio = BOID_MAX_URGENCY;
	//if( fRatio < BOID_MIN_URGENCY ) { fRatio = BOID_MIN_URGENCY; }
	//if( fRatio > BOID_MAX_URGENCY ) { fRatio = BOID_MAX_URGENCY; }

	CWVector2<float> vecChange = m_pos - m_pNearestObstacle->GetPosition();
	vecChange.SetMagnitudeOfVector( fRatio * BOID_AVOID_OBSTACLE_URGENCY );

	return vecChange;
}


CWVector2<float> CWBoid::FlockKeepDistance()
{
	if( m_pNearestFriend == NULL )
	{
		return CWVector2<float>();
	}

	float fRatio = m_fDistToNearstFriend / m_fSeparationDist;
	CWVector2<float> vecChange = m_pNearestFriend->GetPosition() - m_pos;

	if( fRatio < BOID_MIN_URGENCY ) { fRatio = BOID_MIN_URGENCY; }
	if( fRatio > BOID_MAX_URGENCY ) { fRatio = BOID_MAX_URGENCY; }

	if( m_fDistToNearstFriend < m_fSeparationDist ) // 远离
	{
		vecChange.SetMagnitudeOfVector( -fRatio * BOID_KEEP_DISTANCE_URGENCY );
	}
	else if( m_fDistToNearstFriend > m_fSeparationDist )
	{
		vecChange.SetMagnitudeOfVector( fRatio * BOID_KEEP_DISTANCE_URGENCY );
	}
	else //equal, do nothing;
	{
		vecChange.SetMagnitudeOfVector( 0.0f );
	}

	return vecChange;
}




