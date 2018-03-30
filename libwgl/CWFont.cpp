#include <assert.h>
#include <stdio.h> 
#include "CWFont.h"
#include "CWDraw.h"
#include "WDefine.h"

int CWFont::Init( const char * szFontBMPFile, int iPerCharWidth, int iLoadCharMargin)
{
	m_iPerCharWidth   = iPerCharWidth;
	m_iLoadCharMargin = iLoadCharMargin;

	int iRetCode = m_oBMPLoader.Init(szFontBMPFile);
	assert(iRetCode == 0);

	for( int i =0; i<CHAR_NUM_IN_BMP; i++)
	{
		m_apCharBuff[i] = m_oBMPLoader.GetData( i*(m_iPerCharWidth + m_iLoadCharMargin), 0 );
	}

	m_bIsInited = true;

	return 0;
}


const DWORD * CWFont::GetCharBuff( char chTarget )
{
	if(chTarget >=CHAR_ASCII_START && chTarget <= (CHAR_ASCII_START+CHAR_NUM_IN_BMP) ) //asc II 
	{
		return m_apCharBuff[chTarget - CHAR_ASCII_START + 1];
	}
	else 
	{
		return m_apCharBuff[ 0 ];
	}
}

void CWFont::DrawOneChar( CWDraw * pDraw, int iX, int iY, char chTarget )
{
	const DWORD * pCharBuff = GetCharBuff( chTarget );
	for( int j=0; j<m_oBMPLoader.GetHeight(); j++ )
	{
		for( int i = 0; i<m_iPerCharWidth; i++ )
		{
			DWORD dwColor = *(pCharBuff + i + j*m_oBMPLoader.GetWidth());
			if( dwColor != m_dwTransparentColor )
			{
				if( m_bIsSetColor == true )
				{
					dwColor = m_dwColor;					
				}
				pDraw->DrawPoint32Bit( iX + i, iY + j, dwColor );
			}
		}
	}
}

void CWFont::ShowString( CWDraw * pDraw, const char * szFormat, ... )
{
	int iX = 10;
	int iY = m_iPreShowY;
	int iShowCharMargin = 1;
	
	assert( pDraw != NULL );

	va_list ap;	
	va_start(ap, szFormat);
	char szBuff[1024] = {0};
	W_SNPRINTF( szBuff, sizeof( szBuff ), szFormat, ap );
	va_end(ap);

	int iStrLen = (int)strlen(szBuff);
	for( int i=0; i<iStrLen; i++ )
	{
		int iTmpX = iX + i*(m_iPerCharWidth+iShowCharMargin);
		DrawOneChar( pDraw,iTmpX, iY, szBuff[i] );		
	}

	m_iPreShowY += (m_oBMPLoader.GetHeight() + m_oBMPLoader.GetHeight()/2);	
}

void CWFont::ShowStringPure( CWDraw * pDraw, int iShowCharMargin, int iX, int iY, const char * szFormat, ... )
{
	assert( pDraw != NULL );

	va_list ap;	
	va_start(ap, szFormat);
	char szBuff[1024] = {0};
	W_SNPRINTF( szBuff, sizeof(szBuff),szFormat, ap );
	va_end(ap);

	int iStrLen = (int)strlen(szBuff);
	for( int i=0; i<iStrLen; i++ )
	{
		int iTmpX = iX + i*(m_iPerCharWidth+iShowCharMargin);
		DrawOneChar( pDraw,iTmpX, iY, szBuff[i] );		
	}
}

