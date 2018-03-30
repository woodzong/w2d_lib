#include "CGameProcLandingTSP.h"
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


void CWLandingGenome::Mutate()
{
	for( int i = 0; i < LANDING_TSP_GEN_LEN; i++ )
	{
		if( CWMathSim::RandFloatIn01( ) < LANDING_TSP_MUTATION_RATE )
		{
			//rand a new action
			m_aActionList[i].RandAction( );
		}

		if( CWMathSim::RandFloatIn01() < LANDING_TSP_MUTATION_RATE/2 )
		{
			//rand a new action
			m_aActionList[i].MuteDuration();
		}
	}
}


void CWLandingShip::RenderOut()
{
	CWVectorMoveUnit::RenderOut();

	CWVector2<float> vDirectForShow;
	vDirectForShow = m_pos + m_vDirection * 200;
	CWUtil::DrawLineInWorld( m_pos, vDirectForShow, WCOLOR_BLUE, EN_WDRAW_SIZE_1 );

	if( m_bIsFiring ) //draw fire out
	{
		CWVector2<float> vFireForShow;
		vDirectForShow = m_pos - m_vDirection * 100;
		CWUtil::DrawLineInWorld( m_pos, vDirectForShow, WCOLOR_RED, EN_WDRAW_SIZE_3 );
	}
}


void CWLandingShip::Update( float fDeltaTime )
{
	m_fInAirTime += fDeltaTime;

	CWVector2<float> vGForce( 0.0f, -1.0f );
	vGForce.SelfMultiply( PLANET_GALITY );

	m_vDirection.Set( cos( m_fRotation ), sin( m_fRotation ) );


	CWVector2<float> vTForce( vGForce );
	if( m_bIsFiring == true )
	{
		CWVector2<float> vSelfForce( m_vDirection );
		vSelfForce.SelfMultiply( m_fForce );
		vTForce += vSelfForce;
	}

	vTForce.SelfDiv( m_fMass ); //除以质量, 变成加速度了.

	m_vSpeed += ( vTForce * fDeltaTime / 1000 );

	WUpdate( fDeltaTime );
}


void CWLandingShip::RunOneAction( const CWLandingAction * pAction, float fDeltaTime )
{
	SetIsFiring( false );

	switch( pAction->GetAction() )
	{
		case EN_LANDING_ACTION_LEFT:
			TurnLeft( fDeltaTime );
			break;

		case EN_LANDING_ACTION_RIGHT:
			TurnRight( fDeltaTime );
			break;

		case EN_LANDING_ACTION_FIRE:
			SetIsFiring( true );
			break;

		case EN_LANDING_ACTION_NULL: //do nothing
		default:
			break;
	}
}


void CGameProcLandingTSP::HandleNumKeyDown( uint8_t ucNumValue )
{
	m_ucKeyDownValue = ucNumValue;
	return;
}


int CGameProcLandingTSP::GameInit_i()
{
	m_ucKeyDownValue = 0;

	m_bIsInManualCtrl = false;
	m_oBestGenome.InitByRand();

	for( int i = 0; i < LANDING_TSP_POPULATION_NUM; i++ )
	{
		m_aGenome[i].InitByRand();
		m_apGenome[i] = &( m_aGenome[i] );
	}

	WGCAMERA->SetZoom( 10 );

	m_oShip.Init( 1, CW_RES_FILE_PATH( "unit_1.bmp" ), CWVector2<float>( 4000, 4000 ), 20, WCOLOR_DEEPBLUE );

	m_vLandingPos.Set( 8000, 100 );
	m_fLandingPosLength = 100;

	m_fBestScore = 0.0f;
	m_fWorstScore = 0.0f;
	m_fTotalScore = 0.0f;
	m_fAvgScore = 0.0f;
	m_iGeneration = 0;
	m_iSafeLandindIndex = -1;
	m_fReplayActionRunnedTime = 0.0f;

	return 0;
}


float CGameProcLandingTSP::CaclShipStatFitScore()
{
	float fFitInAirTime = m_oShip.GetInAirTime() / 1000;
	float fFitSpeed = m_oShip.GetSpeed().Length();
	float fFitRotation = 1 /( fabs( m_oShip.GetRotation() - ( WMATH_PI / 2 ) ) + 1 );

	float fFitDist =  1 / ( fabs( m_oShip.GetPosition( ).GetX( ) - m_vLandingPos.GetX( ) ) + 1 ) ;
	if( fabs( m_oShip.GetPosition().GetX() - m_vLandingPos.GetX() ) <= m_fLandingPosLength / 2 )
	{
		fFitDist = 1.0f;
	}

	float fFitSpeedTmp = 1 / ( fabs( m_oShip.GetSpeed( ).Length( ) ) + ( 1 - LANDING_TSP_SAFE_SPEED_M ) );

	//WGFONT->ShowString( WGDRAW, "Ship Landed FitInAirTime:%.2f FitSpeed:%.2f FitAScore:%.2f FitRotation:%.2f FitDist:%.2f",
	//					  fFitInAirTime, fFitSpeed, 4 * fFitInAirTime / ( fFitSpeed + 1 ), 400 * fFitRotation, fFitDist );

	return 400 * fFitDist + 400 * fFitRotation + 400 * fFitSpeedTmp;
	//return 400 * fFitDist + 400 * fFitRotation + 4 * fFitInAirTime / ( fFitSpeed + 1 ) + fFitSpeedTmp;
}


bool CGameProcLandingTSP::IsShipInSafeStat()
{
	const int SAFE_LANDING_ANGLE_IN_AGREE = 5;
	const int SAFE_LANDING_MAX_SPEED = 10;

	if( m_oShip.GetRotation() >= ( WMATH_PI / 2 ) + ( WMATH_PI * 2 / 180 ) ) // 在5度范围内, 算安全角度
	{
		return false;
	}

	if( m_oShip.GetRotation( ) <= ( WMATH_PI / 2 ) - ( WMATH_PI * 2 / 180 ) ) // 在5度范围内, 算安全角度
	{
		return false;
	}

	//当前速度标量
	if( m_oShip.GetSpeed( ).Length( ) >= LANDING_TSP_SAFE_SPEED_M )
	{
		return false;
	}

	if( m_oShip.GetPosition().GetX() <= m_vLandingPos.GetX() - m_fLandingPosLength / 2
		|| m_oShip.GetPosition().GetX() >= m_vLandingPos.GetX() + m_fLandingPosLength / 2 )
	{
		return false;
	}

	return true;
}


bool CGameProcLandingTSP::IsShipLanded()
{
	if( m_oShip.GetPosition().GetY() <= m_vLandingPos.GetY() )
	{
		return true;
	}
	else
	{
		return false;
	}
}


void CGameProcLandingTSP::ManualCtrl( float fDeltaTime )
{
	if( m_ucKeyDownValue == 4 )
	{
		m_oShip.TurnLeft( fDeltaTime );
	}

	if( m_ucKeyDownValue == 6 )
	{
		m_oShip.TurnRight( fDeltaTime );
	}

	static int s_iFiringNum = 0;
	if( m_ucKeyDownValue == 5 )
	{
		s_iFiringNum = 60;
		m_oShip.SetIsFiring( true );
	}
	else
	{
		if( s_iFiringNum == 0 )
		{
			m_oShip.SetIsFiring( false );
		}
		else
		{
			s_iFiringNum--;
		}
	}

	m_ucKeyDownValue = 0;
}


void CGameProcLandingTSP::ReplayGenome( CWLandingGenome * pCurRuningGenome, float fDeltaTime )
{
	int i = pCurRuningGenome->GetCurActionIndex();
	const CWLandingAction * pAction = pCurRuningGenome->GetAction( i );
	if( pAction != NULL )
	{
		m_fReplayActionRunnedTime += fDeltaTime;
		m_oShip.RunOneAction( pAction, fDeltaTime );

		if( pAction->IsRunOut( ( int )m_fReplayActionRunnedTime ) == true )
		{
			m_fReplayActionRunnedTime = 0.0f;
			pCurRuningGenome->NextActionIndex();
		}
	}
	else
	{
		m_oShip.SetNullAction();
	}
}



int CGameProcLandingTSP::GameRun_i( float fDeltaTime )
{
	static float s_fReplayPassedTime = 0.0f;

	if( m_bIsInManualCtrl == true )
	{
		if( IsShipLanded() == false )
		{
			ManualCtrl( fDeltaTime );
			m_oShip.Update( fDeltaTime );
		}
		else
		{
			WGFONT->ShowString( WGDRAW, "Ship Landed(%.1f, %.1f) Rotation:%.2f SpeedM:%.2f",
								m_oShip.GetPosition().GetX(), m_oShip.GetPosition().GetY(), m_oShip.GetRotation(), m_oShip.GetSpeed().Length() );
		}
		m_oShip.RenderOut();
	}
	else
	{
		WGFONT->ShowString( WGDRAW, "BestGenome:%s", m_oBestGenome.GetDebugInfo() );

		if( m_iSafeLandindIndex == -1 ) //不断演化, 还没有找到解
		{
			do
			{
				bool bIsFound = Epoch( fDeltaTime );
				if( bIsFound )
				{
					m_oShip.Reset();
					break;
				}
			}
			while( m_iGeneration % 50 != 0 );
		}
		else
		{
			if( IsShipLanded() )
			{
				WGFONT->ShowString( WGDRAW, "Ship Landed(%.1f, %.1f) Rotation:%.2f SpeedM:%.2f Gen:%d",
									m_oShip.GetPosition().GetX(), m_oShip.GetPosition().GetY(), m_oShip.GetRotation(), m_oShip.GetSpeed().Length(), m_iGeneration );
				WGFONT->ShowString( WGDRAW, "Ship Landed LandingX:%.0f LandingXE:%.0f PassedTime:%.f FitScore:%.2f",
									m_vLandingPos.GetX(), m_vLandingPos.GetX() + m_fLandingPosLength, s_fReplayPassedTime, CaclShipStatFitScore() );
				s_fReplayPassedTime = 0.0f;
				m_fReplayActionRunnedTime = 0.0f;
				m_oShip.Reset();
				m_oBestGenome.ResetStat();
			}
			else
			{
				s_fReplayPassedTime += fDeltaTime;
				ReplayGenome( &m_oBestGenome, fDeltaTime );

				WGFONT->ShowString( WGDRAW, "CurActionIndex:%d PassedTime:%.2f fDeltaTime:%.2f", m_oBestGenome.GetCurActionIndex(), s_fReplayPassedTime, fDeltaTime );

				m_oShip.Update( fDeltaTime );
				m_oShip.RenderOut();
			}
		}
		WGFONT->ShowString( WGDRAW, "Generation:%d BestScore:%.2f WorstScore:%.2f TotalScore:%.2f", m_iGeneration, m_fBestScore, m_fWorstScore, m_fTotalScore );
	}

	WGFONT->ShowString( WGDRAW, "ShipPos(%.2f,%.2f) SpeedLen:%.2f Rotation:%.2f", m_oShip.GetPosition().GetX(), m_oShip.GetPosition().GetY(), m_oShip.GetSpeed().Length(), m_oShip.GetRotation()  );

	CWVector2<float> vPosBegin( m_vLandingPos.GetX() - m_fLandingPosLength / 2, m_vLandingPos.GetY( ) );
	CWVector2<float> vPosEnd( m_vLandingPos.GetX() + m_fLandingPosLength / 2, m_vLandingPos.GetY() );
	CWUtil::DrawLineInWorld( vPosBegin, vPosEnd, WCOLOR_DEEPBLUE, EN_WDRAW_SIZE_4 );

	return 0;
}

bool CGameProcLandingTSP::CaclGenomeScore( CWLandingGenome * pCurRuningGenome, float fTickDeltaTime )
{
	m_oShip.Reset();
	for( int i = 0; i < pCurRuningGenome->Len(); i++ )
	{
		const CWLandingAction * pAction = pCurRuningGenome->GetAction( i );
		if( pAction == NULL )
		{
			break;
		}

		float fThisActionRunnedTime = 0.0f;
		while( pAction->IsRunOut( ( int )fThisActionRunnedTime ) == false )
		{
			m_oShip.RunOneAction( pAction, fTickDeltaTime );
			m_oShip.Update( fTickDeltaTime );

			if( IsShipLanded() )
			{
				//计算总分
				float fFitScore = CaclShipStatFitScore();
				pCurRuningGenome->SetFitScore( fFitScore );
				m_fTotalScore += fFitScore;

				if( IsShipInSafeStat() == true )
				{
					return true;
				}				
				else
				{
					return false;
				}
			}

			fThisActionRunnedTime += fTickDeltaTime;		
		}
	}

	//计算这个适应分数
	return false;
}


int SortGenomeByScore( const void *left, const void *right )
{
	const CWLandingGenome * pGenomeLeft  = *( const CWLandingGenome ** )( left );
	const CWLandingGenome * pGenomeRight = *( const CWLandingGenome ** )( right );

	float fDFitScore = pGenomeLeft->GetFitScore() - pGenomeRight->GetFitScore();

	if( fDFitScore > 0 )
	{
		return -1;
	}
	else
	{
		return 1;
	}
}


int CGameProcLandingTSP::SelectParentIndex( )
{
	int iIndexOut = 0;

	float fRand = ( float )CWMathSim::RandFloatIn01( ) * m_fTotalScore;

	float fTmpTotal = 0.0f;
	for( int i = 0; i < LANDING_TSP_POPULATION_NUM; i++ )
	{
		fTmpTotal += m_aGenome[i].GetFitScore( );

		if( fRand <= fTmpTotal )
		{
			return i;
		}
	}

	return 0;
}


void CGameProcLandingTSP::CrossoverPMX( CWLandingGenome * pGenomeParentA, CWLandingGenome * pGenomeParentB, CWLandingGenome * pGenomeChildA, CWLandingGenome * pGenomeChildB )
{
	float fRand = CWMathSim::RandFloatIn01();
	if( fRand > LANDING_TSP_CROSSOVER_RATE )
	{
		*pGenomeChildA = *pGenomeParentA;
		*pGenomeChildB = *pGenomeParentB;
		return;
	}

	float fSwapRate = CWMathSim::RandFloatIn01() * LANDING_TSP_GEN_LEN;

	for( int i = 0; i < LANDING_TSP_GEN_LEN; i++ )
	{
		if( CWMathSim::RandFloatIn01( ) < fSwapRate ) //swap it
		{
			pGenomeChildA->SetAction( i, pGenomeParentB->GetAction( i ) );
			pGenomeChildB->SetAction( i, pGenomeParentA->GetAction( i ) );
		}
		else
		{
			pGenomeChildA->SetAction( i, pGenomeParentA->GetAction( i ) );
			pGenomeChildB->SetAction( i, pGenomeParentB->GetAction( i ) );
		}
	}
}



bool CGameProcLandingTSP::Epoch( float fDeltaTime )
{
	m_fBestScore = 0.0f;
	m_fWorstScore = 0.0f;
	m_fTotalScore = 0.0f;
	m_fAvgScore = 0.0f;
	m_iSafeLandindIndex = -1;

	for( int i = 0; i < LANDING_TSP_POPULATION_NUM; i++ )
	{
		bool bIsFound = CaclGenomeScore( &(m_aGenome[i]), fDeltaTime );
		if( bIsFound == true )
		{
			m_iSafeLandindIndex = i;
			m_oBestGenome = m_aGenome[i];
			return true;
		}
	}

	qsort( m_apGenome, LANDING_TSP_POPULATION_NUM, sizeof( m_apGenome[0] ), SortGenomeByScore );
	m_fBestScore = m_apGenome[0]->GetFitScore();
	m_fWorstScore = m_apGenome[LANDING_TSP_POPULATION_NUM-1]->GetFitScore( );

	m_oBestGenome = *( m_apGenome[0] );
	
	CWLandingGenome aGenomeNew[LANDING_TSP_POPULATION_NUM];
	for( int i = 0; i < LANDING_TSP_NUM_ELITE; i++ )
	{
		aGenomeNew[i] = *( m_apGenome[i] );
	}

	for( int i = LANDING_TSP_NUM_ELITE; i < LANDING_TSP_POPULATION_NUM - 1; i+=2 )
	{
		int iParentAIndex = SelectParentIndex();
		int iParentBIndex = SelectParentIndex();

		CWLandingGenome * pGenomeParentA = &( m_aGenome[iParentAIndex] );
		CWLandingGenome * pGenomeParentB = &( m_aGenome[iParentBIndex] );
		CWLandingGenome * pGenomeChildA = &( aGenomeNew[i] );
		CWLandingGenome * pGenomeChildB = &( aGenomeNew[i+1] );

		CrossoverPMX( pGenomeParentA, pGenomeParentB, pGenomeChildA, pGenomeChildB );

		pGenomeChildA->Mutate( );
		pGenomeChildB->Mutate( );
	}

	for( int i = 0; i < LANDING_TSP_POPULATION_NUM; i++ )
	{
		m_aGenome[i] = aGenomeNew[i];
		m_aGenome[i].ResetStat();
	}

	m_iGeneration++;

	return false;
}

