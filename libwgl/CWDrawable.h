#ifndef __CDRAWABLE_H_1231823718231263
#define __CDRAWABLE_H_1231823718231263

class CWDraw;
class CWCamera;
class CWFont;

class CWDrawable
{
public:
	static void SetDraw( CWDraw * pDraw ) { s_pDraw = pDraw; }
	static void SetFont( CWFont * pFont ) { s_pFont = pFont; }
	static void SetCamera( CWCamera * pCamera ) { s_pCamera = pCamera; }

	virtual void RenderOut() = 0;

protected:
	static CWDraw * s_pDraw;
	static CWFont * s_pFont;
	static CWCamera * s_pCamera;
};

#endif
