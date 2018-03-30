#include "ggtinyxml.h"

bool CGGTinyXml::s_bIsAllowDuplicateChildName = false;

TiXmlElement * CGGTinyXml::FindChildElementByName( const char * szName, TiXmlElement * pTiXmlElement )
{
	TiXmlElement *pTiXmlTarget = pTiXmlElement->FirstChildElement();

    while( pTiXmlTarget )
    {
		if( _stricmp( pTiXmlTarget->Value( ), szName ) == 0 )
    	{
    		return pTiXmlTarget;
    	}

    	pTiXmlTarget = pTiXmlTarget->NextSiblingElement();
    }

    return NULL;
}

TiXmlElement * CGGTinyXml::GGGetRoot(TiXmlDocument * pTiXmlDoc, const char* pszRootName /*= "GSCfg"*/)
{
	TiXmlElement * pRoot = CGGTinyXml::FindSiblingElementByName(pszRootName, pTiXmlDoc->RootElement());
	if (!pRoot)
	{
		return NULL;
	}
	return pRoot->FirstChildElement();
}


TiXmlElement * CGGTinyXml::FindSiblingElementByName( const char * szName, TiXmlElement * pTiXmlElement )
{
	TiXmlElement *pTiXmlTarget = pTiXmlElement;

    while( pTiXmlTarget )
    {
		if( _stricmp( pTiXmlTarget->Value( ), szName ) == 0 )
    	{
    		return pTiXmlTarget;
    	}

    	pTiXmlTarget = pTiXmlTarget->NextSiblingElement();
    }

    return NULL;	
}


int CGGTinyXml::CheckOneDuplicateChildName( TiXmlElement * pChildTiXmlElement )
{
	if( pChildTiXmlElement == NULL )
	{
		return 0;
	}

	const char * szChildName = pChildTiXmlElement->Value( );
	pChildTiXmlElement = pChildTiXmlElement->NextSiblingElement( );

	//printf( "CheckIng Name:%s\n", szChildName );

	while( pChildTiXmlElement != NULL )
	{
		if( _stricmp( pChildTiXmlElement->Value( ), szChildName ) == 0 )
		{
			printf( "==Duilplcate Child Name ERROR_I:%s %s\n", szChildName, pChildTiXmlElement->Value( ) );
			return -1;
		}

		pChildTiXmlElement = pChildTiXmlElement->NextSiblingElement( );
	}

	return 0;
}


int CGGTinyXml::CheckDuplicateChildName( TiXmlElement * pFirstChildTiXmlElement )
{
	if( s_bIsAllowDuplicateChildName == true )
	{
		return 0;
	}

	if( pFirstChildTiXmlElement == NULL )
	{
		return -1;
	}

	int iRetCode = 0;

	TiXmlElement *pTiXmlChild = pFirstChildTiXmlElement;
	
	while( pTiXmlChild != NULL )
	{
		//printf( "CheckIng FName:%s\n", pTiXmlChild->Value() );

		if( pTiXmlChild->FirstChildElement() != NULL )
		{
			iRetCode = CheckDuplicateChildName( pTiXmlChild->FirstChildElement( ) );
			if( iRetCode != 0 )
			{
				printf( "==Duplicate Child Name ERROR In:%s\n", pTiXmlChild->Value( ) );
				return iRetCode;
			}
		}

		iRetCode = CheckOneDuplicateChildName( pTiXmlChild );
		if( iRetCode != 0 )
		{
			printf( "==CheckOneDuplicateChildName ERROR:%s\n", pTiXmlChild->Value( ) );
			return iRetCode;
		}

		pTiXmlChild = pTiXmlChild->NextSiblingElement( );
	}

	return 0;
}


TiXmlAttribute * CGGTinyXml::FindAttributeByName( const char * szName, TiXmlElement * pTiXmlElement )
{
	TiXmlAttribute *pAttr = pTiXmlElement->FirstAttribute( );

	while( pAttr )
	{
		if( _stricmp( pAttr->Name( ), szName ) == 0 )
		{
			return pAttr;
		}

		pAttr = pAttr->Next( );
	}

	return NULL;
}

const char * ConvToStringINT( int iValue )
{
	static char s_szTmp[12];
	_snprintf_s( s_szTmp, sizeof( s_szTmp ), "%d", iValue );
	return s_szTmp;
}

int	CGGTinyXml::CheckFormat(const std::string & strFile , std::string & strErrMsg)
{
	TiXmlDocument xmlCfg(strFile.c_str());
	if (xmlCfg.LoadFile())
	{
		return 0;
	}
	strErrMsg = "ErrorDesc:[";
	strErrMsg += xmlCfg.ErrorDesc();
	strErrMsg += "][Row:";
	strErrMsg += ConvToStringINT(xmlCfg.ErrorRow());
	strErrMsg += ", Col:";
	strErrMsg += ConvToStringINT(xmlCfg.ErrorCol());
	strErrMsg += "]\n";
	return -1;
}
