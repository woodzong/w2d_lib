#ifndef __CMSNNMINESWEEPER_H_123123asdasdasd
#define __CMSNNMINESWEEPER_H_123123asdasdasd

#include "WDefine.h"
#include "CWVectorMoveUnit.h"
#include "CNeuralNet.h"
#include "CNNMineMng.h"

#define SW_SCALING_FACTOR 500   //Ëõ·ÅÒò×Ó

class CNNGenome;

class CNNSweeper :public CWVectorMoveUnit
{
public:

	CNNSweeper( )
	{
	}

	void Reset();

	int Init( int iID, const char * szResBMPFile, const CWVector2<float> & pos, float fRadius, DWORD dwColor );

	int GetWeightNum() const { return m_oBrain.GetWeightNum(); }

	void InitSetMineCtrl( CNNMineMng * pMineCtrl );

	void SetGenomeAsBrain( CNNGenome * pGenome );

	double GetTrackLeft( ) const { return m_fTrackLeft; }
	double GetTrackRight( ) const { return m_fTrackRight; }

	void Update( float fDeltaTimeMS );
	void RenderOut();

	void LeftUp( float fDeltaTimeMS );
	void LeftDown( float fDeltaTimeMS );
	void RightUp( float fDeltaTimeMS );
	void RightDown( float fDeltaTimeMS );

	void SetDrawBold( bool bIsBold );

	const char * GetDebugInfo() const;

private:
	CNNMineMng * m_pMineCtrl;

	CNNGenome * m_pGenome;

	CNeuralNet m_oBrain;

	bool m_bIsBold;
	//CWVector2<float> m_vInitPos;

	double m_fFitness;
	int m_iClosestMine;

	double m_fTrackLeft;
	double m_fTrackRight;
	double m_fRotation;
};

#endif
