#include "CWDraw.h"

CWDraw::CWDraw():
m_pSurface(NULL),
m_iPitch32(0),
m_iPitch16(0),
m_pData(0),
m_bIsReleased(true),
m_iScreenWidth(0),
m_iScreenHeight(0){
}

CWDraw::~CWDraw(){
	Release();
}



void CWDraw::Init(){
	LONG lRetCode = 0;
	D3DSURFACE_DESC stSurfaceDesc;
	lRetCode = m_pSurface->GetDesc(&stSurfaceDesc);
	if(lRetCode != D3D_OK){
		//CWOOD_LOG_ERR_L(lRetCode,"m_pSurface->GetDesc Failed, pSurface:%d",m_pSurface);
		return;
	}

	D3DLOCKED_RECT stLockedRect;
	lRetCode = m_pSurface->LockRect(&stLockedRect,0,0);
	if(lRetCode != D3D_OK){
		//CWOOD_LOG_ERR_L(lRetCode,"m_pSurface->LockRect Failed, D3DERR_INVALIDCALL:%lu  D3DERR_WASSTILLDRAWING:%lu pSurface:%d",
		//	m_pSurface,D3DERR_INVALIDCALL,D3DERR_WASSTILLDRAWING);
		return;
	}
	m_iPitch32 = stLockedRect.Pitch >> 2;
	m_iPitch16 = stLockedRect.Pitch >> 1;
	m_pData = (DWORD *)stLockedRect.pBits;	
	m_iScreenWidth  = stSurfaceDesc.Width;
	m_iScreenHeight = stSurfaceDesc.Height;
	m_bIsReleased = false;
	return;
}

void CWDraw::Release(){
	if(m_bIsReleased){
		return;
	}
	LONG lRetCode = m_pSurface->UnlockRect();
	if( lRetCode != D3D_OK ){
		//CWOOD_LOG_ERR_L(lRetCode,"pSurface->UnlockRect Failed, pSurface:%d D3DERR_INVALIDCALL:%ld",m_pSurface,D3DERR_INVALIDCALL);
		return;
	}
	m_bIsReleased = true;
	return;
}



void CWDraw::DrawRectangleLine32Bit( int x0, int y0, int x1, int y1, DWORD dwColor )
{
	DrawLine32Bit( x0,  y0,  x1,  y0,  dwColor);
	DrawLine32Bit( x0,  y0,  x0,  y1,  dwColor);
	DrawLine32Bit( x1,  y0,  x1,  y1,  dwColor);
	DrawLine32Bit( x0,  y1,  x1,  y1,  dwColor);
	return;
}


void CWDraw::DrawRectangle32Bit( int x0, int y0, int x1, int y1, DWORD dwColor )
{
	if( x0 >= GCONF->m_iWindowsWidth )
	{
		x0 = GCONF->m_iWindowsWidth-1;
	}
	else if( x0 < 0 )
	{
		x0 = 0;
	}

	if( x1 >= GCONF->m_iWindowsWidth )
	{
		x1 = GCONF->m_iWindowsWidth-1;
	}
	else if( x1 < 0 )
	{
		x1 = 0;
	}


	if( y0 >= GCONF->m_iWindowsHeight )
	{
		y0 = GCONF->m_iWindowsHeight-1;
	}
	else if( y0 < 0 )
	{
		y0 = 0;
	}



	if( y1 >= GCONF->m_iWindowsHeight )
	{
		y1 = GCONF->m_iWindowsHeight-1;
	}
	else if( y1 < 0 )
	{
		y1 = 0;
	}	


	int iWidthTmp  = abs(x1 - x0);
	int iHeightTmp = abs(y1 - y0);
	if( iWidthTmp<=0 || iHeightTmp<=0 )
	{
		return;
	}

	/*
	for( int i=0; i<=iHeightTmp; i++ )
	{
		memcpy( m_pData+(y0+i)*m_iPitch32+x0, dwColor, iWidthTmp );
	}
	*/

	

	//DrawPoint32Bit(x1,y1,dwColor);
	int xStart = min( x0, x1 );
	int yStart = min( y0, y1 );

	for( int i=0; i<=iHeightTmp; i++ )
	{	
		for( int j=0; j<=iWidthTmp; j++ )
		{
			DrawPoint32Bit( xStart + j, yStart + i, dwColor );
		}
	}


	return;
}

#define CHECK_RETURN_VOID_VALID_POINT( x, y )\
if( x >= GCONF->m_iWindowsWidth || y >= GCONF->m_iWindowsHeight || x < 0 || y < 0 )\
{\
	return;\
}\


void CWDraw::DrawLine32Bit(int x0, int y0, // starting position 
						   int x1, int y1, // ending position
						   DWORD dwColor)     // color index
{
	//求屏幕范围内的交点
	//y = kx + b
	if( x1 != x0 )
	{
		if( x0 >= GCONF->m_iWindowsWidth ) //求y0;
		{
			int tmp = ( GCONF->m_iWindowsWidth - 1 );
			y0 = y1 - ( ( y1 - y0 ) * ( x1 - tmp ) ) / ( x1 - x0 );
			x0 = tmp;
		}

		if( x0 < 0 ) //求y0;
		{
			int tmp = 0;
			y0 = y1 - ( ( y1 - y0 ) * ( x1 - tmp ) ) / ( x1 - x0 ) ;
			x0 = tmp;
		}

		if( x1 >= GCONF->m_iWindowsWidth )
		{
			int tmp = ( GCONF->m_iWindowsWidth - 1 );
			y1 = y0 + ( ( y1 - y0 ) * ( tmp - x0 ) ) / ( x1 - x0 ) ;
			x1 = tmp;
		}

		if( x1 < 0 )
		{
			int tmp = 0;
			y1 = y0 + ( ( y1 - y0 ) * ( tmp - x0 ) ) / ( x1 - x0 );
			x1 = tmp;
		}
	}
	else
	{
		if( y0 >= GCONF->m_iWindowsHeight )
		{
			y0 = ( GCONF->m_iWindowsHeight - 1 );
		}

		if( y0 < 0 )
		{
			y0 = 0;
		}

		if( y1 >= GCONF->m_iWindowsHeight )
		{
			y1 = ( GCONF->m_iWindowsHeight - 1 );
		}

		if( y1 < 0 )
		{
			y1 = 0;
		}

		if( x0 >= GCONF->m_iWindowsWidth )
		{
			x0 = ( GCONF->m_iWindowsWidth - 1 );
		}

		if( x0 < 0 )
		{
			x0 = 0;
		}

		if( x1 >= GCONF->m_iWindowsWidth )
		{
			x1 = ( GCONF->m_iWindowsWidth - 1 );
		}

		if( x1 < 0 )
		{
			x1 = 0;
		}
	}




	if( y1 != y0 )
	{
		if( y0 >= GCONF->m_iWindowsHeight ) //求x0
		{
			int tmp = ( GCONF->m_iWindowsHeight - 1 );
			x0 = x1 - ( ( ( x1 - x0 ) * ( y1 - tmp ) ) / ( y1 - y0 ) );
			y0 = tmp;
		}

		if( y0 < 0 ) //求x0
		{
			int tmp = 0;
			x0 = x1 - ( ( ( x1 - x0 ) * ( y1 - tmp ) ) / ( y1 - y0 ) );
			y0 = tmp;
		}

		if( y1 >= GCONF->m_iWindowsHeight ) //求x0
		{
			int tmp = ( GCONF->m_iWindowsHeight - 1 );
			x1 = x0 + ( ( ( x1 - x0 ) * ( tmp - y0 ) ) / ( y1 - y0 ) );
			y1 = tmp;
		}

		if( y1 < 0 ) //求x0
		{
			int tmp = 0;
			x1 = x0 + ( ( ( x1 - x0 ) * ( tmp - y0 ) ) / ( y1 - y0 ) );
			y1 = tmp;
		}
	}
	else
	{
		if( x0 >= GCONF->m_iWindowsWidth )
		{
			x0 = ( GCONF->m_iWindowsWidth - 1 );
		}

		if( x0 < 0 )
		{
			x0 = 0;
		}

		if( x1 >= GCONF->m_iWindowsWidth )
		{
			x1 = ( GCONF->m_iWindowsWidth - 1 );
		}

		if( x1 < 0 )
		{
			x1 = 0;
		}

		if( y0 >= GCONF->m_iWindowsHeight )
		{
			y0 = ( GCONF->m_iWindowsHeight - 1 );
		}

		if( y0 < 0 )
		{
			y0 = 0;
		}

		if( y1 >= GCONF->m_iWindowsHeight )
		{
			y1 = ( GCONF->m_iWindowsHeight - 1 );
		}

		if( y1 < 0 )
		{
			y1 = 0;
		}
	}


	/*
	if( x0 >= GCONF->m_iWindowsWidth || y0 >= GCONF->m_iWindowsHeight || x0<0 || y0<0 )
	{
		return;
	}


	if( x1 >= GCONF->m_iWindowsWidth || y1 >= GCONF->m_iWindowsHeight || x1<0 || y1<0 )
	{
		return;
	}
	*/


	// this function draws a line from xo,yo to x1,y1 using differential error
	// terms (based on Bresenahams work)
	if( m_bIsReleased ){
		return;
	}

	int dx,             // difference in x's
		dy,             // difference in y's
		dx2,            // dx,dy * 2
		dy2, 
		x_inc,          // amount in pixel space to move during drawing
		y_inc,          // amount in pixel space to move during drawing
		error,          // the discriminant i.e. error i.e. decision variable
		index;          // used for looping

	// pre-compute first pixel address in video buffer based on 16bit data
	DWORD *pBuffer2 = m_pData + x0 + y0*m_iPitch32;

	DWORD * MIN_BUFFER = m_pData;
	DWORD * MAX_BUFFER = m_pData + (GCONF->m_iWindowsWidth - 1) + (GCONF->m_iWindowsHeight - 1) * m_iPitch32;

	// compute horizontal and vertical deltas
	dx = x1-x0;
	dy = y1-y0;

	// test which direction the line is going in i.e. slope angle
	if (dx>=0)
	{
		x_inc = 1;

	} // end if line is moving right
	else
	{
		x_inc = -1;
		dx    = -dx;  // need absolute value

	} // end else moving left

	// test y component of slope

	if (dy>=0)
	{
		y_inc = m_iPitch32;
	} // end if line is moving down
	else
	{
		y_inc = -m_iPitch32;
		dy    = -dy;  // need absolute value

	} // end else moving up

	// compute (dx,dy) * 2
	dx2 = dx << 1;
	dy2 = dy << 1;

	// now based on which delta is greater we can draw the line
	if (dx > dy)
	{
		// initialize error term
		error = dy2 - dx; 

		// draw the line
		for (index=0; index <= dx; index++)
		{			
			if( pBuffer2 >= MAX_BUFFER || pBuffer2 < MIN_BUFFER )
			{
				return;
			}
			
			// set the pixel
			*pBuffer2 = dwColor;

			// test if error has overflowed
			if (error >= 0) 
			{
				error-=dx2;

				// move to next line
				pBuffer2+=y_inc;

			} // end if error overflowed

			// adjust the error term
			error+=dy2;

			// move to the next pixel
			pBuffer2+=x_inc;

		} // end for

	} // end if |slope| <= 1
	else
	{
		// initialize error term
		error = dx2 - dy; 

		// draw the line
		for (index=0; index <= dy; index++)
		{
			if( pBuffer2 >= MAX_BUFFER || pBuffer2 < MIN_BUFFER )
			{
				return;
			}
		
			// set the pixel
			*pBuffer2 = dwColor;

			// test if error overflowed
			if (error >= 0)
			{
				error-=dy2;

				// move to next line
				pBuffer2+=x_inc;

			} // end if error overflowed

			// adjust the error term
			error+=dx2;

			// move to the next pixel
			pBuffer2+=y_inc;

		} // end for

	} // end else |slope| > 1

	// return success
	return;

} // end DrawLine32Bit


