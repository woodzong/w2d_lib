#include "CWPathMoveUnit.h"

int CWPathMoveUnit::Init( int iID, const char * szResBMPFile, const CWVector2<float> & pos, float fRadius, DWORD dwColor )
{	
	m_oPath.SetStartPos( pos );
	return CWUnit::Init( iID, szResBMPFile, pos, fRadius, dwColor );
}

void CWPathMoveUnit::WUpdate( float fDeltaTimeMS )
{
	if( m_fSpeed == 0.0f )
	{
		return;
	}

	float fMoveDis = m_fSpeed * fDeltaTimeMS / 1000;
	m_pos = m_oPath.MoveDis( m_pos, fMoveDis );
	m_oPath.SetStartPos( m_pos );
}

void CWPathMoveUnit::PathClear()
{
	m_oPath.Clear();
}

void CWPathMoveUnit::PathAddPoint( float fX, float fY )
{
	m_oPath.AddPoint( CWVector2<float>( fX, fY ) );
}

void CWPathMoveUnit::RenderOut()
{
	m_oPath.RenderOut( );
	CWUnit::RenderOut( );
}