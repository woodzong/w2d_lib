#include "WMain.h"
#include "CWUnit.h"
#include "CWVector2.h"
#include "CGameProc.h"
#include "CWCamera.h"

ST_GCFG CGMain::s_stGCFG;

void UpdateMouseCtrl()
{
	GCTRL->m_iMouseScreenX += GCTRL->m_stMouseState.lX;
	GCTRL->m_iMouseScreenY += GCTRL->m_stMouseState.lY;

	if( GCTRL->m_iMouseScreenX >= GCONF->m_iWindowsWidth )
	{
		GCTRL->m_iMouseScreenX = GCONF->m_iWindowsWidth - 1;
	}
	else if( GCTRL->m_iMouseScreenX < 0 )
	{
		GCTRL->m_iMouseScreenX = 0;
	}

	if( GCTRL->m_iMouseScreenY >= GCONF->m_iWindowsHeight )
	{
		GCTRL->m_iMouseScreenY = GCONF->m_iWindowsHeight - 1;
	}
	else if( GCTRL->m_iMouseScreenY < 0 )
	{
		GCTRL->m_iMouseScreenY = 0;
	}

	if( GCTRL->m_stMouseState.rgbButtons[0] )
	{
		GCTRL->m_bMouseLeftButtonIsClicked = true;
	}
	else
	{
		if( GCTRL->m_bMouseLeftButtonIsClicked == true )
		{
			GCTRL->m_bMouseLeftButtonIsReleased = true;
			GCTRL->m_bMouseLeftButtonIsClicked = false;
		}
		else
		{
			GCTRL->m_bMouseLeftButtonIsReleased = false;
		}
	}

	if( GCTRL->m_stMouseState.rgbButtons[1] )
	{
		GCTRL->m_bMouseRightButtonIsClicked = true;
	}
	else
	{
		if( GCTRL->m_bMouseRightButtonIsClicked == true )
		{
			GCTRL->m_bMouseRightButtonIsReleased = true;
			GCTRL->m_bMouseRightButtonIsClicked = false;
		}
		else
		{
			GCTRL->m_bMouseRightButtonIsReleased = false;
		}
	}
}

int W2D_Init( HWND hWnd, IDirect3DDevice9 **pD3D9Device, void *parms, int num_parms )
{
	int iRetCode = 0;
	IDirect3D9 *pD3d9 = NULL;
	pD3d9 = Direct3DCreate9( D3D_SDK_VERSION );

	if( pD3d9 == NULL )
	{
		iRetCode = -1;
		W_BOX_ALERT_CHAR_INFO( iRetCode, "Direct3DCreate9 Failed pD3d9:%p D3D_SDK_VERSION:%d", pD3d9, D3D_SDK_VERSION );
		return iRetCode;
	}

	D3DDEVTYPE enDevType = D3DDEVTYPE_HAL;
	D3DCAPS9 stD3DCaps;
	pD3d9->GetDeviceCaps( D3DADAPTER_DEFAULT, enDevType, &stD3DCaps );
	int vp = 0;

	if( stD3DCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
	{
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}
	else
	{
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}

	//start init directx Input
	ShowCursor( TRUE );
	HRESULT hr;

	if( FAILED( hr = DirectInput8Create( GetModuleHandle( NULL ), DIRECTINPUT_VERSION, IID_IDirectInput8, ( void ** )& ( GCTRL->m_pDInput ), NULL ) ) )
	{
		W_BOX_ALERT_CHAR_INFO( -1, "DirectInput8Create Failed" );
		return hr;
	}

	if( FAILED( GCTRL->m_pDInput->CreateDevice( GUID_SysMouse, &GCTRL->m_pMouse, NULL ) ) )
	{
		W_BOX_ALERT_CHAR_INFO( -1, "Mouse not found. The sample will now exit." );
		return S_OK;
	}

	// A data format specifies which controls on a device we are interested in,
	// and how they should be reported. This tells DInput that we will be
	// passing a MouseState structure to IDirectInputDevice::GetDeviceState().
	if( GCTRL->m_pMouse->SetDataFormat( &c_dfDIMouse ) != DI_OK )
	{
		W_BOX_ALERT_CHAR_INFO( -1, "CW_MOUSE->SetDataFormat Failed" );
		return -1;
	}

	// Set the cooperative level to let DInput know how this device should
	// interact with the system and with other DInput applications.
	if( FAILED( hr = GCTRL->m_pMouse->SetCooperativeLevel( hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND ) ) )
	{
		W_BOX_ALERT_CHAR_INFO( -1, "CW_MOUSE->SetCooperativeLevel Failed" );
		return hr;
	}

	// acquire the mouse
	if( GCTRL->m_pMouse->Acquire() != DI_OK )
	{
		W_BOX_ALERT_CHAR_INFO( -1, "CW_MOUSE->Acquire Failed" );
		return -1;
	}

	//Init Mouse End



	D3DPRESENT_PARAMETERS d3dpp;
	d3dpp.BackBufferWidth = GCONF->m_iWindowsWidth;
	d3dpp.BackBufferHeight = GCONF->m_iWindowsHeight;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount = 1;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hWnd;
	d3dpp.Windowed = true;
	d3dpp.EnableAutoDepthStencil = true;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	HRESULT hrRetCode = 0;
	hrRetCode = pD3d9->CreateDevice(
		D3DADAPTER_DEFAULT, // primary adapter
		enDevType,         // device type
		hWnd,               // window associated with device
		vp,                 // vertex processing
		&d3dpp,             // present parameters
		pD3D9Device );           // return created device

	if( FAILED( hrRetCode ) )
	{
		CWOOD_LOG_ERR( hrRetCode, "CreateDevice Failed pD3d9:%d ColorDepth 32", pD3d9 );

		// try again using a 16-bit depth buffer
		d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
		hrRetCode = pD3d9->CreateDevice( D3DADAPTER_DEFAULT, enDevType, hWnd, vp, &d3dpp, pD3D9Device );

		if( FAILED( hrRetCode ) )
		{
			CWOOD_LOG_ERR( hrRetCode, "CreateDevice Failed pD3d9:%d ColorDepth 16", pD3d9 );
			pD3d9->Release(); // done with d3d9 object
			return hrRetCode;
		}
	}

	pD3d9->Release(); // done with d3d9 object
	
	iRetCode = WGAMEPROC->GameInit( );
	CHECK_ERR( iRetCode, "CGameProc::Init Failed" );


	CWOOD_LOG_DEBUG( "GameInit Finished, DeviceP:%d", pD3D9Device );

	return 0;

}


int W2D_Run( HWND hWnd, IDirect3DDevice9 *pD3D9Device, void *parms, int num_parms )
{
	LONG lRetCode = 0;

	if( GCTRL->m_pMouse->GetDeviceState( sizeof( DIMOUSESTATE ), ( LPVOID )( &( GCTRL->m_stMouseState ) ) ) != DI_OK )
	{
		CWOOD_LOG_ERR_L( -1, "CW_MOUSE->GetDeviceState Failed" );
		return -1;
	}

	UpdateMouseCtrl();

	if( pD3D9Device == NULL )
	{
		lRetCode = -99;
		CWOOD_LOG_ERR_L( lRetCode, "Dying Alert!! pD3D9Device is NULL, pD3D9Device:%d", pD3D9Device );
		return 0;
	}


	pD3D9Device->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, _RGB32BIT( 255, 240, 240, 240 ), 1.0f, 0 );
	pD3D9Device->BeginScene();

	IDirect3DSurface9 *pBackSurface = NULL;
	lRetCode = pD3D9Device->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackSurface );

	if( lRetCode != D3D_OK )
	{
		CWOOD_LOG_ERR_L( lRetCode, "pD3D9Device->GetBackBuffer Failed, pBackSurface:%d", pBackSurface );
		return lRetCode;
	}

	WGDRAW->UpdateSurface( pBackSurface );


	//compute fps;
	if( GFPS->m_fFlames == 10 )
	{
		GFPS->m_fFPS = GFPS->m_fFlames / GTIMEPASS->GetUsedTime();
		GFPS->m_fFlames = 0;
	}

	WGAMEPROC->GameRun( );

	/*
	if( GCTRL->m_bMouseLeftButtonIsReleased )
	{
	g_oChessMain.OnMouseLeftButtonReleased();
	}
	*/

	//show ChessMain;
	/*
	int iRetCode = 0;
	iRetCode = g_oChessMain.Run();
	CHECK_ERR( iRetCode, "g_oChessMain.Run Failed " );
	*/

	//start main render.
	//GDRAW->DrawPoint32Bit(100,100,0xFF0000);
	CWVector2<float> vWorldPos;

	WGCAMERA->ConvScreenPosToWorldPos( CWVector2<int>( GCTRL->m_iMouseScreenX, GCTRL->m_iMouseScreenY ), &vWorldPos );

	WGDRAW->DrawMouse( GCTRL->m_iMouseScreenX, GCTRL->m_iMouseScreenY );
	WGFONT->ShowString( WGDRAW, "FPS:%.2f", GFPS->m_fFPS );
	WGFONT->ShowString( WGDRAW, "Mouse X:%d Y:%d World X:%.2f Y:%.2f LButton:%d RButton:%d",
						GCTRL->m_iMouseScreenX, GCTRL->m_iMouseScreenY, vWorldPos.GetX( ), vWorldPos.GetY(),
						GCTRL->m_bMouseLeftButtonIsReleased, GCTRL->m_bMouseRightButtonIsReleased );

	//WGFONT_SMALL->ShowString( WGDRAW, "sfont" );

	//end of main render.
	//WGDRAW->DrawLine32Bit( 0, 0, 200, 200, RGB32BIT( 255, 0, 0 ) );

	WGFONT->Reset();
	WGFONT_SMALL->Reset();

	WGDRAW->Release();

	pBackSurface->Release();
	pD3D9Device->EndScene();
	pD3D9Device->Present( 0, 0, 0, 0 );

	//deal FPS;
	if( WGCFG.m_bIsInQuickMode == false ) 
	{
		while( WGTIMER->GetTime() < GFPS->m_fPreRenderTime + CW_FPS_WAITIME )//do one frame per X ms. 16.6 = 1000/60, 60FPS
		{
		}
	}

	GFPS->m_fFlames++;
	GFPS->m_fPreRenderTime = WGTIMER->GetTime();

	return 0;
}



int W2D_Shutdown( void *parms, int num_parms )
{
	WGAMEPROC->GameShutDown( );
	return 0;
}



CGameProc * GetGameProc()
{
	static CGameProc * g_pGameProc = NULL;
	if( g_pGameProc == NULL )
	{
		//g_pGameProc = new CGameProcFlock();
		//g_pGameProc = new CGameProcGridMap( );
		g_pGameProc = new CGameProcUnitMove( );
		//g_pGameProc = new CGameProcGridMapEvolution();
		//g_pGameProc = new CGameProcMapTSP();
		//g_pGameProc = new CGameProcLandingTSP();
		//g_pGameProc = new CGameProcNNMineSweeper( );
	}

	return g_pGameProc;
}
