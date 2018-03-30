#ifndef _CFONT_H_18923718236123
#define _CFONT_H_18923718236123

#include "CWBMPLoader.h"

#define CHAR_NUM_IN_BMP 95
#define CHAR_ASCII_START 32
#define CHAR_MARGIN_DEFAULT 2


#define FONT_SMALL_RES_BMP "font.bmp"
#define FONT_SMALL_PER_CHAR_WIDTH 5
#define FONT_SMALL_LOAD_CHAR_MARGIN 1
#define FONT_SMALL_COLOR  0xFFFF00


#define FONT_RES_BMP "font2.bmp"
#define FONT_PER_CHAR_WIDTH 10
#define FONT_LOAD_CHAR_MARGIN 2
#define FONT_COLOR  0xFF0000

#define FONT_DEFAULT_START_Y 10


class CWDraw;

class CWFont
{
public:
	CWFont(){ m_bIsInited = false; m_bIsSetColor = false; m_iPreShowY = FONT_DEFAULT_START_Y; m_dwTransparentColor=0xFFFFFF; }

	int Init(const char * szFontBMPFile, int iPerCharWidth, int iLoadCharMargin);
	
	void ShowStringPure(CWDraw * pDraw,int iShowCharMargin, int iX, int iY, const char * szFormat, ... );

	void ShowString(CWDraw * pDraw, const char * szFormat, ... );

	bool IsInited(){ return m_bIsInited; }

	void SetColor( DWORD dwColor ){ m_dwColor = dwColor; m_bIsSetColor = true; };

	void SetTransparentColor( DWORD dwTransparentColor ){ m_dwTransparentColor = dwTransparentColor; }

	void Reset(){ m_iPreShowY = FONT_DEFAULT_START_Y; }

private:

	const DWORD * GetCharBuff( char chTarget );

	void DrawOneChar(CWDraw * pDraw, int iX, int iY, char chTarget);

private:
	DWORD m_dwColor;
	bool m_bIsSetColor;

	int m_iPreShowY;

	bool m_bIsInited;
	int m_iPerCharWidth;
	int m_iLoadCharMargin;	
	CBMPLoader m_oBMPLoader;
	DWORD m_dwTransparentColor;
	const DWORD * m_apCharBuff[CHAR_NUM_IN_BMP];
};


#endif

