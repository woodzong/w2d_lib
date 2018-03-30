#ifndef __CWBMP_LOADER_H_1231823718231263
#define __CWBMP_LOADER_H_1231823718231263

#define BMP_MAX_WIDTH  10240
#define BMP_MAX_HEIGHT 10240

#include <windows.h> 

//only support 32bit BMP file

class CBMPLoader
{
public:
	CBMPLoader()
	{
		m_szData = NULL;
	}

	~CBMPLoader()
	{
		if( m_szData )
		{
			delete [] m_szData;
			m_szData = NULL;
		}
	}

	int Init(const char * szResBMPFile);

	int GetWidth(){ return m_iWidth; }
	int GetHeight(){ return m_iHeight; }
	short GetColorBit(){ return m_wColorBit; };

	DWORD GetColorData( int iLocalX, int iLocalY )
	{
		int iIndex = iLocalY * m_iWidth + iLocalX;
		return m_szData[iIndex];
	}

	DWORD * GetData( int iLocalX, int iLocalY )
	{
		int iIndex = iLocalY * m_iWidth + iLocalX;
		return m_szData+iIndex;
	}

protected:
	int m_iWidth;
	int m_iHeight;
	int m_iDataSize;
	short m_wColorBit;
	DWORD * m_szData;
};


#endif