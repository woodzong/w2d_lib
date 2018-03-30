#include <fstream>
#include "CWoodLog.h"
#include "CWBMPLoader.h"

using namespace std;

//only support 32 bits;
int CBMPLoader::Init(const char * szResBMPFile)
{
	ifstream file( szResBMPFile, ios::binary );
	if( file.is_open() == false )
	{
		CWOOD_LOG_ERR(-1,"BMP_File open failed BMP_File:%s",szResBMPFile);
		return -1; 
	}

	file.seekg( 2,ios::beg );

	file.read((char *)&m_iDataSize, sizeof(int));

	int iDataOffSet = 0;
	file.seekg( 10,ios::beg );
	file.read((char *)&iDataOffSet, sizeof(int));

	file.seekg( 18,ios::beg );
	file.read((char *)&m_iWidth, sizeof(int));

	file.seekg( 22,ios::beg );
	file.read((char *)&m_iHeight, sizeof(int));

	file.seekg( 28,ios::beg );
	file.read((char *)&m_wColorBit, sizeof(short));

	int iDataSize = m_iDataSize - iDataOffSet;
	iDataSize = iDataSize/sizeof(DWORD) + 1;

	
	DWORD * m_szTmpData = new DWORD[ iDataSize ];
	file.seekg( iDataOffSet,ios::beg );
	file.read( (char *)m_szTmpData, m_iDataSize - iDataOffSet );

	m_szData = new DWORD[ iDataSize ];
	//rever row data now
	for( int i=0; i<m_iHeight; i++ )
	{
		int iSourceRow = i;
		int iTargetRow = m_iHeight - i - 1;
		memcpy( (char *)(m_szData+iSourceRow*m_iWidth), (char *)(m_szTmpData+iTargetRow*m_iWidth), m_iWidth*sizeof(DWORD) );
	}	
	delete [] m_szTmpData;

	CWOOD_LOG_DEBUG("Load %s Finished, Size:%d Width:%d Height:%d OffSet:%d ColorBit:%d DataSizeInDWORD:%d DataSizeInChar:%d",
		            szResBMPFile,m_iDataSize, m_iWidth, m_iHeight, iDataOffSet, m_wColorBit, iDataSize, m_iDataSize - iDataOffSet );

	file.close();

	return 0;
}