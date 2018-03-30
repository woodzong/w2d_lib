#ifndef GS_ASTART_H_123123123123
#define GS_ASTART_H_123123123123

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <limits.h>
#include <math.h>

#ifndef _GSDLL_
	#ifdef WIN32
	void DrawGridBlock( int iGridX, int iGridY, int iVal );
	#endif
#endif

#define WOOD_PRINT printf
//#define WOOD_PRINT

typedef enum _enMapBlockType_
{
	FREE_FOR_ALL = 0, //无阻挡
	STATIC_BLOCK = 1, //地图阻挡
	UNIT_G_BLOCK = 2, //单位 - 地面阻挡
	UNIT_A_BLOCK = 4, //单位 - 空中阻挡
	UNIT_T_BLOCK = 8, //单位 - 染色阻挡
}EN_MAP_BLOCK_TYPE;

typedef enum _enUnitType_
{
	UNIT_GROUND = 1, //地面单位
	UNIT_AIR = 2, //空中单位
	UNIT_FREE = 3, //无视阻挡单位
}EN_UNIT_TYPE;


/*
#define MAP_CELL_PARAM 2
#define MAP_WIDTH ( 800 / MAP_CELL_PARAM )
#define MAP_HEIGHT ( 800 / MAP_CELL_PARAM )
*/

#define PER_CELL ( 100 )

#define IS_TRACE 0


#define MAX_MAPDATA_NUM 255 //最大支持同时执行的地图实例...
typedef struct stMapData
{
	unsigned char uchIsUsing;
	int iPathBeautyDistSquare;
	int iMapWidth;
	int iMapHeight;
	unsigned char ** ppMapBlockData;
	//unsigned char ppMapBlockData[MAP_WIDTH][MAP_HEIGHT];
}ST_MAPDATA;

/*
typedef struct stMapBlock
{
	unsigned char aBlock[MAP_WIDTH][MAP_HEIGHT];
}ST_MAP_BLOCK;
*/

typedef struct _stVec2_
{
	int x;
	int y;
}Vec2;

#define MAX_RET_PATH_LEN 1024

typedef struct _stRetPath_
{
	int len;
	Vec2 astPoints[MAX_RET_PATH_LEN];
}ST_RET_PATH;

//实现C 寻路算法,方便测试验证
typedef struct stLineThroughResult
{
	int lastFreePosX;
	int lastFreePosY;
	int firstBlockPosX;
	int firstBlockPosY;
}ST_LINE_CHECK_RESULT;


#define U1_TMP_PARAM 1
//#define CHAR_BIT 8
//#define MAX_OPEN_LIST_NODE (250*U1_TMP_PARAM)
//#define MAX_CLOSE_LIST_NODE (400*U1_TMP_PARAM)
#define MAX_OPEN_LIST_NODE (125*U1_TMP_PARAM)
#define MAX_CLOSE_LIST_NODE (200*U1_TMP_PARAM)
#define FAILED_CHECK_MIN_CLOSE_LIST_NODE_NUM (100 * U1_TMP_PARAM)
#define FAILED_CHECK_BETWEEN_CLOSE_AND_OPEN_LIST_NODE_NUM (6 * U1_TMP_PARAM)
#define MAX_NODE_NUM_EVER_IN_OPEN_LIST (MAX_OPEN_LIST_NODE*2+1)

#define MAX_MAP_WIDTH 500
#define MAX_MAP_HEIGHT 500
#define MAX_GRID_ID ( MAX_MAP_WIDTH * MAX_MAP_HEIGHT )
#define MAX_G ( MAX_MAP_WIDTH * U1_TMP_PARAM )


typedef struct _stPathNode_
{
	int x;
	int y;
	int G;
	int father;
}ST_PATH_NODE;

typedef struct _stPathNodeList_
{
	ST_PATH_NODE node;
	int nextIdx;
}ST_PATH_NODE_LIST;

typedef struct _stPathNodeInList_
{
	ST_PATH_NODE node;
	long long seq64num;
}ST_PATH_NODE_IN_LIST;

typedef struct _stPathNodeIdx_
{
	short shIdx;
	long long seq64num;
}ST_PATH_NODE_IDX;

typedef struct _stListHeadReferToG_
{
	int headNodeIdxInList;
	long long seq64num;
}ST_LIST_HEAD_REFERTO_G;


typedef struct _stAStarData_
{
	long long minDist;
	unsigned char isFindNearPath;
	int minG;
	int maxG;
	unsigned char isInStaticBlock;

	long long seq64num;

	ST_PATH_NODE nearPathNode;

	int nodeListSize;
	ST_PATH_NODE_LIST nodeList[MAX_NODE_NUM_EVER_IN_OPEN_LIST];

	int openListSize;
	ST_PATH_NODE_IN_LIST openList[MAX_OPEN_LIST_NODE];

	int closeListSize;
	ST_PATH_NODE_IN_LIST closeList[MAX_CLOSE_LIST_NODE];

	int freeOpenListNodeIdxSize;
	ST_PATH_NODE_IDX freeOpenListNodeIdx[MAX_OPEN_LIST_NODE];

	short shOpenCloseListIndex[MAX_GRID_ID];

	ST_LIST_HEAD_REFERTO_G listHeadReferToG[MAX_G];

	char unitType;
	char unitSize;
}ST_ASTAR_DATA;

typedef struct _stPathFindCfg
{
    int pathBeautyDistSquare;
}ST_PATH_FIND_CFG;


ST_MAPDATA * GetMapData( int iMapIndex );

int NewOneMap( int iMapWidth, int iMapHeight, int iPathBeautyDistSquare );

void ReleaseMap( int iMapIndex );

int FindPathAll( int iMapIndex, const Vec2 * pPosBegin, const Vec2 * pPosEnd, int stopDist, char unitSize, char unitType, ST_RET_PATH * pstRetPath );

int FindPathByAStar( int iMapIndex, const Vec2 * pPosBegin, const Vec2 * pPosEnd, int stopDist, char unitSize, char unitType, ST_RET_PATH * pstRetPath );

unsigned char GetMapBlockVal( int iMapIndex, unsigned short x, unsigned short y );

void SetMapBlockVal( int iMapIndex, unsigned short x, unsigned short y, unsigned char val );

void SetGridBlock( int iMapIndex, int posX, int posY, int unitSize, char unitType );

void ClearGridBlock( int iMapIndex, int posX, int posY, int unitSize, char unitType );

unsigned char IsPointFree( int iMapIndex, int posX, int posY, char unitSize, char unitType );

unsigned char IsPointInStaticBlock( int iMapIndex, int posX, int posY, char unitSize, char unitType );

void InitTestMapBlock( int iMapIndex );

int FindPathAllForCS( int iMapIndex, int posBeginX, int posBeginY, int posEndX, int posEndY, int radius, int unitSize, char unitType, ST_RET_PATH *output );

unsigned char IsLineThrough( int posBeginX, int posBeginY, int posEndX, int posEndY, char unitSize, char unitType, ST_LINE_CHECK_RESULT * pOutRetPos );
#endif

