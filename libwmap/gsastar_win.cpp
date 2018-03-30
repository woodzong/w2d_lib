#include "gsastar.h"



void DrawGridBlock( int iGridX, int iGridY, int iVal )
{

#if 0
	if( iVal == 0 )
	{
		return;
	}

	if( iVal & STATIC_BLOCK )
	{
		glColor4f( 0.5, 0.5, 0.5, 1.0f );
	}
	else if( iVal & UNIT_A_BLOCK || iVal & UNIT_G_BLOCK )
	{
		glColor4f( 0.8, 1.0, 0.8, 0.5f );
	}
	else if( iVal != 0 )
	{
		glColor4f( 0.8, 0.8, 1.0, 0.5f );
	}

	float fAX = iGridX * testPerGridPixels;
	float fAY = iGridY * testPerGridPixels;

	float fBX = ( iGridX + 1 ) * testPerGridPixels - 1;
	float fBY = ( iGridY + 1 ) * testPerGridPixels - 1;

	glBegin( GL_QUADS );
	glVertex2f( fAX, fAY );
	glVertex2f( fAX, fBY );
	glVertex2f( fBX, fBY );
	glVertex2f( fBX, fAY );
	glEnd();
#endif
}

