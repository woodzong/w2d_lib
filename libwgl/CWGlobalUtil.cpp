#include "CWGlobalUtil.h"
#include "CWDraw.h"
#include "CWFont.h"
#include "CWTimer.h"
#include "CWDrawable.h"
#include "CWCamera.h"
#include "CWMath.h"
#include <time.h>


CWDraw * CWGlobalUtil::s_pWDraw = NULL;
CWFont * CWGlobalUtil::s_pWFont = NULL;
CWFont * CWGlobalUtil::s_pWFontSmall = NULL;
CWTimer * CWGlobalUtil::s_pWTimer = NULL;
CWCamera * CWGlobalUtil::s_pCamera = NULL;
CWMath * CWGlobalUtil::s_pMath = NULL;


void CWGlobalUtil::Init()
{
	CWMath::InitRandSeed( (int)time( NULL ) );
	s_pMath = new CWMath();
	s_pWDraw = new CWDraw();
	s_pWFont = new CWFont();
	s_pWFontSmall = new CWFont();
	s_pWTimer = new CWTimer();
	s_pWTimer->Init();
	s_pCamera = new CWCamera();
	CWDrawable::SetCamera( s_pCamera );
	CWDrawable::SetDraw( s_pWDraw );
	CWDrawable::SetFont( s_pWFontSmall );	
}