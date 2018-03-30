#ifndef WDEF_H_12314asdfas123123
#define WDEF_H_12314asdfas123123
#include <stdint.h>
#include <assert.h>
#include <stdio.h>

#define TRANSPARENT_COLOR   0xFFFFFF
#define TRANSPARENT_COLOR_32BIT   0xFFFFFFFF

#define W_LP_BUFFER void*

#define WOOD_ERR_INFO_FORMAT        "FILE:%s LN:%d FUN:%s RET:%d EM:"
#define WOOD_ERR_INFO_FORMAT_LONG        "FILE:%s LN:%d FUN:%s RET:%lu EM:"
#define WOOD_DEBUG_INFO_FORMAT      "FILE:%s LN:%d FUN:%s INFO:"
#define WOOD_DEBUG_INFO_CONTENT     __FILE__,__LINE__,__FUNCTION__

#define W_SNPRINTF _vsnprintf_s

#define W_MAKE_ERROR_MSG_CHAR_INFO(sErrMsg,iRetCode,sformat,...)_snprintf_s( sErrMsg, sizeof(sErrMsg),sizeof(sErrMsg),"FILE:%s LN:%d FUN:%s RET:%d EM:"sformat,__FILE__,__LINE__,__FUNCTION__,iRetCode,__VA_ARGS__)
#define W_PRINT_ERROR_MSG_CHAR_INFO(iRetCode,sformat,...)printf( "FILE:%s LN:%d FUN:%s RET:%d EM:"sformat,__FILE__,__LINE__,__FUNCTION__,iRetCode,__VA_ARGS__)
#define W_BOX_ALERT_CHAR_INFO(iRetCode,sformat,...)do{\
	char sBuffer[1024];\
	W_MAKE_ERROR_MSG_CHAR_INFO(sBuffer,iRetCode,sformat,__VA_ARGS__);\
	MessageBox(NULL,sBuffer,"ERROR!!",MB_OK|MB_ICONEXCLAMATION);\
}while( 0 )


#define KEYDOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)

// initializes a direct draw struct, basically zeros it and sets the dwSize field
#define DDRAW_INIT_STRUCT(ddstruct){ memset(&ddstruct,0,sizeof(ddstruct)); ddstruct.dwSize=sizeof(ddstruct); }

#define _RGB16BIT555(r,g,b) ((b & 31) + ((g & 31) << 5) + ((r & 31) << 10))
// this builds a 16 bit color value in 5.6.5 format (green dominate mode)
#define _RGB16BIT565(r,g,b) ((b & 31) + ((g & 63) << 5) + ((r & 31) << 11))
// this builds a 32 bit color value in A.8.8.8 format (8-bit alpha mode)
#define _RGB32BIT(a,r,g,b) ((b) + ((g) << 8) + ((r) << 16) + ((a) << 24))
#define RGB32BIT(r,g,b) ((b) + ((g) << 8) + ((r) << 16) + (0 << 24))

// bit manipulation macros
#define W_SET_BIT(word,bit_flag)   ((word)=((word) | (bit_flag)))
#define W_CLEAR_BIT(word,bit_flag) ((word)=((word) & (~bit_flag)))
#define W_CHECK_BIT( word, bit_flag ) ((word) & (bit_flag))

typedef enum
{
	WCOLOR_RED = RGB32BIT( 255, 0, 0 ),
	WCOLOR_GREEN = RGB32BIT( 0, 255, 0 ),
	WCOLOR_BLUE = RGB32BIT( 0, 0, 255 ),
	WCOLOR_YELLOW = RGB32BIT( 255, 255, 0 ),
	WCOLOR_DEEPGREEN = RGB32BIT( 0, 175, 45 ),
	WCOLOR_DEEPBLUE = RGB32BIT( 35, 63, 167 ),
	WCOLOR_GRAY = RGB32BIT( 200, 200, 200 ),
}WCOLOR;

typedef enum
{
	EN_WDRAW_SIZE_1 = 1,
	EN_WDRAW_SIZE_2 = 2,
	EN_WDRAW_SIZE_3 = 3,
	EN_WDRAW_SIZE_4 = 4,
}EN_WDRAW_SIZE;

typedef enum
{
	EN_WGKEY_NULL = 0,
	EN_WGKEY_UP = 1,
	EN_WGKEY_DOWN = 2,
	EN_WGKEY_LEFT = 3,
	EN_WGKEY_RIGHT = 4,
	EN_WGKEY_NUM0 = 5,
	EN_WGKEY_NUM1 = 6,
	EN_WGKEY_NUM2 = 7,
	EN_WGKEY_NUM3 = 8,
	EN_WGKEY_NUM4 = 9,
	EN_WGKEY_NUM5 = 10,
	EN_WGKEY_NUM6 = 11,
	EN_WGKEY_NUM7 = 12,
	EN_WGKEY_NUM8 = 13,
	EN_WGKEY_NUM9 = 14,

	EN_WGKEY_F11 = 15,
	EN_WGKEY_F12 = 16,
}EN_WGKEY;

#endif