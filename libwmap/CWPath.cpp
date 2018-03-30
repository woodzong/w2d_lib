#include "CWPath.h"
#include "CWCamera.h"
#include "CWDraw.h"
#include "CWMath.h"

const CWVector2<float> CWPath::MoveDis( const CWVector2<float> & rStartPos, float fMoveDis )
{
	if( m_iNum == 0 )
	{
		return rStartPos;
	}

	CWVector2<float> oCurPos;
	oCurPos.CopyFrom( rStartPos );

	float fLeftDis = fMoveDis;
	int iCurPointIndex = 0;
	do 
	{
		float fStartX = oCurPos.GetX( );
		float fStartY = oCurPos.GetY( );

		float fNextX = m_aPath[iCurPointIndex].GetX( );
		float fNextY = m_aPath[iCurPointIndex].GetY( );

		float fOutX = 0.0f;
		float fOutY = 0.0f;

		fLeftDis = CWMathSim::CaclPointByMoveDisInLine( fStartX, fStartY, fNextX, fNextY, fLeftDis, fOutX, fOutY );
		oCurPos.Set( fOutX, fOutY );

		if( fLeftDis > 0.0f )
		{
			iCurPointIndex++;
		}
	}
	while( iCurPointIndex < m_iNum && fLeftDis > 0.0f );

	if( iCurPointIndex > 0 )
	{
		RemovePoint( iCurPointIndex );
	}

	return oCurPos;
}

int CWPath::AddPoint( const CWVector2<float> & rPos )
{
	if( m_iNum >= MAX_POINT_NUM_IN_ONE_PATH )
	{
		return -1;
	}

	m_aPath[m_iNum].CopyFrom( rPos );
	m_iNum++;
	return 0;
}

void CWPath::Clear()
{
	m_iNum = 0;
}

int CWPath::RemovePoint( int iRemoveNum )
{
	if( iRemoveNum > m_iNum || iRemoveNum <= 0 || m_iNum <= 0 )
	{
		return -1;
	}

	if( iRemoveNum == m_iNum )
	{
		m_iNum = 0;
		return 0;
	}
	
	//要优化为环的实现, 不要直接用memove, 用索引移动来实现.
	memmove( m_aPath, m_aPath + iRemoveNum, ( m_iNum - iRemoveNum ) * sizeof( m_aPath[0] ) );
	m_iNum = m_iNum - iRemoveNum;
	
	return 0;
}

void CWPath::RenderOut( )
{
	if( m_iNum <= 0 )
	{
		return;
	}

	CWVector2<int> oCurScreenPos;
	CWVector2<int> oLastScreenPos;

	s_pCamera->ConvWorldPosToScreenPos( m_oStartPos, &oLastScreenPos );

	for( int i = 0; i < m_iNum; i++ )
	{
		const CWVector2<float> & rPosition = m_aPath[i];
		s_pCamera->ConvWorldPosToScreenPos( rPosition, &oCurScreenPos );
		
		s_pDraw->DrawLine32BitWithSize( oLastScreenPos.GetX( ), oLastScreenPos.GetY( ), oCurScreenPos.GetX( ), oCurScreenPos.GetY( ), m_dwLineColor, m_enLineSize );
		s_pDraw->DrawPoint32BitWithSize( oLastScreenPos.GetX(), oLastScreenPos.GetY(), m_dwPointColor, m_enPointSize );

		if( i == m_iNum - 1 )
		{
			s_pDraw->DrawPoint32BitWithSize( oCurScreenPos.GetX( ), oCurScreenPos.GetY( ), m_dwPointColor, m_enPointSize );
		}

		oLastScreenPos.CopyFrom( oCurScreenPos );
	}
}



