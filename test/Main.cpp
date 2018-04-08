#define INITGUID  // make sure al the COM interfaces are available
#define WIN32_LEAN_AND_MEAN 

#include <windows.h>
#include <d3dx9.h>
#include "CWGlobalConf.h"
#include "CWoodLog.h"
#include "WMain.h"
#include "CGameProc.h"

#include "w3d_comm.h"

EN_WGKEY ConvWindowsKeyToWGKey( WPARAM wParam )
{
	switch( wParam )
	{
		case VK_UP:
			return EN_WGKEY_UP;
		case VK_DOWN:
			return EN_WGKEY_DOWN;
		case VK_LEFT:
			return EN_WGKEY_LEFT;
		case VK_RIGHT:
			return EN_WGKEY_RIGHT;
		case VK_NUMPAD0:
			return EN_WGKEY_NUM0;
		case VK_NUMPAD1:
			return EN_WGKEY_NUM1;
		case VK_NUMPAD2:
			return EN_WGKEY_NUM2;
		case VK_NUMPAD3:
			return EN_WGKEY_NUM3;
		case VK_NUMPAD4:
			return EN_WGKEY_NUM4;
		case VK_NUMPAD5:
			return EN_WGKEY_NUM5;
		case VK_NUMPAD6:
			return EN_WGKEY_NUM6;
		case VK_NUMPAD7:
			return EN_WGKEY_NUM7;
		case VK_NUMPAD8:
			return EN_WGKEY_NUM8;
		case VK_NUMPAD9:
			return EN_WGKEY_NUM9;
		case VK_F11:
			return EN_WGKEY_F11;
		case VK_F12:
			return EN_WGKEY_F12;
	}

	return EN_WGKEY_NULL;
}


LRESULT CALLBACK WindowProc( HWND hWnd, UINT stMsg, WPARAM wParam, LPARAM lParam )
{
	switch( stMsg )
	{
		case WM_PAINT:
		{
						 // simply validate the window 
						 //BeginPaint(hWnd,&ps);	 

						 // end painting
						 //EndPaint(hWnd,&ps);
						 // return success
						 return( 0 );
		}
			break;

		case WM_KEYDOWN://°´¼ü°´ÏÂ   
			WGAMEPROC->HandleKeyDown( ConvWindowsKeyToWGKey( wParam ) );
			break;

		case WM_DESTROY:
			PostQuitMessage( 0 );
			break;

		default:break;
	}
	return ( DefWindowProc( hWnd, stMsg, wParam, lParam ) );
}




int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int iCmdShow )
{

	int iRetCode = 0;

	iRetCode = CWOOD_OLOG->Init( "WoodFree", "C:\\CWLOG", true );
	if( iRetCode != 0 )
	{
		W_PRINT_ERROR_MSG_CHAR_INFO( iRetCode, "CWOOD_OLOG->Init Failed ErrMsg:%s", CWOOD_OLOG->GetLastErrMsg() );
		W_BOX_ALERT_CHAR_INFO( iRetCode, "CWOOD_OLOG->Init Failed ErrMsg:%s", CWOOD_OLOG->GetLastErrMsg() );
		return iRetCode;
	}

	CWOOD_LOG_DEBUG( "LibFuncTest:%d", W3D_Test() );
	CWOOD_LOG_DEBUG( " " );
	CWOOD_LOG_DEBUG( " ==================================== Compile Time:%s %s", __DATE__, __TIME__ );

	WNDCLASS stWinclass;	// this will hold the class we create
	HWND	 hWnd;		// generic window handle
	MSG		 stMsg;		// generic message

	// first fill in the window class stucture
	stWinclass.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	stWinclass.lpfnWndProc = WindowProc;
	stWinclass.cbClsExtra = 0;
	stWinclass.cbWndExtra = 0;
	stWinclass.hInstance = hInstance;
	stWinclass.hIcon = LoadIcon( NULL, IDI_APPLICATION );
	stWinclass.hCursor = LoadCursor( NULL, IDC_ARROW );
	stWinclass.hbrBackground = ( HBRUSH )GetStockObject( BLACK_BRUSH );
	stWinclass.lpszMenuName = NULL;
	stWinclass.lpszClassName = GCONF->m_szWindowsClass;

	// register the window class
	iRetCode = RegisterClass( &stWinclass );
	if( iRetCode == 0 )
	{ //0 means error. NULL
		CWOOD_LOG_ERR( iRetCode, "%s", "RegisterClass Failed!!" );
		return iRetCode;
	}

	// create the window, note the test to see if WINDOWED_APP is
	// true to select the appropriate window flags
	hWnd = CreateWindow( GCONF->m_szWindowsClass, // class
						 GCONF->m_szWindowsTitle,	 // title
						 WS_POPUP | WS_VISIBLE,
						 GCONF->m_iWindowsOffsetX,
						 GCONF->m_iWindowsOffsetY,	   // x,y
						 GCONF->m_iWindowsWidth,  // width
						 GCONF->m_iWindowsHeight, // height
						 NULL,	   // handle to parent 
						 NULL,	   // handle to menu
						 hInstance,// instance from outside.
						 NULL );

	if( hWnd == NULL )
	{
		iRetCode = -1;
		CWOOD_LOG_ERR( iRetCode, "%s %d", "CreateWindow Failed!!" );
		return iRetCode;
	}

	IDirect3DDevice9* pD3D9Device = NULL;


	iRetCode = W2D_Init( hWnd, &pD3D9Device );
	if( iRetCode != 0 )
	{
		CWOOD_LOG_ERR( iRetCode, "%s", "Game_Init Failed!!" );
		return iRetCode;
	}

	//============================================================================================
	//============================================================================================

	while( TRUE )
	{
		// test if there is a message in queue, if so get it
		if( PeekMessage( &stMsg, NULL, 0, 0, PM_REMOVE ) )
		{
			// test if this is a quit
			if( stMsg.message == WM_QUIT )
				break;

			// translate any accelerator keys
			TranslateMessage( &stMsg );

			// send the message to the window proc
			DispatchMessage( &stMsg );
		} // end if
		/*
		if (KEYDOWN(VK_ESCAPE)){
		SendMessage(hWnd,WM_CLOSE,0,0);
		}
		*/
		// main game processing goes here		
		W2D_Run( hWnd, pD3D9Device );
	} // end while

	W2D_Shutdown();

	return( int )( stMsg.wParam );
}