#ifndef __CMOVE_UNIT_H_1231823718231263
#define __CMOVE_UNIT_H_1231823718231263

#include "WMapDefine.h"
#include "CWBMPLoader.h"
#include "CWVector2.h"
#include "CWDrawable.h"

typedef enum
{
	EN_UNIT_SHOWCTL_BIT_ID = 0x1, // «∑Òœ‘ æID
}EN_UNIT_SHOWCTL_BIT;

class CWUnit :public CWDrawable
{
public:
	CWUnit()
	{
		m_iID = 0;
		m_fRadius = 0;
		m_wShowFlag = 0xFFFF;
	}

	void SetShowColor( DWORD dwColor )
	{
		m_dwColor = dwColor;
	}

	int Init( int iID, const char * szResBMPFile, const CWVector2<float> & pos, float fRadius, DWORD dwColor = UNIT_DEFAULT_COLOR );

	void SetBMPFile( const char * szResBMPFile );

	void SetIsShowID( bool bIsShowID )
	{
		if( bIsShowID )
		{
			W_SET_BIT( m_wShowFlag, EN_UNIT_SHOWCTL_BIT_ID );
		}
		else
		{
			W_CLEAR_BIT( m_wShowFlag, EN_UNIT_SHOWCTL_BIT_ID );
		}		
	}

	void SetPosition( const CWVector2<float> * pPos ) 
	{ 
		m_pos.CopyFrom( pPos );
	}

	int GetID() const { return m_iID; }

	virtual void RenderOut( );

	const CWVector2<float> & GetPosition() const
	{
		return m_pos;
	}

	float GetRadius() { return m_fRadius; }

protected:
	uint16_t m_wShowFlag;
	int m_iID;
	CWVector2<float> m_pos;
	float m_fRadius;
	DWORD m_dwColor;
	CBMPLoader m_oBMPLoader;
};

#endif
