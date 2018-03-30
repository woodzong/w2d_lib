#include "WDefine.h"
#include "CNNSweeper.h"
#include "CWCamera.h"
#include "CWDraw.h"
#include "CNNCfg.h"
#include "CNNGenome.h"

#define MAX_TURN_RATE 0.3f

void CNNSweeper::InitSetMineCtrl( CNNMineMng * pMineCtrl )
{
	m_pMineCtrl = pMineCtrl;
}

int CNNSweeper::Init( int iID, const char * szResBMPFile, const CWVector2<float> & pos, float fRadius, DWORD dwColor )
{
	m_bIsBold = false;
	m_oBrain.Init( NNMS_GAMECFG->m_iLayerNum, NNMS_GAMECFG->m_iNeuronNumPerLayer, NNMS_GAMECFG->m_iInputNum, NNMS_GAMECFG->m_iOutputNum );
	return CWVectorMoveUnit::Init( iID, szResBMPFile, pos, fRadius, dwColor );
}

void CNNSweeper::Reset( )
{
	m_fTrackLeft = 0.0f;
	m_fTrackRight = 0.0f;
	m_vSpeed = CWVector2<float>( 0, 0 );
	m_pos = CWVector2<float>( CWMathSim::RandFloatIn01( ) * 10240, CWMathSim::RandFloatIn01( ) * 7680 );	
	m_fRotation = ( double )( CWMathSim::RandFloatIn01( ) * ( 3.141592 * 2 ) );
	//m_fRotation = WMATH_PI / 2;
	//m_fRotation = WMATH_PI / 2;
	SetDebugFactor( SW_SCALING_FACTOR );
}

void CNNSweeper::SetGenomeAsBrain( CNNGenome * pGenome )
{
	m_fFitness = 0.0f;
	m_pGenome = pGenome;
	m_oBrain.SetWeight( pGenome );
	m_pGenome->SetFitness( 0 );
	Reset();
}

void CNNSweeper::LeftUp( float fDeltaTimeMS )
{
	m_fTrackLeft += 0.1f;
}

void CNNSweeper::LeftDown( float fDeltaTimeMS )
{
	m_fTrackLeft -= 0.1f;
}

void CNNSweeper::RightUp( float fDeltaTimeMS )
{
	m_fTrackRight += 0.1f;
}

void CNNSweeper::RightDown( float fDeltaTimeMS )
{
	m_fTrackRight -= 0.1f;
}

void CNNSweeper::SetDrawBold( bool bIsBold )
{
	if( m_bIsBold != bIsBold )
	{
		if( bIsBold )
		{
			SetShowColor( WCOLOR_RED );
			SetBMPFile( CW_RES_FILE_PATH( "unit_0.bmp" ) );
		}
		else
		{
			SetShowColor( WCOLOR_DEEPBLUE );
			SetBMPFile( CW_RES_FILE_PATH( "unit_1.bmp" ) );
		}
	}

	m_bIsBold = bIsBold;
}

void CNNSweeper::Update( float fDeltaTimeMS )
{
	int iSweptNum = m_pMineCtrl->MineBeSwept( GetPosition() );
	if( iSweptNum > 0 )
	{
		m_fFitness += iSweptNum;
		m_pGenome->SetFitness( m_fFitness );
	}


	CWUnit * pMine = m_pMineCtrl->GetClosestMine( GetPosition() );
	if( pMine == NULL )
	{
		return;
	}

	CWVector2<float> vClosetPos = pMine->GetPosition() - m_pos;
	vClosetPos.Normalize( );
	
	ST_NEURON_RET stInput;
	CNeuronRetUtil::AddRet( &stInput, vClosetPos.GetX( ) );
	CNeuronRetUtil::AddRet( &stInput, vClosetPos.GetY( ) );

	CNeuronRetUtil::AddRet( &stInput, cos( m_fRotation ) ); //direction x
	CNeuronRetUtil::AddRet( &stInput, sin( m_fRotation ) ); //direction y

	ST_NEURON_RET * pstOutput = m_oBrain.Update( &stInput );

	m_fTrackLeft = pstOutput->m_afRet[0];
	m_fTrackRight = pstOutput->m_afRet[1];

	double dRotationForce = m_fTrackLeft - m_fTrackRight; //计算转向力的大小

	CWMathSim::ClampDouble( dRotationForce, -MAX_TURN_RATE, MAX_TURN_RATE );

	m_fRotation += dRotationForce;

	double fSpeed = m_fTrackLeft + m_fTrackRight;

	CWVector2<float> vDirection;
	vDirection.Set( cos( m_fRotation ), sin( m_fRotation ) );
	vDirection.SetMagnitudeOfVector( fSpeed );

	m_vSpeed = vDirection;

	CWVectorMoveUnit::WUpdate( fDeltaTimeMS );

	s_pCamera->ClampPosInCamera( &m_pos );
}

const char * CNNSweeper::GetDebugInfo() const
{
	static char s_szBuff[255];
	_snprintf_s( s_szBuff, sizeof( s_szBuff ), "Left:%.4f Right:%.4f ID:%d M:%.2f %.2f Ro:%.2f %.2f %.2f Fit:%.0f",
				 m_fTrackLeft, m_fTrackRight, m_iID,
				 m_pMineCtrl->GetClosestMine( GetPosition() )->GetPosition().GetX(), m_pMineCtrl->GetClosestMine( GetPosition() )->GetPosition().GetY(),
				 m_fRotation, sin( m_fRotation ), cos( m_fRotation ), m_fFitness );
	return s_szBuff;
}

void CNNSweeper::RenderOut( )
{
	CWVectorMoveUnit::RenderOut();

	CWVector2<float> vLookAtDirection;
	vLookAtDirection.Set( cos( m_fRotation ), sin( m_fRotation ) );
	vLookAtDirection.SetMagnitudeOfVector( 70 );

	//render direction out;
	CWVector2<int> oBeginPos;
	s_pCamera->ConvWorldPosToScreenPos( m_pos, &oBeginPos );

	CWVector2<float> directPos( m_pos );
	directPos.SelfPlus( vLookAtDirection );

	CWVector2<int> oEndPos;
	s_pCamera->ConvWorldPosToScreenPos( directPos, &oEndPos );

	s_pDraw->DrawLine32BitWithSize( oBeginPos.GetX(), oBeginPos.GetY(), oEndPos.GetX(), oEndPos.GetY(), WCOLOR_RED, EN_WDRAW_SIZE_2 );
}