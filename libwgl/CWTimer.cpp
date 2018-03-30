#include "CWTimer.h"

CWTimer::CWTimer():m_bIsInited(false){
}

void CWTimer::Init(){
	memset(&m_stTimer, 0, sizeof(m_stTimer));

	//声明:
	//QueryPerformanceFrequency(
	//  var lpFrequency: TLargeInteger {此参数获取定时器每秒的频率数; TLargeInteger = Int64}
	//): BOOL;                         {返回 False 表示硬件不支持高性能定时器}


	// Check To See If A Performance Counter Is Available
	// If One Is Available The m_stTimer Frequency Will Be Updated
	if ( !QueryPerformanceFrequency((LARGE_INTEGER *) &m_stTimer.frequency) )
	{
		// No Performace Counter Available
		m_stTimer.performance_timer	= FALSE;					// Set Performance m_stTimer To FALSE
		m_stTimer.mm_timer_start	= timeGetTime();			// Use timeGetTime() To Get Current Time
		m_stTimer.resolution		= 1.0f/1000.0f;				// Set Our m_stTimer Resolution To .001f
		m_stTimer.frequency			= 1000;						// Set Our m_stTimer Frequency To 1000
		m_stTimer.mm_timer_elapsed	= m_stTimer.mm_timer_start;		// Set The Elapsed Time To The Current Time
	}
	else
	{
		//QueryPerformanceCounter 返回定时器当前计数值.
		// Performance Counter Is Available, Use It Instead Of The Multimedia m_stTimer
		// Get The Current Time And Store It In performance_timer_start
		QueryPerformanceCounter((LARGE_INTEGER *) &m_stTimer.performance_timer_start);
		m_stTimer.performance_timer			= TRUE;				// Set Performance m_stTimer To TRUE
		// Calculate The m_stTimer Resolution Using The m_stTimer Frequency
		m_stTimer.resolution				= (float) (((double)1.0f)/((double)m_stTimer.frequency));
		// Set The Elapsed Time To The Current Time
		m_stTimer.performance_timer_elapsed	= m_stTimer.performance_timer_start;
	}

	m_bIsInited = true;
}



float CWTimer::GetTime()										// Get Time In Milliseconds
{
	if(m_bIsInited == false){
		return 0.0f;
	}

	__int64 time;											// time Will Hold A 64 Bit Integer

	if (m_stTimer.performance_timer)							// Are We Using The Performance m_stTimer?
	{
		QueryPerformanceCounter((LARGE_INTEGER *) &time);	// Grab The Current Performance Time
		// Return The Current Time Minus The Start Time Multiplied By The Resolution And 1000 (To Get MS)
		return ( (float) ( time - m_stTimer.performance_timer_start) * m_stTimer.resolution)*1000.0f;
	}
	else
	{
		// Return The Current Time Minus The Start Time Multiplied By The Resolution And 1000 (To Get MS)
		return( (float) ( timeGetTime() - m_stTimer.mm_timer_start) * m_stTimer.resolution)*1000.0f;
	}
}