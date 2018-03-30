#ifndef CTIMER_H_ASDASD8973912846123619236123
#define CTIMER_H_ASDASD8973912846123619236123

#include <windows.h>
#include "CSingletonT.h"

typedef struct{
	__int64       frequency;									// m_stWoodTimer Frequency
	float         resolution;									// m_stWoodTimer Resolution
	unsigned long mm_timer_start;								// Multimedia m_stWoodTimer Start Value
	unsigned long mm_timer_elapsed;							// Multimedia m_stWoodTimer Elapsed Time
	bool		  performance_timer;							// Using The Performance m_stWoodTimer?
	__int64       performance_timer_start;					// Performance m_stWoodTimer Start Value
	__int64       performance_timer_elapsed;					// Performance m_stWoodTimer Elapsed Time
}ST_TIMER;


class CWTimer{
public:
	CWTimer();
	void Init();
	float GetTime();

private:
	bool m_bIsInited;
	ST_TIMER m_stTimer;
};



#endif