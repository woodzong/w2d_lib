#ifndef __CWDRAW_H_12314asdfas123123
#define __CWDRAW_H_12314asdfas123123

#include <assert.h>
#include <windows.h>
#include <d3dx9.h>
#include "CWGlobalConf.h"
#include "WDefine.h"

class CWDraw
{
public:
	CWDraw();
	~CWDraw();
	void Init();
	void Release();

	void DrawCache( DWORD * pCache, int iCacheSize, int iCacheStart = 0 )
	{
		memcpy( m_pData + iCacheStart, pCache, iCacheSize );
	}

	int SaveCache( DWORD * pCache, int iCacheSize )
	{
		int iTmpCacheSize = m_iPitch32 * m_iScreenHeight * sizeof( DWORD );
		assert( iTmpCacheSize <= iCacheSize );
		memcpy( pCache, m_pData, iTmpCacheSize );
		return iTmpCacheSize;
	}

	void DrawPoint32BitWithSize( int x, int y, DWORD dwColor, EN_WDRAW_SIZE enDrawSize )
	{
		switch( enDrawSize )
		{
			case EN_WDRAW_SIZE_4:
				DrawPoint32Bit( x - 1, y + 2, dwColor ); DrawPoint32Bit( x, y + 2, dwColor ); DrawPoint32Bit( x + 1, y + 2, dwColor ); DrawPoint32Bit( x + 2, y + 2, dwColor );
				DrawPoint32Bit( x - 1, y + 1, dwColor ); DrawPoint32Bit( x, y + 1, dwColor ); DrawPoint32Bit( x + 1, y + 1, dwColor ); DrawPoint32Bit( x + 2, y + 1, dwColor );
				DrawPoint32Bit( x - 1, y, dwColor );     DrawPoint32Bit( x, y, dwColor );     DrawPoint32Bit( x + 1, y, dwColor );     DrawPoint32Bit( x + 2, y, dwColor );
				DrawPoint32Bit( x - 1, y - 1, dwColor ); DrawPoint32Bit( x, y - 1, dwColor ); DrawPoint32Bit( x + 1, y - 1, dwColor ); DrawPoint32Bit( x + 2, y - 1, dwColor );
				break;

			case EN_WDRAW_SIZE_3:
				DrawPoint32Bit( x - 1, y + 1, dwColor ); DrawPoint32Bit( x, y + 1, dwColor ); DrawPoint32Bit( x + 1, y + 1, dwColor );
				DrawPoint32Bit( x - 1, y, dwColor ); DrawPoint32Bit( x, y, dwColor ); DrawPoint32Bit( x + 1, y, dwColor );
				DrawPoint32Bit( x - 1, y - 1, dwColor ); DrawPoint32Bit( x, y - 1, dwColor ); DrawPoint32Bit( x + 1, y - 1, dwColor );
				break;

			case EN_WDRAW_SIZE_2:
				DrawPoint32Bit( x, y + 1, dwColor ); DrawPoint32Bit( x + 1, y + 1, dwColor );
				DrawPoint32Bit( x, y, dwColor ); DrawPoint32Bit( x + 1, y, dwColor );
				break;

			case EN_WDRAW_SIZE_1:
				DrawPoint32Bit( x, y, dwColor );
				break;

			default:
				DrawPoint32Bit( x, y, dwColor );
				break;
		}

		return;
	}

	void DrawPoint32Bit( int x, int y, DWORD dwColor )
	{
		if( m_bIsReleased ) { return; }
		if( x >= GCONF->m_iWindowsWidth || y >= GCONF->m_iWindowsHeight || x < 0 || y < 0 )
		{
			return;
		}

		m_pData[y*m_iPitch32 + x] = dwColor;
	}

	void DrawMouse( int iMouseScreenX, int iMouseScreenY, DWORD dwColor = 0x0000FF )
	{
		DrawPoint32Bit( iMouseScreenX, iMouseScreenY, dwColor );

		DrawPoint32Bit( iMouseScreenX + 1, iMouseScreenY, dwColor );
		DrawPoint32Bit( iMouseScreenX, iMouseScreenY + 1, dwColor );

		DrawPoint32Bit( iMouseScreenX + 2, iMouseScreenY, dwColor );
		DrawPoint32Bit( iMouseScreenX, iMouseScreenY + 2, dwColor );

		DrawPoint32Bit( iMouseScreenX + 3, iMouseScreenY, dwColor );
		DrawPoint32Bit( iMouseScreenX, iMouseScreenY + 3, dwColor );

		DrawPoint32Bit( iMouseScreenX + 4, iMouseScreenY, dwColor );
		DrawPoint32Bit( iMouseScreenX, iMouseScreenY + 4, dwColor );

		DrawPoint32Bit( iMouseScreenX + 5, iMouseScreenY, dwColor );
		DrawPoint32Bit( iMouseScreenX, iMouseScreenY + 5, dwColor );

		DrawPoint32Bit( iMouseScreenX + 6, iMouseScreenY, dwColor );
		DrawPoint32Bit( iMouseScreenX, iMouseScreenY + 6, dwColor );

		DrawPoint32Bit( iMouseScreenX + 1, iMouseScreenY + 1, dwColor );
		DrawPoint32Bit( iMouseScreenX + 2, iMouseScreenY + 2, dwColor );
		DrawPoint32Bit( iMouseScreenX + 3, iMouseScreenY + 3, dwColor );
		DrawPoint32Bit( iMouseScreenX + 4, iMouseScreenY + 4, dwColor );
		DrawPoint32Bit( iMouseScreenX + 5, iMouseScreenY + 5, dwColor );
		DrawPoint32Bit( iMouseScreenX + 6, iMouseScreenY + 6, dwColor );
	}

	void UpdateSurface( IDirect3DSurface9* _pSurface ) { m_pSurface = _pSurface; Init(); }

	void DrawRectangle32Bit( int x0, int y0, int x1, int y1, DWORD dwColor );

	void DrawRectangleLine32Bit( int x0, int y0, int x1, int y1, DWORD dwColor );

	void DrawLine32Bit( int x0, int y0, int x1, int y1, DWORD dwColor );

	void DrawLine32BitWithSize( int x0, int y0, int x1, int y1, DWORD dwColor, EN_WDRAW_SIZE enDrawSize )
	{
		if( abs( x1 - x0 ) > abs( y1 - y0 ) && ( y1 != y0 ) )
		{
			switch( enDrawSize )
			{
				case EN_WDRAW_SIZE_4:
					DrawLine32Bit( x0 + 2, y0, x1 + 2, y1, dwColor );
					DrawLine32Bit( x0 + 1, y0, x1 + 1, y1, dwColor );
					DrawLine32Bit( x0,     y0, x1,     y1, dwColor );
					DrawLine32Bit( x0 - 1, y0, x1 - 1, y1, dwColor );
					break;

				case EN_WDRAW_SIZE_3:
					DrawLine32Bit( x0 + 1, y0, x1 + 1, y1, dwColor );
					DrawLine32Bit( x0, y0, x1, y1, dwColor );
					DrawLine32Bit( x0 - 1, y0, x1 - 1, y1, dwColor );
					break;

				case EN_WDRAW_SIZE_2:
					DrawLine32Bit( x0 + 1, y0, x1 + 1, y1, dwColor );
					DrawLine32Bit( x0, y0, x1, y1, dwColor );
					break;

				case EN_WDRAW_SIZE_1:
					DrawLine32Bit( x0, y0, x1, y1, dwColor );
					break;

				default:
					DrawLine32Bit( x0, y0, x1, y1, dwColor );
					break;
			}
		}
		else
		{
			switch( enDrawSize )
			{
				case EN_WDRAW_SIZE_4:
					DrawLine32Bit( x0, y0 + 2, x1, y1 + 2, dwColor );
					DrawLine32Bit( x0, y0 + 1, x1, y1 + 1, dwColor );
					DrawLine32Bit( x0, y0, x1, y1, dwColor );
					DrawLine32Bit( x0, y0 - 1, x1, y1 - 1, dwColor );
					break;

				case EN_WDRAW_SIZE_3:
					DrawLine32Bit( x0, y0 + 1, x1, y1 + 1, dwColor );
					DrawLine32Bit( x0, y0, x1, y1, dwColor );
					DrawLine32Bit( x0, y0 - 1, x1, y1 - 1, dwColor );
					break;

				case EN_WDRAW_SIZE_2:
					DrawLine32Bit( x0, y0 + 1, x1, y1 + 1, dwColor );
					DrawLine32Bit( x0, y0, x1, y1, dwColor );
					break;

				case EN_WDRAW_SIZE_1:
					DrawLine32Bit( x0, y0, x1, y1, dwColor );
					break;

				default:
					DrawLine32Bit( x0, y0, x1, y1, dwColor );
					break;
			}
		}

		return;
	}

private:
	IDirect3DSurface9* m_pSurface;
	int m_iPitch32;
	int m_iPitch16;
	DWORD * m_pData;
	bool m_bIsReleased;
	int m_iScreenWidth;
	int m_iScreenHeight;
};


#endif