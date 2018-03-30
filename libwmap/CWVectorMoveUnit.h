#ifndef __CWVECTORMOVEUNIT_H_13123123123
#define __CWVECTORMOVEUNIT_H_13123123123

#include "CWUnit.h"
#include "CWPath.h"

class CWVectorMoveUnit :public CWUnit
{
public:
	CWVectorMoveUnit() : m_vSpeed( 0.0f, 0.0f )
	{
		m_fDebugSpeedFactor = 1.0f;
	}

	void SetDebugFactor( float fSpeedFactor ) { m_fDebugSpeedFactor = fSpeedFactor; }

	void SetDirectPos( const CWVector2<float> & rvDirectionPos );

	void SetSpeed( const CWVector2<float> & rvSpeed ) { m_vSpeed.CopyFrom( rvSpeed ); }

	void SetSpeed( float fSpeedX, float fSpeedY ) { m_vSpeed.Set( fSpeedX, fSpeedY ); }

	int Init( int iID, const char * szResBMPFile, const CWVector2<float> & pos, float fRadius, DWORD dwColor = UNIT_DEFAULT_COLOR );	

	void WUpdate( float fDeltaTimeMS );

	virtual void RenderOut();

	const CWVector2<float> & GetSpeed() const 
	{
		return m_vSpeed;
	}

protected:
	float m_fDebugSpeedFactor;
	CWVector2<float> m_vSpeed;
};

#endif