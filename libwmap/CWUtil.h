#ifndef __CWUTIL_H_1231823718231263
#define __CWUTIL_H_1231823718231263

#include "CWVector2.h"
#include "WDefine.h"
#include "WMapDefine.h"

#define WDRAW_STR( format, ...)\
	WGFONT->ShowString( WGDRAW, format, __VA_ARGS__ )

class CWUtil
{
public:
	static void DrawLineInWorld( const CWVector2<float>& vWorldPosA, const CWVector2<float>& vWorldPosB, DWORD dwColor, EN_WDRAW_SIZE enDrawSize );
	static void DrawPointDebug( const CWVector2<float>& vWorldPos, DWORD dwColor = WCOLOR_RED, EN_WDRAW_SIZE enDrawSize = EN_WDRAW_SIZE_1);
};

#endif
