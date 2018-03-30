#ifndef _GGTINYXML_H_123123123
#define _GGTINYXML_H_123123123

#include "tinyxml.h"
#include <stdint.h>
#include <string>


class CGGTinyXml
{
public:
    static TiXmlElement * FindChildElementByName( const char * szName, TiXmlElement * pTiXmlElement );
    static TiXmlElement * FindSiblingElementByName( const char * szName, TiXmlElement * pTiXmlElement );
	static TiXmlElement * GGGetRoot(TiXmlDocument * pTiXmlDoc , const char* pszRootName="GSCfg");
	static TiXmlAttribute * FindAttributeByName(const char * szName, TiXmlElement * pTiXmlElement);
	//check format
	static int CheckFormat(const std::string & strFile, std::string & strErrMsg);

	static int CheckDuplicateChildName( TiXmlElement * pFirstChildTiXmlElement );
	static void SetIsAllowDuplicateChildName( bool bIsAllow ) { s_bIsAllowDuplicateChildName = bIsAllow; }

private:
	static int CheckOneDuplicateChildName( TiXmlElement * pChildTiXmlElement );
	static bool s_bIsAllowDuplicateChildName;
};



class CGGString
{
public:

	static char * ToUpper( char * szInOut );

	static const char * ConvToStringUINT( uint32_t dwValue )
	{
		static char s_szTmp[12];
		_snprintf_s( s_szTmp, sizeof( s_szTmp ), "%u", dwValue );
		return s_szTmp;
	}

	static const char * ConvToStringINT( int iValue )
	{
		static char s_szTmp[12];
		_snprintf_s( s_szTmp, sizeof( s_szTmp ), "%d", iValue );
		return s_szTmp;
	}

	static const char * ConvToStringUINT64( uint64_t ullValue )
	{
		static char s_szTmp[32];
		_snprintf_s( s_szTmp, sizeof( s_szTmp ), "%lu", ullValue );
		return s_szTmp;
	}

	static const char * ConvToStringFLOATPrice( uint64_t ullValue )
	{
		float tmp = (float)( ullValue / 10000);
		static char s_szTmp[32];
		_snprintf_s( s_szTmp, sizeof( s_szTmp ), "%.4f", tmp );
		return s_szTmp;
	}

	static int SafeAtoi( const char *pStr )
	{
		if( pStr == NULL )
		{
			return 0;
		}

		return atoi( pStr );
	}

	static int64_t SafeAtoll( const char *pStr )
	{
		if( pStr == NULL )
		{
			return 0;
		}

		return( int64_t )atoll( pStr );
	}

	static uint64_t SafeAtoull( const char *pStr )
	{
		if( pStr == NULL )
		{
			return 0;
		}

		return( uint64_t )strtoull( pStr, 0, 10 );
	}

	static double SafeAtoDouble( const char *pStr )
	{
		if( pStr == NULL )
		{
			return 0;
		}

		return atof( pStr );
	}

	static float SafeAtoFloat( const char *pStr )
	{
		if( pStr == NULL )
		{
			return 0;
		}

		return strtof( pStr, NULL );
	}

	static uint32_t SafeAtoui( const char *pStr )
	{
		if( pStr == NULL )
		{
			return 0;
		}

		return( unsigned int )atoll( pStr );
	}

	static const char * SafeMysqlRowCharPointer( const char * pRowCharPointer )
	{
		if( pRowCharPointer == NULL )
		{
			return "";
		}
		return pRowCharPointer;
	}
};

#define CHECK_NULL_WF( pPointer, format, ... )\
if( pPointer == NULL )\
{\
	printf( "%s is NULL " format, #pPointer, ##__VA_ARGS__ ); \
	return -1; \
}

#define STRCPYM( szTarget, pSrc )\
	_snprintf_s( szTarget, sizeof( szTarget ), "%s", pSrc )

#define TIXML_GET_FLT( stCfg, CfgName, pTiXmlCfg )\
	TiXmlElement *p##CfgName = CGGTinyXml::FindChildElementByName( #CfgName, pTiXmlCfg ); \
	CHECK_NULL_WF( p##CfgName, "CGGTinyXml::FindChildElementByName %s Failed", #CfgName ); \
	( stCfg ).m_f##CfgName = CGGString::SafeAtoFloat( p##CfgName->GetText( ) )

#define TIXML_GET_DBL( stCfg, CfgName, pTiXmlCfg )\
	TiXmlElement *p##CfgName = CGGTinyXml::FindChildElementByName( #CfgName, pTiXmlCfg ); \
	CHECK_NULL_WF( p##CfgName, "CGGTinyXml::FindChildElementByName %s Failed", #CfgName ); \
	( stCfg ).m_d##CfgName = CGGString::SafeAtoDouble( p##CfgName->GetText() )

#define TIXML_GET_INT( stCfg, CfgName, pTiXmlCfg )\
	TiXmlElement *p##CfgName = CGGTinyXml::FindChildElementByName(#CfgName, pTiXmlCfg); \
	CHECK_NULL_WF(p##CfgName, "CGGTinyXml::FindChildElementByName %s Failed", #CfgName); \
	(stCfg).m_i##CfgName = CGGString::SafeAtoi(p##CfgName->GetText())

#define TIXML_GET_INT_DEFAULT( stCfg, CfgName, pTiXmlCfg, iDefVal )\
	TiXmlElement *p##CfgName = CGGTinyXml::FindChildElementByName(#CfgName, pTiXmlCfg); \
	CHECK_NULL_NRT(p##CfgName, "CGGTinyXml::FindChildElementByName %s Failed", #CfgName); \
	if (p##CfgName) \
	{ \
		(stCfg).m_i##CfgName = CGGString::SafeAtoi(p##CfgName->GetText()); \
	}\
		else\
	{\
		(stCfg).m_i##CfgName = (iDefVal); \
	}

#define TIXML_GET_STR( stCfg, CfgName, pTiXmlCfg )\
	TiXmlElement *p##CfgName = CGGTinyXml::FindChildElementByName(#CfgName, pTiXmlCfg); \
	CHECK_NULL_WF(p##CfgName, "CGGTinyXml::FindChildElementByName %s Failed", #CfgName); \
	STRCPYM((stCfg).m_sz##CfgName, p##CfgName->GetText())

#define TIXML_GET_STR_DEFAULT( stCfg, CfgName, pTiXmlCfg, szDefaultStr )\
	TiXmlElement *p##CfgName = CGGTinyXml::FindChildElementByName(#CfgName, pTiXmlCfg); \
	CHECK_NULL_NRT(p##CfgName, "CGGTinyXml::FindChildElementByName %s Failed", #CfgName); \
	if (p##CfgName) \
	{ \
		STRCPYM((stCfg).m_sz##CfgName, p##CfgName->GetText()); \
	}\
				else\
	{\
		STRCPYM((stCfg).m_sz##CfgName, szDefaultStr); \
	}



#define TIXML_ATTR_GET_STR( stCfg, CfgName, pTiXmlCfg )\
	TiXmlAttribute *p##CfgName = CGGTinyXml::FindAttributeByName( #CfgName, pTiXmlCfg ); \
	CHECK_NULL_WF( p##CfgName, "CGGTinyXml::FindAttributeByName %s Failed", #CfgName ); \
	STRCPYM( ( stCfg ).m_sz##CfgName, p##CfgName->Value() )

#define TIXML_ATTR_GET_INT( stCfg, CfgName, pTiXmlCfg )\
	TiXmlAttribute *p##CfgName = CGGTinyXml::FindAttributeByName( #CfgName, pTiXmlCfg ); \
	CHECK_NULL_WF( p##CfgName, "CGGTinyXml::FindAttributeByName %s Failed", #CfgName ); \
	( stCfg ).m_i##CfgName = CGGString::SafeAtoi( p##CfgName->Value( ) )

#endif

