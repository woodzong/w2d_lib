#ifndef _CVECTOR2_H_123123
#define _CVECTOR2_H_123123

#include "WDefine.h"
#include "CWMath.h"

template< typename T >
class CWVector2
{
public:

	CWVector2() :m_x( 0 ), m_y( 0 ) {}
	CWVector2( const T & x, const T & y ) : m_x( x ), m_y( y ) {}

	~CWVector2() {}

	void CopyFrom( const CWVector2<T> & rVec )
	{
		m_x = rVec.GetX();
		m_y = rVec.GetY();
	}

	CWVector2( const CWVector2<T> * pVec )
	{
		CopyFrom( *pVec );
	}

	bool IsZero() const
	{
		return ( m_x == 0 && m_y == 0 );
	}

	CWVector2( const CWVector2<T> & rVec )
	{
		CopyFrom( rVec );
	}

	CWVector2<T> & operator = ( const CWVector2<T> & rVec )
	{
		CopyFrom( rVec );
		return *this;
	}

	CWVector2<T> & operator += ( const CWVector2<T> & rVec )
	{
		SelfPlus( rVec );
		return *this;
	}

	CWVector2<T> & operator /= ( const T & val )
	{
		SelfDiv( val );
		return *this;
	}

	CWVector2<T> operator + ( const CWVector2<T> & rVec ) const
	{
		CWVector2<T> tmp( *this );
		tmp.SelfPlus( rVec );
		return tmp;
	}

	CWVector2<T> operator - ( const T & val ) const
	{
		CWVector2<T> tmp( *this );
		tmp.SelfMinus( val );
		return tmp;
	}

	CWVector2<T> operator - ( const CWVector2<T> & rVec ) const
	{
		CWVector2<T> tmp( *this );
		tmp.SelfMinus( rVec );
		return tmp;
	}

	CWVector2<T> operator * ( const T & val ) const
	{
		CWVector2<T> tmp( *this );
		tmp.SelfMultiply( val );
		return tmp;
	}

	CWVector2<T> operator / ( const T & val ) const
	{
		CWVector2<T> tmp( *this );
		tmp.SelfDiv( val );
		return tmp;
	}
	
	T DotMultiply( const CWVector2<T> & rVec ) const
	{
		return m_x * rVec.GetX( ) + m_y * rVec.GetY( );
	}

	void SelfMultiply( const T & val )
	{
		m_x *= val;
		m_y *= val;
	}

	void SelfPlus( const CWVector2<T> & rVec )
	{
		m_x += rVec.GetX();
		m_y += rVec.GetY();
	}

	void SelfMinus( const CWVector2<T> & rVec )
	{
		m_x -= rVec.GetX();
		m_y -= rVec.GetY();
	}

	void SelfMinus( const T & val )
	{
		m_x -= val;
		m_y -= val;
	}

	void SelfDiv( const T & val )
	{
		m_x /= val;
		m_y /= val;
	}

	T Length() const
	{
		return ( T )CWMathSim::WSqrt( m_x*m_x + m_y*m_y );
	}

	T SqrLength()
	{
		return m_x * m_x + m_y * m_y;
	}

	T Distance( const CWVector2<T> & rVec ) const
	{
		return ( T )CWMathSim::GetDistance( m_x, m_y, rVec.GetX(), rVec.GetY() );
	}

	T SqrDistance( const CWVector2<T> & rVec ) const
	{
		return ( m_x - rVec.GetX() ) * ( m_x - rVec.GetX() ) + ( m_y - rVec.GetY() ) * ( m_y - rVec.GetY() );
	}

	CWVector2<T> & Normalize()
	{
		T m = Length();

		if( m > ( T )( 0.0f ) )
			m = ( T )( 1.0f / m );
		else
			m = ( T )( 0.0f );

		m_x *= m;
		m_y *= m;

		return *this;
	}

	void SetMagnitudeOfVector( const T & magnitude )
	{
		Normalize();
		m_x *= magnitude;
		m_y *= magnitude;
	}

	void SetX( const T & x )
	{
		m_x = x;
	}

	void SetY( const T & y )
	{
		m_y = y;
	}

	void CopyFrom( const CWVector2<T> *pVec )
	{
		CopyFrom( *pVec );
	}

	void Set( const T & x, const T & y )
	{
		m_x = x;
		m_y = y;
	}

	const T & GetX() const { return m_x; }
	const T & GetY() const { return m_y; }

	bool IsEqual( const CWVector2<T> *pVec ) const
	{
		return m_x == pVec->GetX() && m_y == pVec->GetY();
	}

public:
	T m_x;
	T m_y;
};

#endif

