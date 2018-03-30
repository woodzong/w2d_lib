#include "CWCamera.h"

void CWCamera::ClampPosInCamera( CWVector2<float> * pPos )//把整个视窗世界, 变成一个循环的无限区域
{
	CWVector2<float> vWorldPosLeftDown;
	CWVector2<float> vWorldPosRightUp;
	GetLeftDownPointWorldPos( &vWorldPosLeftDown );
	GetRightUpPointWorldPos( &vWorldPosRightUp );

	if( pPos->GetX() > vWorldPosRightUp.GetX() )
	{
		pPos->SetX( vWorldPosLeftDown.GetX() );
	}
	
	if( pPos->GetX() < vWorldPosLeftDown.GetX() )
	{
		pPos->SetX( vWorldPosRightUp.GetX() );
	}

	if( pPos->GetY() > vWorldPosRightUp.GetY() )
	{
		pPos->SetY( vWorldPosLeftDown.GetY() );
	}

	if( pPos->GetY() < vWorldPosLeftDown.GetY() )
	{
		pPos->SetY( vWorldPosRightUp.GetY() );
	}
}

void CWCamera::GetLeftDownPointWorldPos( CWVector2<float> * pWorldPosOut )
{
	assert( m_enOrginPointType == EN_ORGINPOINT_TYPE_LEFT_BOTTOM );
	CWVector2<int> vTmp( 0, m_iWindowHeight );
	ConvScreenPosToWorldPos( vTmp, pWorldPosOut );
}

void CWCamera::GetRightUpPointWorldPos( CWVector2<float> * pWorldPosOut )
{
	assert( m_enOrginPointType == EN_ORGINPOINT_TYPE_LEFT_BOTTOM );
	CWVector2<int> vTmp( m_iWindowWidth,  0 );
	ConvScreenPosToWorldPos( vTmp, pWorldPosOut );
}


void CWCamera::ConvWorldPosToScreenPos( const CWVector2<float> & rWorldPos, CWVector2<int> * pScreenPosOut )
{
	int curScreenPosX = ( int )( ( rWorldPos.GetX() + ( m_iWindowWidth * m_fZoomX / 2 - m_oPos.GetX() ) ) / m_fZoomX );
	int curScreenPosY = ( int )( ( m_iWindowHeight * m_fZoomY - ( rWorldPos.GetY() + ( m_iWindowHeight * m_fZoomY / 2 - m_oPos.GetY() ) ) ) / m_fZoomY );

	pScreenPosOut->SetX( curScreenPosX );
	pScreenPosOut->SetY( curScreenPosY );
}

void CWCamera::ConvScreenPosToWorldPos( const CWVector2<int> & rScreenPos, CWVector2<float> * pWorldPosOut )
{
	float fCurWorldPosX = ( rScreenPos.GetX() - ( float( m_iWindowWidth ) / 2 - m_oPos.GetX() / m_fZoomX ) ) * m_fZoomX;
	float fCurWorldPosY = ( m_iWindowHeight - rScreenPos.GetY() - ( float( m_iWindowHeight ) / 2 - m_oPos.GetY() / m_fZoomY ) ) * m_fZoomY;

	pWorldPosOut->SetX( fCurWorldPosX );
	pWorldPosOut->SetY( fCurWorldPosY );
}



CWCamera::CWCamera( )
{
	m_iMapWidth = 1024 * 2;
	m_iMapHeight = 7680 * 2;

	m_fZoomX = 1.0;
	m_fZoomY = 1.0;

	m_iWindowWidth = 0;
	m_iWindowHeight = 0;

	SetOrginPointType( EN_ORGINPOINT_TYPE_LEFT_BOTTOM );
}

void CWCamera::SetOrginPointType( EN_ORGINPOINT_TYPE enOrginPointType )
{
	if( enOrginPointType == EN_ORGINPOINT_TYPE_LEFT_BOTTOM ) //世界坐标原点位于视窗左下角
	{
		m_enOrginPointType = enOrginPointType;
		m_oPos.SetX( m_iWindowWidth * m_fZoomX / 2 );
		m_oPos.SetY( m_iWindowHeight * m_fZoomY / 2 );
	}
	else if( enOrginPointType == EN_ORGINPOINT_TYPE_MIDDLE_CENTER ) //世界坐标原点位于视窗中心
	{
		m_enOrginPointType = enOrginPointType;
		m_oPos.SetX( 0 );
		m_oPos.SetY( 0 );
	}
}

void CWCamera::MoveRight( float fMoveSpeed )
{
	SetPos( m_oPos.GetX() + fMoveSpeed * m_fZoomX, m_oPos.GetY() );
}

void CWCamera::MoveLeft( float fMoveSpeed )
{
	SetPos( m_oPos.GetX() - fMoveSpeed * m_fZoomX, m_oPos.GetY() );
}

void CWCamera::MoveUp( float fMoveSpeed )
{
	SetPos( m_oPos.GetX(), m_oPos.GetY() + fMoveSpeed * m_fZoomY );
}

void CWCamera::MoveDown( float fMoveSpeed )
{
	SetPos( m_oPos.GetX(), m_oPos.GetY() - fMoveSpeed * m_fZoomY );
}

void CWCamera::SetPos( float fX, float fY )
{
	m_oPos.Set( fX, fY );
}

void CWCamera::InitWindowSize( int iWindowWidth, int iWindowHeight )
{
	m_iWindowWidth = iWindowWidth;
	m_iWindowHeight = iWindowHeight;
}

void CWCamera::SetZoom( float fZoom )
{
	m_fZoomX = fZoom;
	m_fZoomY = fZoom;
	SetOrginPointType( m_enOrginPointType );
}


