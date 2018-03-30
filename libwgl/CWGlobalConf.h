#ifndef CWGLOBAL_CONF_H_123819287asdasdasd
#define CWGLOBAL_CONF_H_123819287asdasdasd

#include <stdio.h>
#include "CSingletonT.h"

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#define CW_FPS_LIMIT 60
#define CW_FPS_WAITIME  ((float)(1000*(1.0f)/CW_FPS_LIMIT))
#define CW_BASE_RES_PATH "../res/"

#define CW_RES_FILE_PATH( file ) CW_BASE_RES_PATH file

class CWGlobalConf
{

public:
    CWGlobalConf()
    {
        m_iWindowsWidth  = 1024;
        m_iWindowsHeight = 768;
		m_iWindowsOffsetX = 500;
		m_iWindowsOffsetY = 150;
        _snprintf_s( m_szWindowsTitle,sizeof(m_szWindowsTitle),sizeof(m_szWindowsTitle),"%s %s %s","W2D Title",__DATE__,__TIME__);
        _snprintf_s( m_szWindowsClass,sizeof(m_szWindowsClass),sizeof(m_szWindowsClass),"%s %s %s","W2D ClassName",__DATE__,__TIME__);
    }

public:
    int m_iWindowsWidth;
    int m_iWindowsHeight;
	int m_iWindowsOffsetX;
	int m_iWindowsOffsetY;

    char m_szWindowsTitle[255];
    char m_szWindowsClass[255];
};



class CWGlobalFPS
{
public:
	CWGlobalFPS()
	{
		m_fPreRenderTime = 0;
		m_fFlames = 0;
		m_fFPS = 0;
	}

public:
	float m_fFPS;
	float m_fPreRenderTime;
	float m_fFlames;

};

class CWGlobalCtrl
{
public:
	CWGlobalCtrl()
	{
		m_iMouseScreenX = 0;
		m_iMouseScreenY = 0;

        m_bMouseLeftButtonIsClicked  = false;
        m_bMouseRightButtonIsClicked = false;

        m_bMouseLeftButtonIsReleased  = false;
        m_bMouseRightButtonIsReleased = false;

		m_iPreClickedBoardX = -1;
		m_iPreClickedBoardY = -1;
	}

public:
	LPDIRECTINPUT8       m_pDInput;
	LPDIRECTINPUTDEVICE8 m_pMouse;
	DIMOUSESTATE         m_stMouseState;

	int m_iMouseScreenX;
	int m_iMouseScreenY;

    bool m_bMouseLeftButtonIsClicked;
    bool m_bMouseRightButtonIsClicked;

    bool m_bMouseLeftButtonIsReleased;
    bool m_bMouseRightButtonIsReleased;

	int m_iPreClickedBoardX;
	int m_iPreClickedBoardY;
	bool m_bIsPreClickedChess;
};



typedef CSingletonT<CWGlobalConf> CWGlobalConfSingleton;
#define GCONF CWGlobalConfSingleton::Instance()


typedef CSingletonT<CWGlobalFPS> CWGlobalFPSSingleton;
#define GFPS CWGlobalFPSSingleton::Instance()

typedef CSingletonT<CWGlobalCtrl> CWGlobalCtrlSingleton;
#define GCTRL CWGlobalCtrlSingleton::Instance()

#endif