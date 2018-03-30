#ifndef __CWGLOBAL_UTIL_H_12314asdfas123123
#define __CWGLOBAL_UTIL_H_12314asdfas123123

class CWDraw;
class CWFont;
class CWTimer;
class CWCamera;
class CWMath;

class CWGlobalUtil
{
public:
	static void Init();

public:
	static CWDraw * s_pWDraw;
	static CWFont * s_pWFont;
	static CWFont * s_pWFontSmall;
	static CWTimer * s_pWTimer;
	static CWCamera * s_pCamera;
	static CWMath * s_pMath;
};

#define WGMATH ( CWGlobalUtil::s_pMath )
#define WGFONT ( CWGlobalUtil::s_pWFont )
#define WGFONT_SMALL ( CWGlobalUtil::s_pWFontSmall )
#define WGDRAW ( CWGlobalUtil::s_pWDraw )
#define WGTIMER ( CWGlobalUtil::s_pWTimer )
#define WGCAMERA ( CWGlobalUtil::s_pCamera )


#endif