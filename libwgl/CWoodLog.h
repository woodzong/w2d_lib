#ifndef CW_LOG_COMM_H_12314asdfas123123
#define CW_LOG_COMM_H_12314asdfas123123

#include <string>
#include <stdio.h>
#include "CSingletonT.h"
#include "WDefine.h"


#define C_WOOD_LOG_FILE_DEFAULT_PATH "C:\\CWLOG"


enum W_LOG_TYPE{
	W_LOG_RUN_TYPE       =0,
	W_LOG_DEBUG_TYPE     =1,
	W_LOG_ERR_TYPE       =2,
	W_LOG_PERFORM_TYPE   =3,
	W_LOG_KEY_TYPE       =4
};

namespace wood{
	namespace comm{
		namespace log{



			class CWoodLog{

			public:	
				CWoodLog();

				int32_t Init(const char * sLogFileName, const char * sLogFilePath=C_WOOD_LOG_FILE_DEFAULT_PATH, bool bIsNeedLog=true);

				void LogRunMsg(__format_string const char * sFormat,...);

				void LogDebugMsg(__format_string const char * sFormat,...);

				void LogErrMsg(__format_string const char * sFormat,...);

				void LogPerformMsg(__format_string const char * sFormat,...);

				void LogKeyMsg(__format_string const char * sFormat,...);

				const char * GetLastErrMsg();

			private:

				void LogMsg(W_LOG_TYPE iType,const char * sPrefix,__format_string const char * sFormat,va_list ap);

				int32_t GetTimeString(char * sOutBuffer, int32_t iBufferSize);

			private:
				bool m_bIsNeedLog;
				std::string m_strLogFileName;
				std::string m_strLogFilePath;
				char m_sLastErrMsg[255];
				int32_t m_iRetCode;
				FILE * m_pLogDebugFile;
				FILE * m_pLogRunFile;
				FILE * m_pLogErrFile;
				FILE * m_pLogKeyFile;
				FILE * m_pLogPerformFile;
			};
		}
	}
}

typedef CSingletonT<wood::comm::log::CWoodLog> CWoodLogSingleton;

#define CWOOD_OLOG CWoodLogSingleton::Instance()

#define CWOOD_LOG_CHECK_NEED() if(m_bIsNeedLog==false){return;}

#define CWOOD_LOG(sformat,...)do{CWOOD_OLOG->LogRunMsg(sformat,__VA_ARGS__);}while(0)

#define CWOOD_LOG_ERR(iErrCode,sformat,...)do{CWOOD_OLOG->LogErrMsg(WOOD_ERR_INFO_FORMAT sformat,WOOD_DEBUG_INFO_CONTENT,iErrCode,__VA_ARGS__);}while(0)

#define CWOOD_LOG_ERR_L(lErrCode,sformat,...)do{CWOOD_OLOG->LogErrMsg(WOOD_ERR_INFO_FORMAT_LONG sformat,WOOD_DEBUG_INFO_CONTENT,lErrCode,__VA_ARGS__);}while(0)

#define CWOOD_LOG_DEBUG(sformat,...)do{CWOOD_OLOG->LogDebugMsg(WOOD_DEBUG_INFO_FORMAT sformat,WOOD_DEBUG_INFO_CONTENT,__VA_ARGS__);}while(0)

#define CWOOD_LOG_KEY(sformat,...)do{CWOOD_OLOG->LogKeyMsg(WOOD_DEBUG_INFO_FORMAT sformat,WOOD_DEBUG_INFO_CONTENT,__VA_ARGS__);}while(0)

#define CHECK_ERR( iRetCode, sformat, ... ) if(iRetCode != 0 ){  CWOOD_LOG_ERR(iRetCode,sformat,__VA_ARGS__); return iRetCode; }

#endif