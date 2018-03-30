#include "CGameProcNNMineSweeper.h"
#include "CWGlobalConf.h"
#include "CWoodLog.h"
#include "CWDraw.h"
#include "CWFont.h"
#include "CWTimer.h"
#include "CWTimePass.h"
#include "CWGlobalUtil.h"
#include "CWVector2.h"
#include "CWPathMoveUnit.h"
#include "CWCamera.h"
#include "CWMath.h"
#include "CWUtil.h"
#include "CNNCfg.h"
#include "ggtinyxml.h"

void CGameProcNNMineSweeper::HandleNumKeyDown( uint8_t ucNumValue )
{
	m_ucKeyDownValue = ucNumValue;
	return;
}

int CGameProcNNMineSweeper::GameInit_i( )
{
	CNNCfg::Init();

	WGCAMERA->SetZoom( 10 );

	TiXmlDocument xmlGameCfg( "./mine_nn.xml" );

	if( xmlGameCfg.LoadFile( ) == false )
	{
		WDRAW_STR( "xmlGGGameSvrCfg.LoadFile Failed:%s Err:%s Row:%d Col:%d", "./mine_nn.xml",
				   xmlGameCfg.ErrorDesc( ), xmlGameCfg.ErrorRow( ), xmlGameCfg.ErrorCol( ) );
		return -1;
	}

	TiXmlElement *pTiXmlGameCfg = CGGTinyXml::FindSiblingElementByName( "GameCfg", CGGTinyXml::GGGetRoot( &xmlGameCfg ) );
	CHECK_NULL_WF( pTiXmlGameCfg, "CGGTinyXml::FindSiblingElementByName GameCfg Failed" );

	TIXML_GET_INT( NNMS_GAMECFG_R, Test, pTiXmlGameCfg );
	TIXML_GET_FLT( NNMS_GAMECFG_R, Bias, pTiXmlGameCfg );
	TIXML_GET_FLT( NNMS_GAMECFG_R, ActivationResponse, pTiXmlGameCfg );
	TIXML_GET_FLT( NNMS_GAMECFG_R, MaxPerturbation, pTiXmlGameCfg );

	TIXML_GET_FLT( NNMS_GAMECFG_R, PassedTimeInMSForNextGeneration, pTiXmlGameCfg );

	TIXML_GET_INT( NNMS_GAMECFG_R, LayerNum, pTiXmlGameCfg );
	TIXML_GET_INT( NNMS_GAMECFG_R, NeuronNumPerLayer, pTiXmlGameCfg );
	TIXML_GET_INT( NNMS_GAMECFG_R, InputNum, pTiXmlGameCfg );
	TIXML_GET_INT( NNMS_GAMECFG_R, OutputNum, pTiXmlGameCfg );

	TIXML_GET_INT( NNMS_GAMECFG_R, SweeperNum, pTiXmlGameCfg );
	TIXML_GET_INT( NNMS_GAMECFG_R, MineNum, pTiXmlGameCfg );

	TIXML_GET_INT( NNMS_GAMECFG_R, TSPEliteNum, pTiXmlGameCfg );
	TIXML_GET_INT( NNMS_GAMECFG_R, TSPEliteCopyNum, pTiXmlGameCfg );
	TIXML_GET_FLT( NNMS_GAMECFG_R, TSPMutationRate, pTiXmlGameCfg );
	TIXML_GET_FLT( NNMS_GAMECFG_R, TSPCrossOverRate, pTiXmlGameCfg );


	m_oMineMng.Init( NNMS_GAMECFG->m_iMineNum );

	m_iSweeperNum = NNMS_GAMECFG->m_iSweeperNum;

	m_oMineSweeper.Init( 0, CW_RES_FILE_PATH( "unit_1.bmp" ), CWVector2<float>( 4000, 4000 ), 20, WCOLOR_DEEPBLUE );
	m_oMineSweeper.InitSetMineCtrl( &m_oMineMng );
	m_oMineSweeper.Reset();

	CNNTSP::s_oPopulation.Init( m_iSweeperNum, m_oMineSweeper.GetWeightNum() );

	for( int i = 0; i < m_iSweeperNum; i++ )
	{
		m_aSweeper[i].Init( i, CW_RES_FILE_PATH( "unit_1.bmp" ), CWVector2<float>( CWMathSim::RandFloatIn01( ) * 10000, CWMathSim::RandFloatIn01( ) * 6000 ), 20, WCOLOR_DEEPBLUE );

		m_aSweeper[i].SetGenomeAsBrain( CNNTSP::s_oPopulation.GetGenomeByIndex( i ) );
		CNNTSP::s_oPopulation.GetGenomeByIndex( i )->SetBindSweeper( &( m_aSweeper[i] ) );

		m_aSweeper[i].InitSetMineCtrl( &m_oMineMng );
		m_aSweeper[i].Reset( );
	}

	return 0;
}


int CGameProcNNMineSweeper::GameRun_i( float fDeltaTime )
{
	ManualCtrl( fDeltaTime );
	bool bIsEpoched = CNNTSP::Update( fDeltaTime );
	if( bIsEpoched == true ) //进化到下一代了. 设置新一代染色体进神经网络
	{
		for( int i = 0; i < m_iSweeperNum; i++ )
		{
			m_aSweeper[i].SetGenomeAsBrain( CNNTSP::s_oPopulation.GetGenomeByIndex( i ) );
			CNNTSP::s_oPopulation.GetGenomeByIndex( i )->SetBindSweeper( &(m_aSweeper[i]) );
		}
	}

	//m_oMineSweeper.Update( fDeltaTime );
	//m_oMineSweeper.RenderOut( );

	for( int i = 0; i < m_iSweeperNum; i++ )
	{
		m_aSweeper[i].Update( fDeltaTime );
		m_aSweeper[i].RenderOut( );
	}

	m_oMineMng.RenderOut( );

	WDRAW_STR( "DeltaTime:%.2f Generation:%d Best:%.0f Total:%.0f Time:%.1f", fDeltaTime, CNNTSP::s_iGeneration, CNNTSP::s_oPopulation.GetBestFitness( ), CNNTSP::s_oPopulation.GetTotalFitness( ), CNNTSP::s_fUsedTimeInMS );
	//WDRAW_STR( "Sweeper Left:%.2f Right:%.2f", m_oMineSweeper.GetTrackLeft(), m_oMineSweeper.GetTrackRight() );
	WDRAW_STR( "BestSweeper %s", CNNTSP::s_oPopulation.GetBestSweeper()->GetDebugInfo() );
	return 0;
}



void CGameProcNNMineSweeper::ManualCtrl( float fDeltaTime )
{
	if( m_ucKeyDownValue == 4 )
	{
		m_oMineSweeper.LeftDown( fDeltaTime );
	}

	if( m_ucKeyDownValue == 7 )
	{
		m_oMineSweeper.LeftUp( fDeltaTime );
	}

	if( m_ucKeyDownValue == 6 )
	{
		m_oMineSweeper.RightDown( fDeltaTime );
	}

	if( m_ucKeyDownValue == 9 )
	{
		m_oMineSweeper.RightUp( fDeltaTime );
	}

	if( m_ucKeyDownValue == 1 )
	{
		m_bIsInLogicMode = true;
	}

	m_ucKeyDownValue = 0;
}