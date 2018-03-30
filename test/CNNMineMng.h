#ifndef __CNNMINE_MNG_H_12312asdasdasd
#define __CNNMINE_MNG_H_12312asdasdasd

#include "WDefine.h"
#include "CWUnit.h"
#include "CNNCfg.h"

class CNNMineMng
{
public:

	CNNMineMng( )
	{
	}

	void RenderOut();

	CWUnit * GetClosestMine( const CWVector2<float> & rPos );

	int MineBeSwept( const CWVector2<float> & rSweeperPos );

	void Init( int iMineNum );

private:
	void RandInitOneMine( CWUnit * pMine );
	
private:
	int m_iMineNum;
	CWUnit m_aMine[NN_MAX_MINE_NUM];
};

#endif
