#ifndef __CWPATH_H_13123123123
#define __CWPATH_H_13123123123

#include "CWVector2.h"
#include "CWDrawable.h"

#define MAX_POINT_NUM_IN_ONE_PATH 200

class CWPath :public CWDrawable
{
public:
	CWPath() 
	{ 
		m_iNum = 0; 
		m_dwLineColor = WCOLOR_GREEN;
		m_dwPointColor = WCOLOR_RED;
		m_enPointSize = EN_WDRAW_SIZE_3;
		m_enLineSize = EN_WDRAW_SIZE_1;
	}

	const int & GetNum() const { return m_iNum; }

	int AddPoint( const CWVector2<float> & rPos );

	int RemovePoint( int iRemoveNum );

	void Clear();

	const CWVector2<float> MoveDis( const CWVector2<float> & rStartPos, float fMoveDis );

	virtual void RenderOut( );

	void SetStartPos( const CWVector2<float> & rPos )
	{
		m_oStartPos.CopyFrom( rPos );
	}

private:
	EN_WDRAW_SIZE m_enPointSize;
	EN_WDRAW_SIZE m_enLineSize;
	unsigned int m_dwLineColor;
	unsigned int m_dwPointColor;

	CWVector2<float> m_oStartPos;

	int m_iNum;
	CWVector2<float> m_aPath[MAX_POINT_NUM_IN_ONE_PATH];
};

#endif