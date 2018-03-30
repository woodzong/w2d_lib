#ifndef __CWGRIDMAP_H_13123123123
#define __CWGRIDMAP_H_13123123123

#include "WDefine.h"
#include "CWDrawable.h"
#include "CWVector2.h"

extern "C"
{
#include "gsastar.h"
}


class CWPathMoveUnit;

class CWGridMap :public CWDrawable
{
public:
	CWGridMap()
	{
		m_iMapDataID = -1;
		m_dwMapCorColor = WCOLOR_YELLOW;
	}

	~CWGridMap();
	
	void RandInitMapBlock( int iRandIndex );

	int Init( int iMapGridWidth, int iMapGridHeight );

	virtual void RenderOut( );

	void RenderOneGrid( int iGridX, int iGridY, uint8_t ucGridVal );

	void SetOneGridBlockVal( int iGridX, int iGridY, uint8_t ucGridVal );

	uint8_t GetGridBlockVal( int iGridX, int iGridY );

	int FindPath( CWPathMoveUnit * pPathMoveUnit, const CWVector2<float> & rTargetPos );

private:
	int m_iMapDataID;
	uint32_t m_dwMapCorColor;
};

#endif