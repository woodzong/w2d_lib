#include "CWUnit.h"
#include "CWDraw.h"
#include "CWTimer.h"
#include "CWFont.h"
#include "CWMath.h"
#include "CWCamera.h"
#include "CWGlobalUtil.h"

using namespace std;

int CWUnit::Init( int iID, const char * szResBMPFile, const CWVector2<float> & pos, float fRadius, DWORD dwColor )
{
	m_iID = iID;
	m_fRadius = fRadius;
	m_pos = pos;
	m_dwColor = dwColor;

	int iRetCode = m_oBMPLoader.Init(szResBMPFile);
	assert( iRetCode == 0 );

	return 0;
}

void CWUnit::SetBMPFile( const char * szResBMPFile )
{
	int iRetCode = m_oBMPLoader.Init( szResBMPFile );
	assert( iRetCode == 0 );
}

void CWUnit::RenderOut( )
{
	CWVector2<int> oScreenPos;
	s_pCamera->ConvWorldPosToScreenPos( m_pos, &oScreenPos );
	
	for( int j=0; j < m_oBMPLoader.GetHeight(); j++ )
	{
		for( int i=0; i < m_oBMPLoader.GetWidth(); i++ )
		{
			DWORD dwColor = m_oBMPLoader.GetColorData( i, j );			
			if( dwColor != TRANSPARENT_COLOR )
			{
				if( m_dwColor != UNIT_DEFAULT_COLOR )
				{
					dwColor = m_dwColor;
				}

				s_pDraw->DrawPoint32Bit( oScreenPos.GetX( ) + i - m_oBMPLoader.GetWidth( ) / 2, oScreenPos.GetY( ) + j - m_oBMPLoader.GetHeight( ) / 2, dwColor );
			}
		}
	}

	//show unit id out
	if( W_CHECK_BIT( m_wShowFlag, EN_UNIT_SHOWCTL_BIT_ID ) == true )
	{
		s_pFont->ShowStringPure( s_pDraw, 2, oScreenPos.GetX( ) + 6, oScreenPos.GetY( ) - 2, "%d", m_iID );
	}	
}





