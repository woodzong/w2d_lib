#define WIN32_LEAN_AND_MEAN  

#include <time.h>
#include <stdarg.h>
#include <windows.h>
#include "CWoodLog.h"

using namespace wood::comm::log;

CWoodLog::CWoodLog(){
	m_strLogFileName = "";
	m_strLogFilePath = "";
	memset(m_sLastErrMsg,0,sizeof(m_sLastErrMsg));
	m_pLogDebugFile = NULL;
	m_pLogRunFile   = NULL;
	m_pLogErrFile   = NULL;
	m_pLogKeyFile   = NULL;
	m_pLogPerformFile   = NULL;
	m_iRetCode = 0;
	m_bIsNeedLog = true;
}

int32_t CWoodLog::Init(const char * sLogFileName, const char * sLogFilePath,bool bIsNeedLog){
	m_bIsNeedLog = bIsNeedLog;
	m_strLogFileName = sLogFileName;
	m_strLogFilePath = sLogFilePath;
	std::string strTmpPath = "";

	if( m_pLogDebugFile != NULL ){
		m_iRetCode = -1;
		W_MAKE_ERROR_MSG_CHAR_INFO(m_sLastErrMsg,m_iRetCode,"CWoodLog Has Already Inited Before,No Need Init Again.");
		return m_iRetCode;
	}	
	strTmpPath = m_strLogFilePath+"\\"+m_strLogFileName+".debug.log";
	
	m_iRetCode = fopen_s(&m_pLogDebugFile,strTmpPath.c_str(),"a+");
	if( m_iRetCode != 0 ){
		m_pLogDebugFile = NULL;
		W_MAKE_ERROR_MSG_CHAR_INFO(m_sLastErrMsg,m_iRetCode,"fopen_s Failed File:%s",strTmpPath.c_str());
		return m_iRetCode;
	}

	strTmpPath = m_strLogFilePath+"\\"+m_strLogFileName+".run.log";	
	m_iRetCode = fopen_s(&m_pLogRunFile,strTmpPath.c_str(),"a+");
	if( m_iRetCode != 0 ){
		m_pLogRunFile = NULL;
		W_MAKE_ERROR_MSG_CHAR_INFO(m_sLastErrMsg,m_iRetCode,"fopen_s Failed File:%s",strTmpPath.c_str());
		return m_iRetCode;
	}

	strTmpPath = m_strLogFilePath+"\\"+m_strLogFileName+".err.log";	
	m_iRetCode = fopen_s(&m_pLogErrFile,strTmpPath.c_str(),"a+");
	if( m_iRetCode != 0 ){
		m_pLogErrFile = NULL;
		W_MAKE_ERROR_MSG_CHAR_INFO(m_sLastErrMsg,m_iRetCode,"fopen_s Failed File:%s",strTmpPath.c_str());
		return m_iRetCode;
	}

	strTmpPath = m_strLogFilePath+"\\"+m_strLogFileName+".perform.log";	
	m_iRetCode = fopen_s(&m_pLogPerformFile,strTmpPath.c_str(),"a+");
	if( m_iRetCode != 0 ){
		m_pLogPerformFile = NULL;
		W_MAKE_ERROR_MSG_CHAR_INFO(m_sLastErrMsg,m_iRetCode,"fopen_s Failed File:%s",strTmpPath.c_str());
		return m_iRetCode;
	}

	strTmpPath = m_strLogFilePath+"\\"+m_strLogFileName+".key.log";	
	m_iRetCode = fopen_s(&m_pLogKeyFile,strTmpPath.c_str(),"a+");
	if( m_iRetCode != 0 ){
		m_pLogKeyFile = NULL;
		W_MAKE_ERROR_MSG_CHAR_INFO(m_sLastErrMsg,m_iRetCode,"fopen_s Failed File:%s",strTmpPath.c_str());
		return m_iRetCode;
	}

	fclose(m_pLogKeyFile);
	m_pLogKeyFile = NULL;

	fclose(m_pLogPerformFile);
	m_pLogPerformFile = NULL;

	fclose(m_pLogErrFile);
	m_pLogErrFile = NULL;

	fclose(m_pLogRunFile);
	m_pLogRunFile = NULL;

	fclose(m_pLogDebugFile);
	m_pLogDebugFile = NULL;

	return 0;
}

int32_t CWoodLog::GetTimeString(char * sOutBuffer, int32_t iBufferSize){
	struct tm stTime;
	time_t tNow;
	time(&tNow);
    localtime_s(&stTime,&tNow);
	_snprintf_s(sOutBuffer,iBufferSize,iBufferSize,"%04d-%02d-%02d %02d:%02d:%02d",
                stTime.tm_year+1900, stTime.tm_mon+1, stTime.tm_mday,
                stTime.tm_hour, stTime.tm_min, stTime.tm_sec);
	return 0;
}

const char * CWoodLog::GetLastErrMsg(){ 
	return m_sLastErrMsg;
};

void CWoodLog::LogRunMsg(__format_string const char * sFormat,...){
	CWOOD_LOG_CHECK_NEED();
	va_list ap;	
	va_start(ap, sFormat);
	LogMsg(W_LOG_RUN_TYPE,"RUN",sFormat,ap);
	LogMsg(W_LOG_DEBUG_TYPE,"RUN",sFormat,ap);
	va_end(ap);
}

void CWoodLog::LogDebugMsg(__format_string const char * sFormat,...){
	CWOOD_LOG_CHECK_NEED();
	va_list ap;	
	va_start(ap, sFormat);
	LogMsg(W_LOG_DEBUG_TYPE,"DEG",sFormat,ap);
	va_end(ap);
}

void CWoodLog::LogErrMsg(__format_string const char * sFormat,...){
	CWOOD_LOG_CHECK_NEED();
	va_list ap;	
	va_start(ap, sFormat);
	LogMsg(W_LOG_ERR_TYPE,"ERR",sFormat,ap);
	LogMsg(W_LOG_DEBUG_TYPE,"ERR",sFormat,ap);
	va_end(ap);
}

void CWoodLog::LogPerformMsg(__format_string const char * sFormat,...){
	CWOOD_LOG_CHECK_NEED();
	va_list ap;	
	va_start(ap, sFormat);
	LogMsg(W_LOG_PERFORM_TYPE,"PER",sFormat,ap);
	LogMsg(W_LOG_DEBUG_TYPE,"PER",sFormat,ap);
	va_end(ap);
}

void CWoodLog::LogKeyMsg(__format_string const char * sFormat,...){	
	CWOOD_LOG_CHECK_NEED();
	va_list ap;	
	va_start(ap, sFormat);
	LogMsg(W_LOG_KEY_TYPE,"KEY",sFormat,ap);
	LogMsg(W_LOG_DEBUG_TYPE,"KEY",sFormat,ap);
	va_end(ap);
}

void CWoodLog::LogMsg(W_LOG_TYPE iType,const char * sPrefix,__format_string const char * sFormat,va_list ap){
	CWOOD_LOG_CHECK_NEED();
	FILE * pLogFile = NULL;
	char sFileTypeName[25] = {0};

	switch(iType){
		case W_LOG_RUN_TYPE:
			_snprintf_s( sFileTypeName, sizeof(sFileTypeName),sizeof(sFileTypeName),"run");
			pLogFile = m_pLogRunFile;
			break;
		case W_LOG_ERR_TYPE:
			_snprintf_s( sFileTypeName, sizeof(sFileTypeName),sizeof(sFileTypeName),"err");
			pLogFile = m_pLogErrFile;
			break;
		case W_LOG_PERFORM_TYPE:
			_snprintf_s( sFileTypeName, sizeof(sFileTypeName),sizeof(sFileTypeName),"perform");
			pLogFile = m_pLogPerformFile;
			break;
		case W_LOG_KEY_TYPE:
			_snprintf_s( sFileTypeName, sizeof(sFileTypeName),sizeof(sFileTypeName),"key");
			pLogFile = m_pLogKeyFile;
			break;
		case W_LOG_DEBUG_TYPE:
		default:
			_snprintf_s( sFileTypeName, sizeof(sFileTypeName),sizeof(sFileTypeName),"debug");
			pLogFile = m_pLogDebugFile;
			break;
	}

	std::string strTmpPath = "";
	strTmpPath = m_strLogFilePath+"\\"+m_strLogFileName+"."+sFileTypeName+".log";	
	m_iRetCode = fopen_s(&pLogFile,strTmpPath.c_str(),"a+");
	if( m_iRetCode != 0 ){
		pLogFile = NULL;
		W_MAKE_ERROR_MSG_CHAR_INFO(m_sLastErrMsg,m_iRetCode,"fopen_s Failed File:%s",strTmpPath.c_str());
		return ;
	}

	char sTimeBuffer[20] = {0};  //2009-12-10 10:11:12
	GetTimeString(sTimeBuffer,sizeof(sTimeBuffer));
	fprintf(pLogFile,"%s | %s TID:%d PID:%d ",sTimeBuffer,sPrefix,GetCurrentProcessId(),GetCurrentThreadId()); //print time info;
	vfprintf(pLogFile,sFormat,ap);
	fprintf(pLogFile,"\n");	
	fclose(pLogFile);
}
