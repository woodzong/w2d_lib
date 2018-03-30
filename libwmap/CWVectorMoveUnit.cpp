#include "CWVectorMoveUnit.h"
#include "CWCamera.h"
#include "CWDraw.h"

#define RENDER_DIRECTION_RATIO (2.0f)

int CWVectorMoveUnit::Init( int iID, const char * szResBMPFile, const CWVector2<float> & pos, float fRadius, DWORD dwColor )
{	
	return CWUnit::Init( iID, szResBMPFile, pos, fRadius, dwColor );
}


void CWVectorMoveUnit::SetDirectPos( const CWVector2<float> & rvDirectionPos )
{
	CWVector2<float> vRet = rvDirectionPos - m_pos;

	float fLenght = m_vSpeed.Length();
	m_vSpeed.CopyFrom( vRet );
	m_vSpeed.SetMagnitudeOfVector( fLenght );
}

void CWVectorMoveUnit::WUpdate( float fDeltaTimeMS )
{
	if( m_vSpeed.IsZero() == true )
	{
		return;
	}

	m_pos.SelfPlus( m_vSpeed * m_fDebugSpeedFactor * ( fDeltaTimeMS / 1000 ) );
}

void CWVectorMoveUnit::RenderOut()
{
	CWUnit::RenderOut();

	//render direction out;
	CWVector2<int> oBeginPos;
	s_pCamera->ConvWorldPosToScreenPos( m_pos, &oBeginPos );

	CWVector2<float> directPos(m_pos);
	directPos.SelfPlus( m_vSpeed * RENDER_DIRECTION_RATIO );

	CWVector2<int> oEndPos;
	s_pCamera->ConvWorldPosToScreenPos( directPos, &oEndPos );

	s_pDraw->DrawLine32BitWithSize( oBeginPos.GetX( ), oBeginPos.GetY( ), oEndPos.GetX( ), oEndPos.GetY( ), WCOLOR_RED, EN_WDRAW_SIZE_1 );
	//s_pDraw->DrawPoint32BitWithSize( oBeginPos.GetX( ), oBeginPos.GetY( ), m_dwColor, EN_WDRAW_SIZE_1 );
	//s_pDraw->DrawPoint32BitWithSize( oEndPos.GetX( ), oEndPos.GetY( ), m_dwColor, EN_WDRAW_SIZE_1 );
}