#ifndef CWTIMEPASS_H_123819287asdasdasd
#define CWTIMEPASS_H_123819287asdasdasd

#include <windows.h>
#include "CSingletonT.h"

class CWTimePass{
public:
	CWTimePass(){
		LARGE_INTEGER lv;
		QueryPerformanceFrequency( &lv );
		m_fSecsPerTick = (float)lv.QuadPart;
		QueryPerformanceCounter(&m_lvPreTime);
	}

	float GetUsedTime(){
		float flTmp = GetUsedTimePart();
		ResetPreTimePart();
		return flTmp;
	}

	float GetUsedTimePart(){
		LARGE_INTEGER   currentTime;   
		QueryPerformanceCounter(&currentTime);   
		return (float)(currentTime.QuadPart - m_lvPreTime.QuadPart) / m_fSecsPerTick;		
	}	

	void ResetPreTimePart(){
		QueryPerformanceCounter(&m_lvPreTime);
	}

public:
	LARGE_INTEGER m_lvPreTime; 
	float m_fSecsPerTick;
};
//----------------!>

typedef CSingletonT<CWTimePass> CWTimePassSingleton;
#define GTIMEPASS CWTimePassSingleton::Instance()

#endif