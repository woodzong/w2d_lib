#ifndef __CWPATHMOVEUNIT_H_13123123123
#define __CWPATHMOVEUNIT_H_13123123123

#include "CWUnit.h"
#include "CWPath.h"

class CWPathMoveUnit :public CWUnit
{
public:
	CWPathMoveUnit()
	{
		m_fSpeed = 0.0f;
	}

	void SetSpeed( float fSpeed ) { m_fSpeed = fSpeed; }

	int Init( int iID, const char * szResBMPFile, const CWVector2<float> & pos, float fRadius, DWORD dwColor = UNIT_DEFAULT_COLOR );

	void WUpdate( float fDeltaTimeMS );

	virtual void RenderOut();

	void PathAddPoint( float fX, float Y );	

	void PathClear();

private:
	float m_fSpeed;
	CWPath m_oPath;
};

#endif