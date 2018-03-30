#ifndef _CWCAMERA_H_123123
#define _CWCAMERA_H_123123

#include "WDefine.h"
#include "CWVector2.h"


typedef enum
{
	EN_ORGINPOINT_TYPE_MIDDLE_CENTER = 1, //屏幕中点为世界坐标原点
	EN_ORGINPOINT_TYPE_LEFT_BOTTOM = 2, //屏幕左下角为世界坐标原点
}EN_ORGINPOINT_TYPE;

class CWCamera
{
public:
	CWCamera();

	void SetOrginPointType( EN_ORGINPOINT_TYPE enOrginPointType );

	void MoveRight( float fMoveSpeed );
	void MoveLeft( float fMoveSpeed );
	void MoveUp( float fMoveSpeed );
	void MoveDown( float fMoveSpeed );

	void SetPos( float fX, float fY );
	const CWVector2<float> & GetPos() const { return m_oPos; }

	int GetMapWidth() const { return m_iMapWidth; }
	int GetMapHeight() const { return m_iMapHeight; }

	void InitWindowSize( int iWindowWidth, int iWindowHeight );
	void SetZoom( float fZoom );

	int GetWindowWidth() const { return m_iWindowWidth; }
	int GetWindowHeight() const { return m_iWindowHeight; }

	void ConvWorldPosToScreenPos( const CWVector2<float> & rWorldPos, CWVector2<int> * pScreenPosOut );
	void ConvScreenPosToWorldPos( const CWVector2<int> & rScreenPos, CWVector2<float> * pWorldPosOut );

	void GetLeftDownPointWorldPos( CWVector2<float> * pWorldPosOut );
	void GetRightUpPointWorldPos( CWVector2<float> * pWorldPosOut );
	void ClampPosInCamera( CWVector2<float> * pPos ); //把整个视窗世界, 变成一个循环的无限区域

private:
	int m_iMapWidth;
	int m_iMapHeight;

	float m_fZoomX;
	float m_fZoomY;

	CWVector2<float> m_oPos;
	int m_iWindowWidth;
	int m_iWindowHeight;

	EN_ORGINPOINT_TYPE m_enOrginPointType;
};


#define POS_TO_SCREEN_X(iX) (int)((iX - s_pCamera->GetOffsetX() )/ s_pCamera->GetPixelCell() )
#define POS_TO_SCREEN_Y(iY) (int)((s_pCamera->GetMapHeight() - iY - 1 - s_pCamera->GetOffsetY() ) / s_pCamera->GetPixelCell() )

#endif

