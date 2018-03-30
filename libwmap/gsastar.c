#include "gsastar.h"


ST_MAPDATA g_stMapData[MAX_MAPDATA_NUM];
unsigned char g_ucIsGlobalMapDataInit = 0;

unsigned char ** GCUR_MAPBLOCK = NULL;
ST_MAPDATA * GCUR_MAPDATA = NULL;

ST_ASTAR_DATA gAStar;

#define MAX_G_INCAL ( GCUR_MAPDATA->iMapWidth * U1_TMP_PARAM )
#define MAX_GRID_ID_INCAL ( GCUR_MAPDATA->iMapWidth * GCUR_MAPDATA->iMapHeight )



#ifdef _GSWINDOWS_
void DebugLogToFileSlow( const char * szContent )
{
	FILE *fp = NULL;
	fp = fopen( "C:\\u1_fata_c.log", "a" );
	fputs( szContent, fp );
	fclose( fp );
}
#define SLOW_FATAL_LOG( format, ... )\
{\
	char szTmpBuff[255];\
	snprintf( szTmpBuff, sizeof(szTmpBuff), "Line:%d " format "\n", __LINE__, ##__VA_ARGS__ );\
	DebugLogToFileSlow( szTmpBuff );\
}

#else

#define SLOW_FATAL_LOG( format, ... ){}

#endif

ST_MAPDATA * GetMapData( int iMapIndex )
{
	if( iMapIndex < 0 || iMapIndex >= MAX_MAPDATA_NUM )
	{
		SLOW_FATAL_LOG( "Invalid MapIndex:%d", iMapIndex );
		return NULL;
	}

	if( g_stMapData[iMapIndex].uchIsUsing == 0 )
	{
		SLOW_FATAL_LOG( "Invalid MapIndex:%d isUsing False", iMapIndex )
		return NULL;
	}

	return &( g_stMapData[iMapIndex] );
}

void SetCurMapDataBlock_i( int iMapIndex )
{
	if( iMapIndex < 0 || iMapIndex >= MAX_MAPDATA_NUM )
	{
		SLOW_FATAL_LOG( "Invalid MapIndex:%d", iMapIndex );
		return;
	}

	if( g_stMapData[iMapIndex].uchIsUsing == 0 )
	{
		SLOW_FATAL_LOG( "Invalid MapIndex:%d isUsing False", iMapIndex )
		return;
	}

	GCUR_MAPDATA = &( g_stMapData[iMapIndex] );
	GCUR_MAPBLOCK = g_stMapData[iMapIndex].ppMapBlockData;
}


#define CACL_NODE_G( ax, ay, bx, by )(abs( ax - bx ) + abs( ay - by ) )
//#define CACL_NODE_G( ax, ay, bx, by )(  min( abs( ax - bx ) , abs( ay - by ) ) )


unsigned char IsPointInRect( int iPosX, int iPosY, int iCenterX, int iCenterY, int iWidth, int iLength, double angle, int iRadius, int iHeight )
{
    double s = sin( -angle );
    double c = cos( -angle );

    double offsetX = iPosX - iCenterX;
    double offsetY = iPosY - iCenterY;

    double transposX = offsetX * c - offsetY * s;
    double transposY = offsetX * s + offsetY * c;

    if( ( transposX > -iLength / 2 - iRadius )
        && ( transposX < iLength / 2 + iRadius )
        && ( transposY > -iWidth / 2 - iHeight / 2 )
        && ( transposY < iWidth / 2 + iHeight / 2 ) )
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void ReleaseMap( int iMapIndex )
{
	if( iMapIndex < 0 || iMapIndex >= MAX_MAPDATA_NUM )
	{
		return;
	}

	if( g_stMapData[iMapIndex].uchIsUsing == 0 )
	{
		return;
	}

	g_stMapData[iMapIndex].uchIsUsing = 0;

	unsigned char ** ppMapBlockData = g_stMapData[iMapIndex].ppMapBlockData;

	free( ppMapBlockData[0] );
	free( ppMapBlockData );
}


int NewOneMap( int iMapWidth, int iMapHeight, int iPathBeautyDistSquare )
{
	if( iMapWidth > MAX_MAP_WIDTH )
	{
		return -10;
	}

	if( iMapHeight > MAX_MAP_HEIGHT )
	{
		return -11;
	}

	if( g_ucIsGlobalMapDataInit == 0 )
	{
		g_ucIsGlobalMapDataInit = 1;
		memset( g_stMapData, 0, sizeof( g_stMapData ) );
	}

	//find one no used id;
	int iEmptyIndex = -1;
	int i = 0;
	for( i = 0; i < MAX_MAPDATA_NUM; i++ )
	{
		if( g_stMapData[i].uchIsUsing == 0 )
		{
			g_stMapData[i].uchIsUsing = 1;
			g_stMapData[i].iMapHeight = iMapHeight;
			g_stMapData[i].iMapWidth = iMapWidth;
			g_stMapData[i].iPathBeautyDistSquare = iPathBeautyDistSquare;
			iEmptyIndex = i;
			break;
		}
	}

	if( iEmptyIndex == -1 )
	{
		return -1;
	}

	unsigned char ** ppMapBlockData = malloc( sizeof( unsigned char * ) * iMapWidth );
	if( ppMapBlockData == NULL )
	{
		return -2;
	}

	ppMapBlockData[0] = malloc( sizeof( unsigned char ) * iMapWidth * iMapHeight );
	if( ppMapBlockData[0] == NULL )
	{
		return -3;
	}

	memset( ppMapBlockData[0], 0, sizeof( unsigned char ) * iMapWidth * iMapHeight );

	int j = 0;
	for( j = 1; j < iMapWidth; j++ )
	{
		ppMapBlockData[j] = ppMapBlockData[j-1] + iMapHeight;
	}

	g_stMapData[iEmptyIndex].ppMapBlockData = ppMapBlockData;

	return iEmptyIndex;
}

void SetMapBlockVal( int iMapIndex, unsigned short x, unsigned short y, unsigned char val )
{
	SetCurMapDataBlock_i( iMapIndex );

	if( x < 0 || x >= GCUR_MAPDATA->iMapWidth || y < 0 || y >=  GCUR_MAPDATA->iMapHeight )
	{
		return;
	}

	g_stMapData[iMapIndex].ppMapBlockData[x][y] = val;
}

unsigned char GetMapBlockVal( int iMapIndex, unsigned short x, unsigned short y )
{
	SetCurMapDataBlock_i( iMapIndex );

	if( x < 0 || x >= GCUR_MAPDATA->iMapWidth || y < 0 || y >= GCUR_MAPDATA->iMapHeight )
	{
		return STATIC_BLOCK;
	}

	return g_stMapData[iMapIndex].ppMapBlockData[x][y];
}

unsigned int GetGridWidth()
{
    return PER_CELL;
}

unsigned char IsValidGrid_i( unsigned short gridX, unsigned short gridY )
{
	if( gridX < 0 || gridX >= GCUR_MAPDATA->iMapWidth )
    {
        return 0;
    }

	if( gridY < 0 || gridY >= GCUR_MAPDATA->iMapHeight )
    {
        return 0;
    }

    return 1;
}

void PosToGrid( int posX, int posY, Vec2 *pVecOut )
{
    pVecOut->x = posX / PER_CELL;
    pVecOut->y = posY / PER_CELL;
}

unsigned char IG_s( int gridX, int gridY, char unitType )
{
	if( gridX < 0 || gridX >= GCUR_MAPDATA->iMapWidth )
    {
        return 1;
    }

	if( gridY < 0 || gridY >= GCUR_MAPDATA->iMapHeight )
    {
        return 1;
    }

	unsigned char val = GCUR_MAPBLOCK[gridX][gridY];

    if( val & STATIC_BLOCK )
    {
        return 1;
    }

    return 0;
}

unsigned char IG_i( int gridX, int gridY, char unitType )
{
	if( gridX < 0 || gridX >= GCUR_MAPDATA->iMapWidth )
    {
        return 0;
    }

	if( gridY < 0 || gridY >= GCUR_MAPDATA->iMapHeight )
    {
        return 0;
    }

#ifdef DAW_LINE_TEST
	GCUR_MAPBLOCK[gridX][gridY] = ( GCUR_MAPBLOCK[gridX][gridY] | UNIT_G_BLOCK );
    return 1;
#endif


	unsigned char val = GCUR_MAPBLOCK[gridX][gridY];

    if( val & STATIC_BLOCK )
    {
        return 0;
    }

    switch( unitType )
    {
        case UNIT_FREE:
            return 1;

        case UNIT_AIR:
            if( ( val & UNIT_A_BLOCK ) == 0 )
            {
                return 0;
            }

            break;

        case UNIT_GROUND:
            if( ( val & UNIT_G_BLOCK ) == 0 )
            {
                return 1;
            }

            break;

        default:
            break;
    }

    return 0;
}



void IG_b( int gridX, int gridY, char unitType )
{
	if( gridX < 0 || gridX >= GCUR_MAPDATA->iMapWidth )
    {
        return;
    }

	if( gridY < 0 || gridY >= GCUR_MAPDATA->iMapHeight )
    {
        return;
    }

	unsigned char val = GCUR_MAPBLOCK[gridX][gridY];

    switch( unitType )
    {
        case UNIT_FREE:
            return;

        case UNIT_AIR:
            if( ( val & UNIT_A_BLOCK ) == 0 )
            {
				GCUR_MAPBLOCK[gridX][gridY] = ( val | UNIT_A_BLOCK );
            }

            break;

        case UNIT_GROUND:
            if( ( val & UNIT_G_BLOCK ) == 0 )
            {
				GCUR_MAPBLOCK[gridX][gridY] = ( val | UNIT_G_BLOCK );
            }

            break;

        default:
            break;
    }

    return;
}




void IG_c( int gridX, int gridY, char unitType )
{
	if( gridX < 0 || gridX >= GCUR_MAPDATA->iMapWidth )
    {
        return;
    }

	if( gridY < 0 || gridY >= GCUR_MAPDATA->iMapHeight )
    {
        return;
    }

	unsigned char val = GCUR_MAPBLOCK[gridX][gridY];

    switch( unitType )
    {
        case UNIT_FREE:
            return;

        case UNIT_AIR:
            if( ( val & UNIT_A_BLOCK ) == UNIT_A_BLOCK )
            {
				GCUR_MAPBLOCK[gridX][gridY] = ( val & ( ~UNIT_A_BLOCK ) );
            }

            break;

        case UNIT_GROUND:
            if( ( val & UNIT_G_BLOCK ) == UNIT_G_BLOCK )
            {
				GCUR_MAPBLOCK[gridX][gridY] = ( val & ( ~UNIT_G_BLOCK ) );
            }

            break;

        default:
            break;
    }

    return;
}

unsigned char IsGridInStaticBlock_i( int gridX, int gridY, char unitSize, char unitType )
{
    if( unitSize == 1 )
    {
        return IG_s( gridX, gridY, unitType );
    }

    else if( unitSize == 2 )
        return IG_s( gridX - 1, gridY, unitType ) && IG_s( gridX, gridY, unitType ) &&
               IG_s( gridX - 1, gridY - 1, unitType ) && IG_s( gridX, gridY - 1, unitType );

    else if( unitSize == 3 )
        return IG_s( gridX - 1, gridY + 1, unitType ) && IG_s( gridX, gridY + 1, unitType ) && IG_s( gridX + 1, gridY + 1, unitType ) &&
               IG_s( gridX - 1, gridY, unitType ) && IG_s( gridX, gridY, unitType ) && IG_s( gridX + 1, gridY, unitType ) &&
               IG_s( gridX - 1, gridY - 1, unitType ) && IG_s( gridX, gridY - 1, unitType ) && IG_s( gridX + 1, gridY - 1, unitType );

    else if( unitSize == 4 )
        return IG_s( gridX - 2, gridY + 2, unitType ) && IG_s( gridX - 1, gridY + 2, unitType ) && IG_s( gridX, gridY + 2, unitType ) && IG_s( gridX + 1, gridY + 2, unitType ) &&
               IG_s( gridX - 2, gridY + 1, unitType ) && IG_s( gridX - 1, gridY + 1, unitType ) && IG_s( gridX, gridY + 1, unitType ) && IG_s( gridX + 1, gridY + 1, unitType ) &&
               IG_s( gridX - 2, gridY, unitType ) && IG_s( gridX - 1, gridY, unitType ) && IG_s( gridX, gridY, unitType ) && IG_s( gridX + 1, gridY, unitType ) &&
               IG_s( gridX - 2, gridY - 1, unitType ) && IG_s( gridX - 1, gridY - 1, unitType ) && IG_s( gridX, gridY - 1, unitType ) && IG_s( gridX + 1, gridY - 1, unitType );

    else if( unitSize == 5 )
        return IG_s( gridX - 2, gridY + 2, unitType ) && IG_s( gridX - 1, gridY + 2, unitType ) && IG_s( gridX, gridY + 2, unitType ) && IG_s( gridX + 1, gridY + 2, unitType ) && IG_s( gridX + 2, gridY + 2, unitType ) &&
               IG_s( gridX - 2, gridY + 1, unitType ) && IG_s( gridX - 1, gridY + 1, unitType ) && IG_s( gridX, gridY + 1, unitType ) && IG_s( gridX + 1, gridY + 1, unitType ) && IG_s( gridX + 2, gridY + 1, unitType ) &&
               IG_s( gridX - 2, gridY, unitType ) && IG_s( gridX - 1, gridY, unitType ) && IG_s( gridX, gridY, unitType ) && IG_s( gridX + 1, gridY, unitType ) && IG_s( gridX + 2, gridY, unitType ) &&
               IG_s( gridX - 2, gridY - 1, unitType ) && IG_s( gridX - 1, gridY - 1, unitType ) && IG_s( gridX, gridY - 1, unitType ) && IG_s( gridX + 1, gridY - 1, unitType ) && IG_s( gridX + 2, gridY - 1, unitType ) &&
               IG_s( gridX - 2, gridY - 2, unitType ) && IG_s( gridX - 1, gridY - 2, unitType ) && IG_s( gridX, gridY - 2, unitType ) && IG_s( gridX + 1, gridY - 2, unitType ) && IG_s( gridX + 2, gridY - 2, unitType );

    else if( unitSize == 6 )
        return IG_s( gridX - 3, gridY + 3, unitType ) && IG_s( gridX - 2, gridY + 3, unitType ) && IG_s( gridX - 1, gridY + 3, unitType ) && IG_s( gridX, gridY + 3, unitType ) && IG_s( gridX + 1, gridY + 3, unitType ) && IG_s( gridX + 2, gridY + 3, unitType ) &&
               IG_s( gridX - 3, gridY + 2, unitType ) && IG_s( gridX - 2, gridY + 2, unitType ) && IG_s( gridX - 1, gridY + 2, unitType ) && IG_s( gridX, gridY + 2, unitType ) && IG_s( gridX + 1, gridY + 2, unitType ) && IG_s( gridX + 2, gridY + 2, unitType ) &&
               IG_s( gridX - 3, gridY + 1, unitType ) && IG_s( gridX - 2, gridY + 1, unitType ) && IG_s( gridX - 1, gridY + 1, unitType ) && IG_s( gridX, gridY + 1, unitType ) && IG_s( gridX + 1, gridY + 1, unitType ) && IG_s( gridX + 2, gridY + 1, unitType ) &&
               IG_s( gridX - 3, gridY, unitType ) && IG_s( gridX - 2, gridY, unitType ) && IG_s( gridX - 1, gridY, unitType ) && IG_s( gridX, gridY, unitType ) && IG_s( gridX + 1, gridY, unitType ) && IG_s( gridX + 2, gridY, unitType ) &&
               IG_s( gridX - 3, gridY - 1, unitType ) && IG_s( gridX - 2, gridY - 1, unitType ) && IG_s( gridX - 1, gridY - 1, unitType ) && IG_s( gridX, gridY - 1, unitType ) && IG_s( gridX + 1, gridY - 1, unitType ) && IG_s( gridX + 2, gridY - 1, unitType ) &&
               IG_s( gridX - 3, gridY - 2, unitType ) && IG_s( gridX - 2, gridY - 2, unitType ) && IG_s( gridX - 1, gridY - 2, unitType ) && IG_s( gridX, gridY - 2, unitType ) && IG_s( gridX + 1, gridY - 2, unitType ) && IG_s( gridX + 2, gridY - 2, unitType );

    else if( unitSize == 7 )
        return  IG_s( gridX - 3, gridY + 3, unitType ) && IG_s( gridX - 2, gridY + 3, unitType ) && IG_s( gridX - 1, gridY + 3, unitType ) && IG_s( gridX, gridY + 3, unitType ) && IG_s( gridX + 1, gridY + 3, unitType ) && IG_s( gridX + 2, gridY + 3, unitType ) && IG_s( gridX + 3, gridY + 3, unitType ) &&
                IG_s( gridX - 3, gridY + 2, unitType ) && IG_s( gridX - 2, gridY + 2, unitType ) && IG_s( gridX - 1, gridY + 2, unitType ) && IG_s( gridX, gridY + 2, unitType ) && IG_s( gridX + 1, gridY + 2, unitType ) && IG_s( gridX + 2, gridY + 2, unitType ) && IG_s( gridX + 3, gridY + 2, unitType ) &&
                IG_s( gridX - 3, gridY + 1, unitType ) && IG_s( gridX - 2, gridY + 1, unitType ) && IG_s( gridX - 1, gridY + 1, unitType ) && IG_s( gridX, gridY + 1, unitType ) && IG_s( gridX + 1, gridY + 1, unitType ) && IG_s( gridX + 2, gridY + 1, unitType ) && IG_s( gridX + 3, gridY + 1, unitType ) &&
                IG_s( gridX - 3, gridY, unitType ) && IG_s( gridX - 2, gridY, unitType ) && IG_s( gridX - 1, gridY, unitType ) && IG_s( gridX, gridY, unitType ) && IG_s( gridX + 1, gridY, unitType ) && IG_s( gridX + 2, gridY, unitType ) && IG_s( gridX + 3, gridY, unitType ) &&
                IG_s( gridX - 3, gridY - 1, unitType ) && IG_s( gridX - 2, gridY - 1, unitType ) && IG_s( gridX - 1, gridY - 1, unitType ) && IG_s( gridX, gridY - 1, unitType ) && IG_s( gridX + 1, gridY - 1, unitType ) && IG_s( gridX + 2, gridY - 1, unitType ) && IG_s( gridX + 3, gridY - 1, unitType ) &&
                IG_s( gridX - 3, gridY - 2, unitType ) && IG_s( gridX - 2, gridY - 2, unitType ) && IG_s( gridX - 1, gridY - 2, unitType ) && IG_s( gridX, gridY - 2, unitType ) && IG_s( gridX + 1, gridY - 2, unitType ) && IG_s( gridX + 2, gridY - 2, unitType ) && IG_s( gridX + 3, gridY - 2, unitType ) &&
                IG_s( gridX - 3, gridY - 3, unitType ) && IG_s( gridX - 2, gridY - 3, unitType ) && IG_s( gridX - 1, gridY - 3, unitType ) && IG_s( gridX, gridY - 3, unitType ) && IG_s( gridX + 1, gridY - 3, unitType ) && IG_s( gridX + 2, gridY - 3, unitType ) && IG_s( gridX + 3, gridY - 3, unitType );
    else if( unitSize == 8 )
        return  IG_s( gridX - 4, gridY + 3, unitType ) && IG_s( gridX - 3, gridY + 3, unitType ) && IG_s( gridX - 2, gridY + 3, unitType ) && IG_s( gridX - 1, gridY + 3, unitType ) && IG_s( gridX, gridY + 3, unitType ) && IG_s( gridX + 1, gridY + 3, unitType ) && IG_s( gridX + 2, gridY + 3, unitType ) && IG_s( gridX + 3, gridY + 3, unitType ) &&
                IG_s( gridX - 4, gridY + 2, unitType ) && IG_s( gridX - 3, gridY + 2, unitType ) && IG_s( gridX - 2, gridY + 2, unitType ) && IG_s( gridX - 1, gridY + 2, unitType ) && IG_s( gridX, gridY + 2, unitType ) && IG_s( gridX + 1, gridY + 2, unitType ) && IG_s( gridX + 2, gridY + 2, unitType ) && IG_s( gridX + 3, gridY + 2, unitType ) &&
                IG_s( gridX - 4, gridY + 1, unitType ) && IG_s( gridX - 3, gridY + 1, unitType ) && IG_s( gridX - 2, gridY + 1, unitType ) && IG_s( gridX - 1, gridY + 1, unitType ) && IG_s( gridX, gridY + 1, unitType ) && IG_s( gridX + 1, gridY + 1, unitType ) && IG_s( gridX + 2, gridY + 1, unitType ) && IG_s( gridX + 3, gridY + 1, unitType ) &&
                IG_s( gridX - 4, gridY, unitType ) && IG_s( gridX - 3, gridY, unitType ) && IG_s( gridX - 2, gridY, unitType ) && IG_s( gridX - 1, gridY, unitType ) && IG_s( gridX, gridY, unitType ) && IG_s( gridX + 1, gridY, unitType ) && IG_s( gridX + 2, gridY, unitType ) && IG_s( gridX + 3, gridY, unitType ) &&
                IG_s( gridX - 4, gridY - 1, unitType ) && IG_s( gridX - 3, gridY - 1, unitType ) && IG_s( gridX - 2, gridY - 1, unitType ) && IG_s( gridX - 1, gridY - 1, unitType ) && IG_s( gridX, gridY - 1, unitType ) && IG_s( gridX + 1, gridY - 1, unitType ) && IG_s( gridX + 2, gridY - 1, unitType ) && IG_s( gridX + 3, gridY - 1, unitType ) &&
                IG_s( gridX - 4, gridY - 2, unitType ) && IG_s( gridX - 3, gridY - 2, unitType ) && IG_s( gridX - 2, gridY - 2, unitType ) && IG_s( gridX - 1, gridY - 2, unitType ) && IG_s( gridX, gridY - 2, unitType ) && IG_s( gridX + 1, gridY - 2, unitType ) && IG_s( gridX + 2, gridY - 2, unitType ) && IG_s( gridX + 3, gridY - 2, unitType ) &&
                IG_s( gridX - 4, gridY - 3, unitType ) && IG_s( gridX - 3, gridY - 3, unitType ) && IG_s( gridX - 2, gridY - 3, unitType ) && IG_s( gridX - 1, gridY - 3, unitType ) && IG_s( gridX, gridY - 3, unitType ) && IG_s( gridX + 1, gridY - 3, unitType ) && IG_s( gridX + 2, gridY - 3, unitType ) && IG_s( gridX + 3, gridY - 3, unitType ) &&
                IG_s( gridX - 4, gridY - 4, unitType ) && IG_s( gridX - 3, gridY - 4, unitType ) && IG_s( gridX - 2, gridY - 4, unitType ) && IG_s( gridX - 1, gridY - 4, unitType ) && IG_s( gridX, gridY - 4, unitType ) && IG_s( gridX + 1, gridY - 4, unitType ) && IG_s( gridX + 2, gridY - 4, unitType ) && IG_s( gridX + 3, gridY - 4, unitType );
    else if( unitSize == 9 )
        return  IG_s( gridX - 4, gridY + 4, unitType ) && IG_s( gridX - 3, gridY + 4, unitType ) && IG_s( gridX - 2, gridY + 4, unitType ) && IG_s( gridX - 1, gridY + 4, unitType ) && IG_s( gridX, gridY + 4, unitType ) && IG_s( gridX + 1, gridY + 4, unitType ) && IG_s( gridX + 2, gridY + 4, unitType ) && IG_s( gridX + 3, gridY + 4, unitType ) && IG_s( gridX + 4, gridY + 4, unitType ) &&
                IG_s( gridX - 4, gridY + 3, unitType ) && IG_s( gridX - 3, gridY + 3, unitType ) && IG_s( gridX - 2, gridY + 3, unitType ) && IG_s( gridX - 1, gridY + 3, unitType ) && IG_s( gridX, gridY + 3, unitType ) && IG_s( gridX + 1, gridY + 3, unitType ) && IG_s( gridX + 2, gridY + 3, unitType ) && IG_s( gridX + 3, gridY + 3, unitType ) && IG_s( gridX + 4, gridY + 3, unitType ) &&
                IG_s( gridX - 4, gridY + 2, unitType ) && IG_s( gridX - 3, gridY + 2, unitType ) && IG_s( gridX - 2, gridY + 2, unitType ) && IG_s( gridX - 1, gridY + 2, unitType ) && IG_s( gridX, gridY + 2, unitType ) && IG_s( gridX + 1, gridY + 2, unitType ) && IG_s( gridX + 2, gridY + 2, unitType ) && IG_s( gridX + 3, gridY + 2, unitType ) && IG_s( gridX + 4, gridY + 2, unitType ) &&
                IG_s( gridX - 4, gridY + 1, unitType ) && IG_s( gridX - 3, gridY + 1, unitType ) && IG_s( gridX - 2, gridY + 1, unitType ) && IG_s( gridX - 1, gridY + 1, unitType ) && IG_s( gridX, gridY + 1, unitType ) && IG_s( gridX + 1, gridY + 1, unitType ) && IG_s( gridX + 2, gridY + 1, unitType ) && IG_s( gridX + 3, gridY + 1, unitType ) && IG_s( gridX + 4, gridY + 1, unitType ) &&
                IG_s( gridX - 4, gridY, unitType ) && IG_s( gridX - 3, gridY, unitType ) && IG_s( gridX - 2, gridY, unitType ) && IG_s( gridX - 1, gridY, unitType ) && IG_s( gridX, gridY, unitType ) && IG_s( gridX + 1, gridY, unitType ) && IG_s( gridX + 2, gridY, unitType ) && IG_s( gridX + 3, gridY, unitType ) && IG_s( gridX + 4, gridY, unitType ) &&
                IG_s( gridX - 4, gridY - 1, unitType ) && IG_s( gridX - 3, gridY - 1, unitType ) && IG_s( gridX - 2, gridY - 1, unitType ) && IG_s( gridX - 1, gridY - 1, unitType ) && IG_s( gridX, gridY - 1, unitType ) && IG_s( gridX + 1, gridY - 1, unitType ) && IG_s( gridX + 2, gridY - 1, unitType ) && IG_s( gridX + 3, gridY - 1, unitType ) && IG_s( gridX + 4, gridY - 1, unitType ) &&
                IG_s( gridX - 4, gridY - 2, unitType ) && IG_s( gridX - 3, gridY - 2, unitType ) && IG_s( gridX - 2, gridY - 2, unitType ) && IG_s( gridX - 1, gridY - 2, unitType ) && IG_s( gridX, gridY - 2, unitType ) && IG_s( gridX + 1, gridY - 2, unitType ) && IG_s( gridX + 2, gridY - 2, unitType ) && IG_s( gridX + 3, gridY - 2, unitType ) && IG_s( gridX + 4, gridY - 2, unitType ) &&
                IG_s( gridX - 4, gridY - 3, unitType ) && IG_s( gridX - 3, gridY - 3, unitType ) && IG_s( gridX - 2, gridY - 3, unitType ) && IG_s( gridX - 1, gridY - 3, unitType ) && IG_s( gridX, gridY - 3, unitType ) && IG_s( gridX + 1, gridY - 3, unitType ) && IG_s( gridX + 2, gridY - 3, unitType ) && IG_s( gridX + 3, gridY - 3, unitType ) && IG_s( gridX + 4, gridY - 3, unitType ) &&
                IG_s( gridX - 4, gridY - 4, unitType ) && IG_s( gridX - 3, gridY - 4, unitType ) && IG_s( gridX - 2, gridY - 4, unitType ) && IG_s( gridX - 1, gridY - 4, unitType ) && IG_s( gridX, gridY - 4, unitType ) && IG_s( gridX + 1, gridY - 4, unitType ) && IG_s( gridX + 2, gridY - 4, unitType ) && IG_s( gridX + 3, gridY - 4, unitType ) && IG_s( gridX + 4, gridY - 4, unitType );
    else if( unitSize == 10 )
        return  IG_s( gridX - 5, gridY + 5, unitType ) && IG_s( gridX - 4, gridY + 5, unitType ) && IG_s( gridX - 3, gridY + 5, unitType ) && IG_s( gridX - 2, gridY + 5, unitType ) && IG_s( gridX - 1, gridY + 5, unitType ) && IG_s( gridX, gridY + 5, unitType ) && IG_s( gridX + 1, gridY + 5, unitType ) && IG_s( gridX + 2, gridY + 5, unitType ) && IG_s( gridX + 3, gridY + 5, unitType ) && IG_s( gridX + 4, gridY + 5, unitType ) &&
                IG_s( gridX - 5, gridY + 4, unitType ) && IG_s( gridX - 4, gridY + 4, unitType ) && IG_s( gridX - 3, gridY + 4, unitType ) && IG_s( gridX - 2, gridY + 4, unitType ) && IG_s( gridX - 1, gridY + 4, unitType ) && IG_s( gridX, gridY + 4, unitType ) && IG_s( gridX + 1, gridY + 4, unitType ) && IG_s( gridX + 2, gridY + 4, unitType ) && IG_s( gridX + 3, gridY + 4, unitType ) && IG_s( gridX + 4, gridY + 4, unitType ) &&
                IG_s( gridX - 5, gridY + 3, unitType ) && IG_s( gridX - 4, gridY + 3, unitType ) && IG_s( gridX - 3, gridY + 3, unitType ) && IG_s( gridX - 2, gridY + 3, unitType ) && IG_s( gridX - 1, gridY + 3, unitType ) && IG_s( gridX, gridY + 3, unitType ) && IG_s( gridX + 1, gridY + 3, unitType ) && IG_s( gridX + 2, gridY + 3, unitType ) && IG_s( gridX + 3, gridY + 3, unitType ) && IG_s( gridX + 4, gridY + 3, unitType ) &&
                IG_s( gridX - 5, gridY + 2, unitType ) && IG_s( gridX - 4, gridY + 2, unitType ) && IG_s( gridX - 3, gridY + 2, unitType ) && IG_s( gridX - 2, gridY + 2, unitType ) && IG_s( gridX - 1, gridY + 2, unitType ) && IG_s( gridX, gridY + 2, unitType ) && IG_s( gridX + 1, gridY + 2, unitType ) && IG_s( gridX + 2, gridY + 2, unitType ) && IG_s( gridX + 3, gridY + 2, unitType ) && IG_s( gridX + 4, gridY + 2, unitType ) &&
                IG_s( gridX - 5, gridY + 1, unitType ) && IG_s( gridX - 4, gridY + 1, unitType ) && IG_s( gridX - 3, gridY + 1, unitType ) && IG_s( gridX - 2, gridY + 1, unitType ) && IG_s( gridX - 1, gridY + 1, unitType ) && IG_s( gridX, gridY + 1, unitType ) && IG_s( gridX + 1, gridY + 1, unitType ) && IG_s( gridX + 2, gridY + 1, unitType ) && IG_s( gridX + 3, gridY + 1, unitType ) && IG_s( gridX + 4, gridY + 1, unitType ) &&
                IG_s( gridX - 5, gridY, unitType ) && IG_s( gridX - 4, gridY, unitType ) && IG_s( gridX - 3, gridY, unitType ) && IG_s( gridX - 2, gridY, unitType ) && IG_s( gridX - 1, gridY, unitType ) && IG_s( gridX, gridY, unitType ) && IG_s( gridX + 1, gridY, unitType ) && IG_s( gridX + 2, gridY, unitType ) && IG_s( gridX + 3, gridY, unitType ) && IG_s( gridX + 4, gridY, unitType ) &&
                IG_s( gridX - 5, gridY - 1, unitType ) && IG_s( gridX - 4, gridY - 1, unitType ) && IG_s( gridX - 3, gridY - 1, unitType ) && IG_s( gridX - 2, gridY - 1, unitType ) && IG_s( gridX - 1, gridY - 1, unitType ) && IG_s( gridX, gridY - 1, unitType ) && IG_s( gridX + 1, gridY - 1, unitType ) && IG_s( gridX + 2, gridY - 1, unitType ) && IG_s( gridX + 3, gridY - 1, unitType ) && IG_s( gridX + 4, gridY - 1, unitType ) &&
                IG_s( gridX - 5, gridY - 2, unitType ) && IG_s( gridX - 4, gridY - 2, unitType ) && IG_s( gridX - 3, gridY - 2, unitType ) && IG_s( gridX - 2, gridY - 2, unitType ) && IG_s( gridX - 1, gridY - 2, unitType ) && IG_s( gridX, gridY - 2, unitType ) && IG_s( gridX + 1, gridY - 2, unitType ) && IG_s( gridX + 2, gridY - 2, unitType ) && IG_s( gridX + 3, gridY - 2, unitType ) && IG_s( gridX + 4, gridY - 2, unitType ) &&
                IG_s( gridX - 5, gridY - 3, unitType ) && IG_s( gridX - 4, gridY - 3, unitType ) && IG_s( gridX - 3, gridY - 3, unitType ) && IG_s( gridX - 2, gridY - 3, unitType ) && IG_s( gridX - 1, gridY - 3, unitType ) && IG_s( gridX, gridY - 3, unitType ) && IG_s( gridX + 1, gridY - 3, unitType ) && IG_s( gridX + 2, gridY - 3, unitType ) && IG_s( gridX + 3, gridY - 3, unitType ) && IG_s( gridX + 4, gridY - 3, unitType ) &&
                IG_s( gridX - 5, gridY - 4, unitType ) && IG_s( gridX - 4, gridY - 4, unitType ) && IG_s( gridX - 3, gridY - 4, unitType ) && IG_s( gridX - 2, gridY - 4, unitType ) && IG_s( gridX - 1, gridY - 4, unitType ) && IG_s( gridX, gridY - 4, unitType ) && IG_s( gridX + 1, gridY - 4, unitType ) && IG_s( gridX + 2, gridY - 4, unitType ) && IG_s( gridX + 3, gridY - 4, unitType ) && IG_s( gridX + 4, gridY - 4, unitType );
    else
    {
        return 1;
    }
}


unsigned char IsGridFree_i( int gridX, int gridY, char unitSize, char unitType )
{
    if( unitSize == 1 )
    {
        return IG_i( gridX, gridY, unitType );
    }

    else if( unitSize == 2 )
        return IG_i( gridX - 1, gridY, unitType ) && IG_i( gridX, gridY, unitType ) &&
               IG_i( gridX - 1, gridY - 1, unitType ) && IG_i( gridX, gridY - 1, unitType );

    else if( unitSize == 3 )
        return IG_i( gridX - 1, gridY + 1, unitType ) && IG_i( gridX, gridY + 1, unitType ) && IG_i( gridX + 1, gridY + 1, unitType ) &&
               IG_i( gridX - 1, gridY, unitType ) && IG_i( gridX, gridY, unitType ) && IG_i( gridX + 1, gridY, unitType ) &&
               IG_i( gridX - 1, gridY - 1, unitType ) && IG_i( gridX, gridY - 1, unitType ) && IG_i( gridX + 1, gridY - 1, unitType );

    else if( unitSize == 4 )
        return IG_i( gridX - 2, gridY + 2, unitType ) && IG_i( gridX - 1, gridY + 2, unitType ) && IG_i( gridX, gridY + 2, unitType ) && IG_i( gridX + 1, gridY + 2, unitType ) &&
               IG_i( gridX - 2, gridY + 1, unitType ) && IG_i( gridX - 1, gridY + 1, unitType ) && IG_i( gridX, gridY + 1, unitType ) && IG_i( gridX + 1, gridY + 1, unitType ) &&
               IG_i( gridX - 2, gridY, unitType ) && IG_i( gridX - 1, gridY, unitType ) && IG_i( gridX, gridY, unitType ) && IG_i( gridX + 1, gridY, unitType ) &&
               IG_i( gridX - 2, gridY - 1, unitType ) && IG_i( gridX - 1, gridY - 1, unitType ) && IG_i( gridX, gridY - 1, unitType ) && IG_i( gridX + 1, gridY - 1, unitType );

    else if( unitSize == 5 )
        return IG_i( gridX - 2, gridY + 2, unitType ) && IG_i( gridX - 1, gridY + 2, unitType ) && IG_i( gridX, gridY + 2, unitType ) && IG_i( gridX + 1, gridY + 2, unitType ) && IG_i( gridX + 2, gridY + 2, unitType ) &&
               IG_i( gridX - 2, gridY + 1, unitType ) && IG_i( gridX - 1, gridY + 1, unitType ) && IG_i( gridX, gridY + 1, unitType ) && IG_i( gridX + 1, gridY + 1, unitType ) && IG_i( gridX + 2, gridY + 1, unitType ) &&
               IG_i( gridX - 2, gridY, unitType ) && IG_i( gridX - 1, gridY, unitType ) && IG_i( gridX, gridY, unitType ) && IG_i( gridX + 1, gridY, unitType ) && IG_i( gridX + 2, gridY, unitType ) &&
               IG_i( gridX - 2, gridY - 1, unitType ) && IG_i( gridX - 1, gridY - 1, unitType ) && IG_i( gridX, gridY - 1, unitType ) && IG_i( gridX + 1, gridY - 1, unitType ) && IG_i( gridX + 2, gridY - 1, unitType ) &&
               IG_i( gridX - 2, gridY - 2, unitType ) && IG_i( gridX - 1, gridY - 2, unitType ) && IG_i( gridX, gridY - 2, unitType ) && IG_i( gridX + 1, gridY - 2, unitType ) && IG_i( gridX + 2, gridY - 2, unitType );

    else if( unitSize == 6 )
        return IG_i( gridX - 3, gridY + 3, unitType ) && IG_i( gridX - 2, gridY + 3, unitType ) && IG_i( gridX - 1, gridY + 3, unitType ) && IG_i( gridX, gridY + 3, unitType ) && IG_i( gridX + 1, gridY + 3, unitType ) && IG_i( gridX + 2, gridY + 3, unitType ) &&
               IG_i( gridX - 3, gridY + 2, unitType ) && IG_i( gridX - 2, gridY + 2, unitType ) && IG_i( gridX - 1, gridY + 2, unitType ) && IG_i( gridX, gridY + 2, unitType ) && IG_i( gridX + 1, gridY + 2, unitType ) && IG_i( gridX + 2, gridY + 2, unitType ) &&
               IG_i( gridX - 3, gridY + 1, unitType ) && IG_i( gridX - 2, gridY + 1, unitType ) && IG_i( gridX - 1, gridY + 1, unitType ) && IG_i( gridX, gridY + 1, unitType ) && IG_i( gridX + 1, gridY + 1, unitType ) && IG_i( gridX + 2, gridY + 1, unitType ) &&
               IG_i( gridX - 3, gridY, unitType ) && IG_i( gridX - 2, gridY, unitType ) && IG_i( gridX - 1, gridY, unitType ) && IG_i( gridX, gridY, unitType ) && IG_i( gridX + 1, gridY, unitType ) && IG_i( gridX + 2, gridY, unitType ) &&
               IG_i( gridX - 3, gridY - 1, unitType ) && IG_i( gridX - 2, gridY - 1, unitType ) && IG_i( gridX - 1, gridY - 1, unitType ) && IG_i( gridX, gridY - 1, unitType ) && IG_i( gridX + 1, gridY - 1, unitType ) && IG_i( gridX + 2, gridY - 1, unitType ) &&
               IG_i( gridX - 3, gridY - 2, unitType ) && IG_i( gridX - 2, gridY - 2, unitType ) && IG_i( gridX - 1, gridY - 2, unitType ) && IG_i( gridX, gridY - 2, unitType ) && IG_i( gridX + 1, gridY - 2, unitType ) && IG_i( gridX + 2, gridY - 2, unitType );

    else if( unitSize == 7 )
        return  IG_i( gridX - 3, gridY + 3, unitType ) && IG_i( gridX - 2, gridY + 3, unitType ) && IG_i( gridX - 1, gridY + 3, unitType ) && IG_i( gridX, gridY + 3, unitType ) && IG_i( gridX + 1, gridY + 3, unitType ) && IG_i( gridX + 2, gridY + 3, unitType ) && IG_i( gridX + 3, gridY + 3, unitType ) &&
                IG_i( gridX - 3, gridY + 2, unitType ) && IG_i( gridX - 2, gridY + 2, unitType ) && IG_i( gridX - 1, gridY + 2, unitType ) && IG_i( gridX, gridY + 2, unitType ) && IG_i( gridX + 1, gridY + 2, unitType ) && IG_i( gridX + 2, gridY + 2, unitType ) && IG_i( gridX + 3, gridY + 2, unitType ) &&
                IG_i( gridX - 3, gridY + 1, unitType ) && IG_i( gridX - 2, gridY + 1, unitType ) && IG_i( gridX - 1, gridY + 1, unitType ) && IG_i( gridX, gridY + 1, unitType ) && IG_i( gridX + 1, gridY + 1, unitType ) && IG_i( gridX + 2, gridY + 1, unitType ) && IG_i( gridX + 3, gridY + 1, unitType ) &&
                IG_i( gridX - 3, gridY, unitType ) && IG_i( gridX - 2, gridY, unitType ) && IG_i( gridX - 1, gridY, unitType ) && IG_i( gridX, gridY, unitType ) && IG_i( gridX + 1, gridY, unitType ) && IG_i( gridX + 2, gridY, unitType ) && IG_i( gridX + 3, gridY, unitType ) &&
                IG_i( gridX - 3, gridY - 1, unitType ) && IG_i( gridX - 2, gridY - 1, unitType ) && IG_i( gridX - 1, gridY - 1, unitType ) && IG_i( gridX, gridY - 1, unitType ) && IG_i( gridX + 1, gridY - 1, unitType ) && IG_i( gridX + 2, gridY - 1, unitType ) && IG_i( gridX + 3, gridY - 1, unitType ) &&
                IG_i( gridX - 3, gridY - 2, unitType ) && IG_i( gridX - 2, gridY - 2, unitType ) && IG_i( gridX - 1, gridY - 2, unitType ) && IG_i( gridX, gridY - 2, unitType ) && IG_i( gridX + 1, gridY - 2, unitType ) && IG_i( gridX + 2, gridY - 2, unitType ) && IG_i( gridX + 3, gridY - 2, unitType ) &&
                IG_i( gridX - 3, gridY - 3, unitType ) && IG_i( gridX - 2, gridY - 3, unitType ) && IG_i( gridX - 1, gridY - 3, unitType ) && IG_i( gridX, gridY - 3, unitType ) && IG_i( gridX + 1, gridY - 3, unitType ) && IG_i( gridX + 2, gridY - 3, unitType ) && IG_i( gridX + 3, gridY - 3, unitType );
    else if( unitSize == 8 )
        return  IG_i( gridX - 4, gridY + 3, unitType ) && IG_i( gridX - 3, gridY + 3, unitType ) && IG_i( gridX - 2, gridY + 3, unitType ) && IG_i( gridX - 1, gridY + 3, unitType ) && IG_i( gridX, gridY + 3, unitType ) && IG_i( gridX + 1, gridY + 3, unitType ) && IG_i( gridX + 2, gridY + 3, unitType ) && IG_i( gridX + 3, gridY + 3, unitType ) &&
                IG_i( gridX - 4, gridY + 2, unitType ) && IG_i( gridX - 3, gridY + 2, unitType ) && IG_i( gridX - 2, gridY + 2, unitType ) && IG_i( gridX - 1, gridY + 2, unitType ) && IG_i( gridX, gridY + 2, unitType ) && IG_i( gridX + 1, gridY + 2, unitType ) && IG_i( gridX + 2, gridY + 2, unitType ) && IG_i( gridX + 3, gridY + 2, unitType ) &&
                IG_i( gridX - 4, gridY + 1, unitType ) && IG_i( gridX - 3, gridY + 1, unitType ) && IG_i( gridX - 2, gridY + 1, unitType ) && IG_i( gridX - 1, gridY + 1, unitType ) && IG_i( gridX, gridY + 1, unitType ) && IG_i( gridX + 1, gridY + 1, unitType ) && IG_i( gridX + 2, gridY + 1, unitType ) && IG_i( gridX + 3, gridY + 1, unitType ) &&
                IG_i( gridX - 4, gridY, unitType ) && IG_i( gridX - 3, gridY, unitType ) && IG_i( gridX - 2, gridY, unitType ) && IG_i( gridX - 1, gridY, unitType ) && IG_i( gridX, gridY, unitType ) && IG_i( gridX + 1, gridY, unitType ) && IG_i( gridX + 2, gridY, unitType ) && IG_i( gridX + 3, gridY, unitType ) &&
                IG_i( gridX - 4, gridY - 1, unitType ) && IG_i( gridX - 3, gridY - 1, unitType ) && IG_i( gridX - 2, gridY - 1, unitType ) && IG_i( gridX - 1, gridY - 1, unitType ) && IG_i( gridX, gridY - 1, unitType ) && IG_i( gridX + 1, gridY - 1, unitType ) && IG_i( gridX + 2, gridY - 1, unitType ) && IG_i( gridX + 3, gridY - 1, unitType ) &&
                IG_i( gridX - 4, gridY - 2, unitType ) && IG_i( gridX - 3, gridY - 2, unitType ) && IG_i( gridX - 2, gridY - 2, unitType ) && IG_i( gridX - 1, gridY - 2, unitType ) && IG_i( gridX, gridY - 2, unitType ) && IG_i( gridX + 1, gridY - 2, unitType ) && IG_i( gridX + 2, gridY - 2, unitType ) && IG_i( gridX + 3, gridY - 2, unitType ) &&
                IG_i( gridX - 4, gridY - 3, unitType ) && IG_i( gridX - 3, gridY - 3, unitType ) && IG_i( gridX - 2, gridY - 3, unitType ) && IG_i( gridX - 1, gridY - 3, unitType ) && IG_i( gridX, gridY - 3, unitType ) && IG_i( gridX + 1, gridY - 3, unitType ) && IG_i( gridX + 2, gridY - 3, unitType ) && IG_i( gridX + 3, gridY - 3, unitType ) &&
                IG_i( gridX - 4, gridY - 4, unitType ) && IG_i( gridX - 3, gridY - 4, unitType ) && IG_i( gridX - 2, gridY - 4, unitType ) && IG_i( gridX - 1, gridY - 4, unitType ) && IG_i( gridX, gridY - 4, unitType ) && IG_i( gridX + 1, gridY - 4, unitType ) && IG_i( gridX + 2, gridY - 4, unitType ) && IG_i( gridX + 3, gridY - 4, unitType );
    else if( unitSize == 9 )
        return  IG_i( gridX - 4, gridY + 4, unitType ) && IG_i( gridX - 3, gridY + 4, unitType ) && IG_i( gridX - 2, gridY + 4, unitType ) && IG_i( gridX - 1, gridY + 4, unitType ) && IG_i( gridX, gridY + 4, unitType ) && IG_i( gridX + 1, gridY + 4, unitType ) && IG_i( gridX + 2, gridY + 4, unitType ) && IG_i( gridX + 3, gridY + 4, unitType ) && IG_i( gridX + 4, gridY + 4, unitType ) &&
                IG_i( gridX - 4, gridY + 3, unitType ) && IG_i( gridX - 3, gridY + 3, unitType ) && IG_i( gridX - 2, gridY + 3, unitType ) && IG_i( gridX - 1, gridY + 3, unitType ) && IG_i( gridX, gridY + 3, unitType ) && IG_i( gridX + 1, gridY + 3, unitType ) && IG_i( gridX + 2, gridY + 3, unitType ) && IG_i( gridX + 3, gridY + 3, unitType ) && IG_i( gridX + 4, gridY + 3, unitType ) &&
                IG_i( gridX - 4, gridY + 2, unitType ) && IG_i( gridX - 3, gridY + 2, unitType ) && IG_i( gridX - 2, gridY + 2, unitType ) && IG_i( gridX - 1, gridY + 2, unitType ) && IG_i( gridX, gridY + 2, unitType ) && IG_i( gridX + 1, gridY + 2, unitType ) && IG_i( gridX + 2, gridY + 2, unitType ) && IG_i( gridX + 3, gridY + 2, unitType ) && IG_i( gridX + 4, gridY + 2, unitType ) &&
                IG_i( gridX - 4, gridY + 1, unitType ) && IG_i( gridX - 3, gridY + 1, unitType ) && IG_i( gridX - 2, gridY + 1, unitType ) && IG_i( gridX - 1, gridY + 1, unitType ) && IG_i( gridX, gridY + 1, unitType ) && IG_i( gridX + 1, gridY + 1, unitType ) && IG_i( gridX + 2, gridY + 1, unitType ) && IG_i( gridX + 3, gridY + 1, unitType ) && IG_i( gridX + 4, gridY + 1, unitType ) &&
                IG_i( gridX - 4, gridY, unitType ) && IG_i( gridX - 3, gridY, unitType ) && IG_i( gridX - 2, gridY, unitType ) && IG_i( gridX - 1, gridY, unitType ) && IG_i( gridX, gridY, unitType ) && IG_i( gridX + 1, gridY, unitType ) && IG_i( gridX + 2, gridY, unitType ) && IG_i( gridX + 3, gridY, unitType ) && IG_i( gridX + 4, gridY, unitType ) &&
                IG_i( gridX - 4, gridY - 1, unitType ) && IG_i( gridX - 3, gridY - 1, unitType ) && IG_i( gridX - 2, gridY - 1, unitType ) && IG_i( gridX - 1, gridY - 1, unitType ) && IG_i( gridX, gridY - 1, unitType ) && IG_i( gridX + 1, gridY - 1, unitType ) && IG_i( gridX + 2, gridY - 1, unitType ) && IG_i( gridX + 3, gridY - 1, unitType ) && IG_i( gridX + 4, gridY - 1, unitType ) &&
                IG_i( gridX - 4, gridY - 2, unitType ) && IG_i( gridX - 3, gridY - 2, unitType ) && IG_i( gridX - 2, gridY - 2, unitType ) && IG_i( gridX - 1, gridY - 2, unitType ) && IG_i( gridX, gridY - 2, unitType ) && IG_i( gridX + 1, gridY - 2, unitType ) && IG_i( gridX + 2, gridY - 2, unitType ) && IG_i( gridX + 3, gridY - 2, unitType ) && IG_i( gridX + 4, gridY - 2, unitType ) &&
                IG_i( gridX - 4, gridY - 3, unitType ) && IG_i( gridX - 3, gridY - 3, unitType ) && IG_i( gridX - 2, gridY - 3, unitType ) && IG_i( gridX - 1, gridY - 3, unitType ) && IG_i( gridX, gridY - 3, unitType ) && IG_i( gridX + 1, gridY - 3, unitType ) && IG_i( gridX + 2, gridY - 3, unitType ) && IG_i( gridX + 3, gridY - 3, unitType ) && IG_i( gridX + 4, gridY - 3, unitType ) &&
                IG_i( gridX - 4, gridY - 4, unitType ) && IG_i( gridX - 3, gridY - 4, unitType ) && IG_i( gridX - 2, gridY - 4, unitType ) && IG_i( gridX - 1, gridY - 4, unitType ) && IG_i( gridX, gridY - 4, unitType ) && IG_i( gridX + 1, gridY - 4, unitType ) && IG_i( gridX + 2, gridY - 4, unitType ) && IG_i( gridX + 3, gridY - 4, unitType ) && IG_i( gridX + 4, gridY - 4, unitType );
    else if( unitSize == 10 )
        return  IG_i( gridX - 5, gridY + 5, unitType ) && IG_i( gridX - 4, gridY + 5, unitType ) && IG_i( gridX - 3, gridY + 5, unitType ) && IG_i( gridX - 2, gridY + 5, unitType ) && IG_i( gridX - 1, gridY + 5, unitType ) && IG_i( gridX, gridY + 5, unitType ) && IG_i( gridX + 1, gridY + 5, unitType ) && IG_i( gridX + 2, gridY + 5, unitType ) && IG_i( gridX + 3, gridY + 5, unitType ) && IG_i( gridX + 4, gridY + 5, unitType ) &&
                IG_i( gridX - 5, gridY + 4, unitType ) && IG_i( gridX - 4, gridY + 4, unitType ) && IG_i( gridX - 3, gridY + 4, unitType ) && IG_i( gridX - 2, gridY + 4, unitType ) && IG_i( gridX - 1, gridY + 4, unitType ) && IG_i( gridX, gridY + 4, unitType ) && IG_i( gridX + 1, gridY + 4, unitType ) && IG_i( gridX + 2, gridY + 4, unitType ) && IG_i( gridX + 3, gridY + 4, unitType ) && IG_i( gridX + 4, gridY + 4, unitType ) &&
                IG_i( gridX - 5, gridY + 3, unitType ) && IG_i( gridX - 4, gridY + 3, unitType ) && IG_i( gridX - 3, gridY + 3, unitType ) && IG_i( gridX - 2, gridY + 3, unitType ) && IG_i( gridX - 1, gridY + 3, unitType ) && IG_i( gridX, gridY + 3, unitType ) && IG_i( gridX + 1, gridY + 3, unitType ) && IG_i( gridX + 2, gridY + 3, unitType ) && IG_i( gridX + 3, gridY + 3, unitType ) && IG_i( gridX + 4, gridY + 3, unitType ) &&
                IG_i( gridX - 5, gridY + 2, unitType ) && IG_i( gridX - 4, gridY + 2, unitType ) && IG_i( gridX - 3, gridY + 2, unitType ) && IG_i( gridX - 2, gridY + 2, unitType ) && IG_i( gridX - 1, gridY + 2, unitType ) && IG_i( gridX, gridY + 2, unitType ) && IG_i( gridX + 1, gridY + 2, unitType ) && IG_i( gridX + 2, gridY + 2, unitType ) && IG_i( gridX + 3, gridY + 2, unitType ) && IG_i( gridX + 4, gridY + 2, unitType ) &&
                IG_i( gridX - 5, gridY + 1, unitType ) && IG_i( gridX - 4, gridY + 1, unitType ) && IG_i( gridX - 3, gridY + 1, unitType ) && IG_i( gridX - 2, gridY + 1, unitType ) && IG_i( gridX - 1, gridY + 1, unitType ) && IG_i( gridX, gridY + 1, unitType ) && IG_i( gridX + 1, gridY + 1, unitType ) && IG_i( gridX + 2, gridY + 1, unitType ) && IG_i( gridX + 3, gridY + 1, unitType ) && IG_i( gridX + 4, gridY + 1, unitType ) &&
                IG_i( gridX - 5, gridY, unitType ) && IG_i( gridX - 4, gridY, unitType ) && IG_i( gridX - 3, gridY, unitType ) && IG_i( gridX - 2, gridY, unitType ) && IG_i( gridX - 1, gridY, unitType ) && IG_i( gridX, gridY, unitType ) && IG_i( gridX + 1, gridY, unitType ) && IG_i( gridX + 2, gridY, unitType ) && IG_i( gridX + 3, gridY, unitType ) && IG_i( gridX + 4, gridY, unitType ) &&
                IG_i( gridX - 5, gridY - 1, unitType ) && IG_i( gridX - 4, gridY - 1, unitType ) && IG_i( gridX - 3, gridY - 1, unitType ) && IG_i( gridX - 2, gridY - 1, unitType ) && IG_i( gridX - 1, gridY - 1, unitType ) && IG_i( gridX, gridY - 1, unitType ) && IG_i( gridX + 1, gridY - 1, unitType ) && IG_i( gridX + 2, gridY - 1, unitType ) && IG_i( gridX + 3, gridY - 1, unitType ) && IG_i( gridX + 4, gridY - 1, unitType ) &&
                IG_i( gridX - 5, gridY - 2, unitType ) && IG_i( gridX - 4, gridY - 2, unitType ) && IG_i( gridX - 3, gridY - 2, unitType ) && IG_i( gridX - 2, gridY - 2, unitType ) && IG_i( gridX - 1, gridY - 2, unitType ) && IG_i( gridX, gridY - 2, unitType ) && IG_i( gridX + 1, gridY - 2, unitType ) && IG_i( gridX + 2, gridY - 2, unitType ) && IG_i( gridX + 3, gridY - 2, unitType ) && IG_i( gridX + 4, gridY - 2, unitType ) &&
                IG_i( gridX - 5, gridY - 3, unitType ) && IG_i( gridX - 4, gridY - 3, unitType ) && IG_i( gridX - 3, gridY - 3, unitType ) && IG_i( gridX - 2, gridY - 3, unitType ) && IG_i( gridX - 1, gridY - 3, unitType ) && IG_i( gridX, gridY - 3, unitType ) && IG_i( gridX + 1, gridY - 3, unitType ) && IG_i( gridX + 2, gridY - 3, unitType ) && IG_i( gridX + 3, gridY - 3, unitType ) && IG_i( gridX + 4, gridY - 3, unitType ) &&
                IG_i( gridX - 5, gridY - 4, unitType ) && IG_i( gridX - 4, gridY - 4, unitType ) && IG_i( gridX - 3, gridY - 4, unitType ) && IG_i( gridX - 2, gridY - 4, unitType ) && IG_i( gridX - 1, gridY - 4, unitType ) && IG_i( gridX, gridY - 4, unitType ) && IG_i( gridX + 1, gridY - 4, unitType ) && IG_i( gridX + 2, gridY - 4, unitType ) && IG_i( gridX + 3, gridY - 4, unitType ) && IG_i( gridX + 4, gridY - 4, unitType );
    else
    {
        return 0;
    }
}



unsigned char IsPointFree( int iMapIndex, int posX, int posY, char unitSize, char unitType )
{
	SetCurMapDataBlock_i( iMapIndex );
    Vec2 stVecTmp;

    if( ( unitSize * 0x1 ) == 0 ) //偶数
    {
        posX = posX + PER_CELL / 2;
        posY = posY + PER_CELL / 2;
    }    

    PosToGrid( posX, posY, &stVecTmp );
    int gridX = stVecTmp.x;
    int gridY = stVecTmp.y;

	return IsGridFree_i( gridX, gridY, unitSize, unitType );
}

unsigned char IsPointInStaticBlock( int iMapIndex, int posX, int posY, char unitSize, char unitType )
{
	SetCurMapDataBlock_i( iMapIndex );
	Vec2 stVecTmp;

	if ((unitSize * 0x1) == 0) //偶数
	{
		posX = posX + PER_CELL / 2;
		posY = posY + PER_CELL / 2;
	}

	PosToGrid(posX, posY, &stVecTmp);
	int gridX = stVecTmp.x;
	int gridY = stVecTmp.y;

	return IsGridInStaticBlock_i( gridX, gridY, unitSize, unitType );
}

void ClearGridBlock( int iMapIndex, int posX, int posY, int unitSize, char unitType )
{
	SetCurMapDataBlock_i( iMapIndex );

    Vec2 stVecTmp;

    if( ( unitSize * 0x1 ) == 0 ) //偶数
    {
        posX = posX + PER_CELL / 2;
        posY = posY + PER_CELL / 2;
    }

    PosToGrid( posX, posY, &stVecTmp );
    int gridX = stVecTmp.x;
    int gridY = stVecTmp.y;

    if( unitSize == 1 )
    {
        IG_c( gridX, gridY, unitType );
    }

    else if( unitSize == 2 )
    {
        IG_c( gridX - 1, gridY, unitType );
        IG_c( gridX, gridY, unitType );
        IG_c( gridX - 1, gridY - 1, unitType );
        IG_c( gridX, gridY - 1, unitType );
    }
    else if( unitSize == 3 )
    {
        IG_c( gridX - 1, gridY + 1, unitType );
        IG_c( gridX, gridY + 1, unitType );
        IG_c( gridX + 1, gridY + 1, unitType );
        IG_c( gridX - 1, gridY, unitType );
        IG_c( gridX, gridY, unitType );
        IG_c( gridX + 1, gridY, unitType );
        IG_c( gridX - 1, gridY - 1, unitType );
        IG_c( gridX, gridY - 1, unitType );
        IG_c( gridX + 1, gridY - 1, unitType );
    }
    else if( unitSize == 4 )
    {
        IG_c( gridX - 2, gridY + 2, unitType );
        IG_c( gridX - 1, gridY + 2, unitType );
        IG_c( gridX, gridY + 2, unitType );
        IG_c( gridX + 1, gridY + 2, unitType );
        IG_c( gridX - 2, gridY + 1, unitType );
        IG_c( gridX - 1, gridY + 1, unitType );
        IG_c( gridX, gridY + 1, unitType );
        IG_c( gridX + 1, gridY + 1, unitType );
        IG_c( gridX - 2, gridY, unitType );
        IG_c( gridX - 1, gridY, unitType );
        IG_c( gridX, gridY, unitType );
        IG_c( gridX + 1, gridY, unitType );
        IG_c( gridX - 2, gridY - 1, unitType );
        IG_c( gridX - 1, gridY - 1, unitType );
        IG_c( gridX, gridY - 1, unitType );
        IG_c( gridX + 1, gridY - 1, unitType );
    }
    else if( unitSize == 5 )
    {
        IG_c( gridX - 2, gridY + 2, unitType );
        IG_c( gridX - 1, gridY + 2, unitType );
        IG_c( gridX, gridY + 2, unitType );
        IG_c( gridX + 1, gridY + 2, unitType );
        IG_c( gridX + 2, gridY + 2, unitType );
        IG_c( gridX - 2, gridY + 1, unitType );
        IG_c( gridX - 1, gridY + 1, unitType );
        IG_c( gridX, gridY + 1, unitType );
        IG_c( gridX + 1, gridY + 1, unitType );
        IG_c( gridX + 2, gridY + 1, unitType );
        IG_c( gridX - 2, gridY, unitType );
        IG_c( gridX - 1, gridY, unitType );
        IG_c( gridX, gridY, unitType );
        IG_c( gridX + 1, gridY, unitType );
        IG_c( gridX + 2, gridY, unitType );
        IG_c( gridX - 2, gridY - 1, unitType );
        IG_c( gridX - 1, gridY - 1, unitType );
        IG_c( gridX, gridY - 1, unitType );
        IG_c( gridX + 1, gridY - 1, unitType );
        IG_c( gridX + 2, gridY - 1, unitType );
        IG_c( gridX - 2, gridY - 2, unitType );
        IG_c( gridX - 1, gridY - 2, unitType );
        IG_c( gridX, gridY - 2, unitType );
        IG_c( gridX + 1, gridY - 2, unitType );
        IG_c( gridX + 2, gridY - 2, unitType );
    }
    else if( unitSize == 6 )
    {
        IG_c( gridX - 3, gridY + 3, unitType );
        IG_c( gridX - 2, gridY + 3, unitType );
        IG_c( gridX - 1, gridY + 3, unitType );
        IG_c( gridX, gridY + 3, unitType );
        IG_c( gridX + 1, gridY + 3, unitType );
        IG_c( gridX + 2, gridY + 3, unitType );
        IG_c( gridX - 3, gridY + 2, unitType );
        IG_c( gridX - 2, gridY + 2, unitType );
        IG_c( gridX - 1, gridY + 2, unitType );
        IG_c( gridX, gridY + 2, unitType );
        IG_c( gridX + 1, gridY + 2, unitType );
        IG_c( gridX + 2, gridY + 2, unitType );
        IG_c( gridX - 3, gridY + 1, unitType );
        IG_c( gridX - 2, gridY + 1, unitType );
        IG_c( gridX - 1, gridY + 1, unitType );
        IG_c( gridX, gridY + 1, unitType );
        IG_c( gridX + 1, gridY + 1, unitType );
        IG_c( gridX + 2, gridY + 1, unitType );
        IG_c( gridX - 3, gridY, unitType );
        IG_c( gridX - 2, gridY, unitType );
        IG_c( gridX - 1, gridY, unitType );
        IG_c( gridX, gridY, unitType );
        IG_c( gridX + 1, gridY, unitType );
        IG_c( gridX + 2, gridY, unitType );
        IG_c( gridX - 3, gridY - 1, unitType );
        IG_c( gridX - 2, gridY - 1, unitType );
        IG_c( gridX - 1, gridY - 1, unitType );
        IG_c( gridX, gridY - 1, unitType );
        IG_c( gridX + 1, gridY - 1, unitType );
        IG_c( gridX + 2, gridY - 1, unitType );
        IG_c( gridX - 3, gridY - 2, unitType );
        IG_c( gridX - 2, gridY - 2, unitType );
        IG_c( gridX - 1, gridY - 2, unitType );
        IG_c( gridX, gridY - 2, unitType );
        IG_c( gridX + 1, gridY - 2, unitType );
        IG_c( gridX + 2, gridY - 2, unitType );
    }
    else if( unitSize == 7 )
    {
        IG_c( gridX - 3, gridY + 3, unitType );
        IG_c( gridX - 2, gridY + 3, unitType );
        IG_c( gridX - 1, gridY + 3, unitType );
        IG_c( gridX, gridY + 3, unitType );
        IG_c( gridX + 1, gridY + 3, unitType );
        IG_c( gridX + 2, gridY + 3, unitType );
        IG_c( gridX + 3, gridY + 3, unitType );
        IG_c( gridX - 3, gridY + 2, unitType );
        IG_c( gridX - 2, gridY + 2, unitType );
        IG_c( gridX - 1, gridY + 2, unitType );
        IG_c( gridX, gridY + 2, unitType );
        IG_c( gridX + 1, gridY + 2, unitType );
        IG_c( gridX + 2, gridY + 2, unitType );
        IG_c( gridX + 3, gridY + 2, unitType );
        IG_c( gridX - 3, gridY + 1, unitType );
        IG_c( gridX - 2, gridY + 1, unitType );
        IG_c( gridX - 1, gridY + 1, unitType );
        IG_c( gridX, gridY + 1, unitType );
        IG_c( gridX + 1, gridY + 1, unitType );
        IG_c( gridX + 2, gridY + 1, unitType );
        IG_c( gridX + 3, gridY + 1, unitType );
        IG_c( gridX - 3, gridY, unitType );
        IG_c( gridX - 2, gridY, unitType );
        IG_c( gridX - 1, gridY, unitType );
        IG_c( gridX, gridY, unitType );
        IG_c( gridX + 1, gridY, unitType );
        IG_c( gridX + 2, gridY, unitType );
        IG_c( gridX + 3, gridY, unitType );
        IG_c( gridX - 3, gridY - 1, unitType );
        IG_c( gridX - 2, gridY - 1, unitType );
        IG_c( gridX - 1, gridY - 1, unitType );
        IG_c( gridX, gridY - 1, unitType );
        IG_c( gridX + 1, gridY - 1, unitType );
        IG_c( gridX + 2, gridY - 1, unitType );
        IG_c( gridX + 3, gridY - 1, unitType );
        IG_c( gridX - 3, gridY - 2, unitType );
        IG_c( gridX - 2, gridY - 2, unitType );
        IG_c( gridX - 1, gridY - 2, unitType );
        IG_c( gridX, gridY - 2, unitType );
        IG_c( gridX + 1, gridY - 2, unitType );
        IG_c( gridX + 2, gridY - 2, unitType );
        IG_c( gridX + 3, gridY - 2, unitType );
        IG_c( gridX - 3, gridY - 3, unitType );
        IG_c( gridX - 2, gridY - 3, unitType );
        IG_c( gridX - 1, gridY - 3, unitType );
        IG_c( gridX, gridY - 3, unitType );
        IG_c( gridX + 1, gridY - 3, unitType );
        IG_c( gridX + 2, gridY - 3, unitType );
        IG_c( gridX + 3, gridY - 3, unitType );
    }
    else if( unitSize == 8 )
    {
        IG_c( gridX - 4, gridY + 3, unitType );
        IG_c( gridX - 3, gridY + 3, unitType );
        IG_c( gridX - 2, gridY + 3, unitType );
        IG_c( gridX - 1, gridY + 3, unitType );
        IG_c( gridX, gridY + 3, unitType );
        IG_c( gridX + 1, gridY + 3, unitType );
        IG_c( gridX + 2, gridY + 3, unitType );
        IG_c( gridX + 3, gridY + 3, unitType );
        IG_c( gridX - 4, gridY + 2, unitType );
        IG_c( gridX - 3, gridY + 2, unitType );
        IG_c( gridX - 2, gridY + 2, unitType );
        IG_c( gridX - 1, gridY + 2, unitType );
        IG_c( gridX, gridY + 2, unitType );
        IG_c( gridX + 1, gridY + 2, unitType );
        IG_c( gridX + 2, gridY + 2, unitType );
        IG_c( gridX + 3, gridY + 2, unitType );
        IG_c( gridX - 4, gridY + 1, unitType );
        IG_c( gridX - 3, gridY + 1, unitType );
        IG_c( gridX - 2, gridY + 1, unitType );
        IG_c( gridX - 1, gridY + 1, unitType );
        IG_c( gridX, gridY + 1, unitType );
        IG_c( gridX + 1, gridY + 1, unitType );
        IG_c( gridX + 2, gridY + 1, unitType );
        IG_c( gridX + 3, gridY + 1, unitType );
        IG_c( gridX - 4, gridY, unitType );
        IG_c( gridX - 3, gridY, unitType );
        IG_c( gridX - 2, gridY, unitType );
        IG_c( gridX - 1, gridY, unitType );
        IG_c( gridX, gridY, unitType );
        IG_c( gridX + 1, gridY, unitType );
        IG_c( gridX + 2, gridY, unitType );
        IG_c( gridX + 3, gridY, unitType );
        IG_c( gridX - 4, gridY - 1, unitType );
        IG_c( gridX - 3, gridY - 1, unitType );
        IG_c( gridX - 2, gridY - 1, unitType );
        IG_c( gridX - 1, gridY - 1, unitType );
        IG_c( gridX, gridY - 1, unitType );
        IG_c( gridX + 1, gridY - 1, unitType );
        IG_c( gridX + 2, gridY - 1, unitType );
        IG_c( gridX + 3, gridY - 1, unitType );
        IG_c( gridX - 4, gridY - 2, unitType );
        IG_c( gridX - 3, gridY - 2, unitType );
        IG_c( gridX - 2, gridY - 2, unitType );
        IG_c( gridX - 1, gridY - 2, unitType );
        IG_c( gridX, gridY - 2, unitType );
        IG_c( gridX + 1, gridY - 2, unitType );
        IG_c( gridX + 2, gridY - 2, unitType );
        IG_c( gridX + 3, gridY - 2, unitType );
        IG_c( gridX - 4, gridY - 3, unitType );
        IG_c( gridX - 3, gridY - 3, unitType );
        IG_c( gridX - 2, gridY - 3, unitType );
        IG_c( gridX - 1, gridY - 3, unitType );
        IG_c( gridX, gridY - 3, unitType );
        IG_c( gridX + 1, gridY - 3, unitType );
        IG_c( gridX + 2, gridY - 3, unitType );
        IG_c( gridX + 3, gridY - 3, unitType );
        IG_c( gridX - 4, gridY - 4, unitType );
        IG_c( gridX - 3, gridY - 4, unitType );
        IG_c( gridX - 2, gridY - 4, unitType );
        IG_c( gridX - 1, gridY - 4, unitType );
        IG_c( gridX, gridY - 4, unitType );
        IG_c( gridX + 1, gridY - 4, unitType );
        IG_c( gridX + 2, gridY - 4, unitType );
        IG_c( gridX + 3, gridY - 4, unitType );
    }
    else if( unitSize == 9 )
    {
        IG_c( gridX - 4, gridY + 4, unitType );
        IG_c( gridX - 3, gridY + 4, unitType );
        IG_c( gridX - 2, gridY + 4, unitType );
        IG_c( gridX - 1, gridY + 4, unitType );
        IG_c( gridX, gridY + 4, unitType );
        IG_c( gridX + 1, gridY + 4, unitType );
        IG_c( gridX + 2, gridY + 4, unitType );
        IG_c( gridX + 3, gridY + 4, unitType );
        IG_c( gridX + 4, gridY + 4, unitType );
        IG_c( gridX - 4, gridY + 3, unitType );
        IG_c( gridX - 3, gridY + 3, unitType );
        IG_c( gridX - 2, gridY + 3, unitType );
        IG_c( gridX - 1, gridY + 3, unitType );
        IG_c( gridX, gridY + 3, unitType );
        IG_c( gridX + 1, gridY + 3, unitType );
        IG_c( gridX + 2, gridY + 3, unitType );
        IG_c( gridX + 3, gridY + 3, unitType );
        IG_c( gridX + 4, gridY + 3, unitType );
        IG_c( gridX - 4, gridY + 2, unitType );
        IG_c( gridX - 3, gridY + 2, unitType );
        IG_c( gridX - 2, gridY + 2, unitType );
        IG_c( gridX - 1, gridY + 2, unitType );
        IG_c( gridX, gridY + 2, unitType );
        IG_c( gridX + 1, gridY + 2, unitType );
        IG_c( gridX + 2, gridY + 2, unitType );
        IG_c( gridX + 3, gridY + 2, unitType );
        IG_c( gridX + 4, gridY + 2, unitType );
        IG_c( gridX - 4, gridY + 1, unitType );
        IG_c( gridX - 3, gridY + 1, unitType );
        IG_c( gridX - 2, gridY + 1, unitType );
        IG_c( gridX - 1, gridY + 1, unitType );
        IG_c( gridX, gridY + 1, unitType );
        IG_c( gridX + 1, gridY + 1, unitType );
        IG_c( gridX + 2, gridY + 1, unitType );
        IG_c( gridX + 3, gridY + 1, unitType );
        IG_c( gridX + 4, gridY + 1, unitType );
        IG_c( gridX - 4, gridY, unitType );
        IG_c( gridX - 3, gridY, unitType );
        IG_c( gridX - 2, gridY, unitType );
        IG_c( gridX - 1, gridY, unitType );
        IG_c( gridX, gridY, unitType );
        IG_c( gridX + 1, gridY, unitType );
        IG_c( gridX + 2, gridY, unitType );
        IG_c( gridX + 3, gridY, unitType );
        IG_c( gridX + 4, gridY, unitType );
        IG_c( gridX - 4, gridY - 1, unitType );
        IG_c( gridX - 3, gridY - 1, unitType );
        IG_c( gridX - 2, gridY - 1, unitType );
        IG_c( gridX - 1, gridY - 1, unitType );
        IG_c( gridX, gridY - 1, unitType );
        IG_c( gridX + 1, gridY - 1, unitType );
        IG_c( gridX + 2, gridY - 1, unitType );
        IG_c( gridX + 3, gridY - 1, unitType );
        IG_c( gridX + 4, gridY - 1, unitType );
        IG_c( gridX - 4, gridY - 2, unitType );
        IG_c( gridX - 3, gridY - 2, unitType );
        IG_c( gridX - 2, gridY - 2, unitType );
        IG_c( gridX - 1, gridY - 2, unitType );
        IG_c( gridX, gridY - 2, unitType );
        IG_c( gridX + 1, gridY - 2, unitType );
        IG_c( gridX + 2, gridY - 2, unitType );
        IG_c( gridX + 3, gridY - 2, unitType );
        IG_c( gridX + 4, gridY - 2, unitType );
        IG_c( gridX - 4, gridY - 3, unitType );
        IG_c( gridX - 3, gridY - 3, unitType );
        IG_c( gridX - 2, gridY - 3, unitType );
        IG_c( gridX - 1, gridY - 3, unitType );
        IG_c( gridX, gridY - 3, unitType );
        IG_c( gridX + 1, gridY - 3, unitType );
        IG_c( gridX + 2, gridY - 3, unitType );
        IG_c( gridX + 3, gridY - 3, unitType );
        IG_c( gridX + 4, gridY - 3, unitType );
        IG_c( gridX - 4, gridY - 4, unitType );
        IG_c( gridX - 3, gridY - 4, unitType );
        IG_c( gridX - 2, gridY - 4, unitType );
        IG_c( gridX - 1, gridY - 4, unitType );
        IG_c( gridX, gridY - 4, unitType );
        IG_c( gridX + 1, gridY - 4, unitType );
        IG_c( gridX + 2, gridY - 4, unitType );
        IG_c( gridX + 3, gridY - 4, unitType );
        IG_c( gridX + 4, gridY - 4, unitType );
    }
    else if( unitSize == 10 )
    {
        IG_c( gridX - 5, gridY + 5, unitType );
        IG_c( gridX - 4, gridY + 5, unitType );
        IG_c( gridX - 3, gridY + 5, unitType );
        IG_c( gridX - 2, gridY + 5, unitType );
        IG_c( gridX - 1, gridY + 5, unitType );
        IG_c( gridX, gridY + 5, unitType );
        IG_c( gridX + 1, gridY + 5, unitType );
        IG_c( gridX + 2, gridY + 5, unitType );
        IG_c( gridX + 3, gridY + 5, unitType );
        IG_c( gridX + 4, gridY + 5, unitType );
        IG_c( gridX - 5, gridY + 4, unitType );
        IG_c( gridX - 4, gridY + 4, unitType );
        IG_c( gridX - 3, gridY + 4, unitType );
        IG_c( gridX - 2, gridY + 4, unitType );
        IG_c( gridX - 1, gridY + 4, unitType );
        IG_c( gridX, gridY + 4, unitType );
        IG_c( gridX + 1, gridY + 4, unitType );
        IG_c( gridX + 2, gridY + 4, unitType );
        IG_c( gridX + 3, gridY + 4, unitType );
        IG_c( gridX + 4, gridY + 4, unitType );
        IG_c( gridX - 5, gridY + 3, unitType );
        IG_c( gridX - 4, gridY + 3, unitType );
        IG_c( gridX - 3, gridY + 3, unitType );
        IG_c( gridX - 2, gridY + 3, unitType );
        IG_c( gridX - 1, gridY + 3, unitType );
        IG_c( gridX, gridY + 3, unitType );
        IG_c( gridX + 1, gridY + 3, unitType );
        IG_c( gridX + 2, gridY + 3, unitType );
        IG_c( gridX + 3, gridY + 3, unitType );
        IG_c( gridX + 4, gridY + 3, unitType );
        IG_c( gridX - 5, gridY + 2, unitType );
        IG_c( gridX - 4, gridY + 2, unitType );
        IG_c( gridX - 3, gridY + 2, unitType );
        IG_c( gridX - 2, gridY + 2, unitType );
        IG_c( gridX - 1, gridY + 2, unitType );
        IG_c( gridX, gridY + 2, unitType );
        IG_c( gridX + 1, gridY + 2, unitType );
        IG_c( gridX + 2, gridY + 2, unitType );
        IG_c( gridX + 3, gridY + 2, unitType );
        IG_c( gridX + 4, gridY + 2, unitType );
        IG_c( gridX - 5, gridY + 1, unitType );
        IG_c( gridX - 4, gridY + 1, unitType );
        IG_c( gridX - 3, gridY + 1, unitType );
        IG_c( gridX - 2, gridY + 1, unitType );
        IG_c( gridX - 1, gridY + 1, unitType );
        IG_c( gridX, gridY + 1, unitType );
        IG_c( gridX + 1, gridY + 1, unitType );
        IG_c( gridX + 2, gridY + 1, unitType );
        IG_c( gridX + 3, gridY + 1, unitType );
        IG_c( gridX + 4, gridY + 1, unitType );
        IG_c( gridX - 5, gridY, unitType );
        IG_c( gridX - 4, gridY, unitType );
        IG_c( gridX - 3, gridY, unitType );
        IG_c( gridX - 2, gridY, unitType );
        IG_c( gridX - 1, gridY, unitType );
        IG_c( gridX, gridY, unitType );
        IG_c( gridX + 1, gridY, unitType );
        IG_c( gridX + 2, gridY, unitType );
        IG_c( gridX + 3, gridY, unitType );
        IG_c( gridX + 4, gridY, unitType );
        IG_c( gridX - 5, gridY - 1, unitType );
        IG_c( gridX - 4, gridY - 1, unitType );
        IG_c( gridX - 3, gridY - 1, unitType );
        IG_c( gridX - 2, gridY - 1, unitType );
        IG_c( gridX - 1, gridY - 1, unitType );
        IG_c( gridX, gridY - 1, unitType );
        IG_c( gridX + 1, gridY - 1, unitType );
        IG_c( gridX + 2, gridY - 1, unitType );
        IG_c( gridX + 3, gridY - 1, unitType );
        IG_c( gridX + 4, gridY - 1, unitType );
        IG_c( gridX - 5, gridY - 2, unitType );
        IG_c( gridX - 4, gridY - 2, unitType );
        IG_c( gridX - 3, gridY - 2, unitType );
        IG_c( gridX - 2, gridY - 2, unitType );
        IG_c( gridX - 1, gridY - 2, unitType );
        IG_c( gridX, gridY - 2, unitType );
        IG_c( gridX + 1, gridY - 2, unitType );
        IG_c( gridX + 2, gridY - 2, unitType );
        IG_c( gridX + 3, gridY - 2, unitType );
        IG_c( gridX + 4, gridY - 2, unitType );
        IG_c( gridX - 5, gridY - 3, unitType );
        IG_c( gridX - 4, gridY - 3, unitType );
        IG_c( gridX - 3, gridY - 3, unitType );
        IG_c( gridX - 2, gridY - 3, unitType );
        IG_c( gridX - 1, gridY - 3, unitType );
        IG_c( gridX, gridY - 3, unitType );
        IG_c( gridX + 1, gridY - 3, unitType );
        IG_c( gridX + 2, gridY - 3, unitType );
        IG_c( gridX + 3, gridY - 3, unitType );
        IG_c( gridX + 4, gridY - 3, unitType );
        IG_c( gridX - 5, gridY - 4, unitType );
        IG_c( gridX - 4, gridY - 4, unitType );
        IG_c( gridX - 3, gridY - 4, unitType );
        IG_c( gridX - 2, gridY - 4, unitType );
        IG_c( gridX - 1, gridY - 4, unitType );
        IG_c( gridX, gridY - 4, unitType );
        IG_c( gridX + 1, gridY - 4, unitType );
        IG_c( gridX + 2, gridY - 4, unitType );
        IG_c( gridX + 3, gridY - 4, unitType );
        IG_c( gridX + 4, gridY - 4, unitType );
    }
    else
        //do nothing
    {
        return;
    }
}


void SetGridBlock( int iMapIndex, int posX, int posY, int unitSize, char unitType )
{
	SetCurMapDataBlock_i( iMapIndex ); 

    Vec2 stVecTmp;

    if( ( unitSize * 0x1 ) == 0 ) //偶数
    {
        posX = posX + PER_CELL / 2;
        posY = posY + PER_CELL / 2;
    }


    PosToGrid( posX, posY, &stVecTmp );
    int gridX = stVecTmp.x;
    int gridY = stVecTmp.y;

    if( unitSize == 1 )
    {
        IG_b( gridX, gridY, unitType );
    }

    else if( unitSize == 2 )
    {
        IG_b( gridX - 1, gridY, unitType );
        IG_b( gridX, gridY, unitType );
        IG_b( gridX - 1, gridY - 1, unitType );
        IG_b( gridX, gridY - 1, unitType );
    }
    else if( unitSize == 3 )
    {
        IG_b( gridX - 1, gridY + 1, unitType );
        IG_b( gridX, gridY + 1, unitType );
        IG_b( gridX + 1, gridY + 1, unitType );
        IG_b( gridX - 1, gridY, unitType );
        IG_b( gridX, gridY, unitType );
        IG_b( gridX + 1, gridY, unitType );
        IG_b( gridX - 1, gridY - 1, unitType );
        IG_b( gridX, gridY - 1, unitType );
        IG_b( gridX + 1, gridY - 1, unitType );
    }
    else if( unitSize == 4 )
    {
        IG_b( gridX - 2, gridY + 2, unitType );
        IG_b( gridX - 1, gridY + 2, unitType );
        IG_b( gridX, gridY + 2, unitType );
        IG_b( gridX + 1, gridY + 2, unitType );
        IG_b( gridX - 2, gridY + 1, unitType );
        IG_b( gridX - 1, gridY + 1, unitType );
        IG_b( gridX, gridY + 1, unitType );
        IG_b( gridX + 1, gridY + 1, unitType );
        IG_b( gridX - 2, gridY, unitType );
        IG_b( gridX - 1, gridY, unitType );
        IG_b( gridX, gridY, unitType );
        IG_b( gridX + 1, gridY, unitType );
        IG_b( gridX - 2, gridY - 1, unitType );
        IG_b( gridX - 1, gridY - 1, unitType );
        IG_b( gridX, gridY - 1, unitType );
        IG_b( gridX + 1, gridY - 1, unitType );
    }
    else if( unitSize == 5 )
    {
        IG_b( gridX - 2, gridY + 2, unitType );
        IG_b( gridX - 1, gridY + 2, unitType );
        IG_b( gridX, gridY + 2, unitType );
        IG_b( gridX + 1, gridY + 2, unitType );
        IG_b( gridX + 2, gridY + 2, unitType );
        IG_b( gridX - 2, gridY + 1, unitType );
        IG_b( gridX - 1, gridY + 1, unitType );
        IG_b( gridX, gridY + 1, unitType );
        IG_b( gridX + 1, gridY + 1, unitType );
        IG_b( gridX + 2, gridY + 1, unitType );
        IG_b( gridX - 2, gridY, unitType );
        IG_b( gridX - 1, gridY, unitType );
        IG_b( gridX, gridY, unitType );
        IG_b( gridX + 1, gridY, unitType );
        IG_b( gridX + 2, gridY, unitType );
        IG_b( gridX - 2, gridY - 1, unitType );
        IG_b( gridX - 1, gridY - 1, unitType );
        IG_b( gridX, gridY - 1, unitType );
        IG_b( gridX + 1, gridY - 1, unitType );
        IG_b( gridX + 2, gridY - 1, unitType );
        IG_b( gridX - 2, gridY - 2, unitType );
        IG_b( gridX - 1, gridY - 2, unitType );
        IG_b( gridX, gridY - 2, unitType );
        IG_b( gridX + 1, gridY - 2, unitType );
        IG_b( gridX + 2, gridY - 2, unitType );
    }
    else if( unitSize == 6 )
    {
        IG_b( gridX - 3, gridY + 3, unitType );
        IG_b( gridX - 2, gridY + 3, unitType );
        IG_b( gridX - 1, gridY + 3, unitType );
        IG_b( gridX, gridY + 3, unitType );
        IG_b( gridX + 1, gridY + 3, unitType );
        IG_b( gridX + 2, gridY + 3, unitType );
        IG_b( gridX - 3, gridY + 2, unitType );
        IG_b( gridX - 2, gridY + 2, unitType );
        IG_b( gridX - 1, gridY + 2, unitType );
        IG_b( gridX, gridY + 2, unitType );
        IG_b( gridX + 1, gridY + 2, unitType );
        IG_b( gridX + 2, gridY + 2, unitType );
        IG_b( gridX - 3, gridY + 1, unitType );
        IG_b( gridX - 2, gridY + 1, unitType );
        IG_b( gridX - 1, gridY + 1, unitType );
        IG_b( gridX, gridY + 1, unitType );
        IG_b( gridX + 1, gridY + 1, unitType );
        IG_b( gridX + 2, gridY + 1, unitType );
        IG_b( gridX - 3, gridY, unitType );
        IG_b( gridX - 2, gridY, unitType );
        IG_b( gridX - 1, gridY, unitType );
        IG_b( gridX, gridY, unitType );
        IG_b( gridX + 1, gridY, unitType );
        IG_b( gridX + 2, gridY, unitType );
        IG_b( gridX - 3, gridY - 1, unitType );
        IG_b( gridX - 2, gridY - 1, unitType );
        IG_b( gridX - 1, gridY - 1, unitType );
        IG_b( gridX, gridY - 1, unitType );
        IG_b( gridX + 1, gridY - 1, unitType );
        IG_b( gridX + 2, gridY - 1, unitType );
        IG_b( gridX - 3, gridY - 2, unitType );
        IG_b( gridX - 2, gridY - 2, unitType );
        IG_b( gridX - 1, gridY - 2, unitType );
        IG_b( gridX, gridY - 2, unitType );
        IG_b( gridX + 1, gridY - 2, unitType );
        IG_b( gridX + 2, gridY - 2, unitType );
    }
    else if( unitSize == 7 )
    {
        IG_b( gridX - 3, gridY + 3, unitType );
        IG_b( gridX - 2, gridY + 3, unitType );
        IG_b( gridX - 1, gridY + 3, unitType );
        IG_b( gridX, gridY + 3, unitType );
        IG_b( gridX + 1, gridY + 3, unitType );
        IG_b( gridX + 2, gridY + 3, unitType );
        IG_b( gridX + 3, gridY + 3, unitType );
        IG_b( gridX - 3, gridY + 2, unitType );
        IG_b( gridX - 2, gridY + 2, unitType );
        IG_b( gridX - 1, gridY + 2, unitType );
        IG_b( gridX, gridY + 2, unitType );
        IG_b( gridX + 1, gridY + 2, unitType );
        IG_b( gridX + 2, gridY + 2, unitType );
        IG_b( gridX + 3, gridY + 2, unitType );
        IG_b( gridX - 3, gridY + 1, unitType );
        IG_b( gridX - 2, gridY + 1, unitType );
        IG_b( gridX - 1, gridY + 1, unitType );
        IG_b( gridX, gridY + 1, unitType );
        IG_b( gridX + 1, gridY + 1, unitType );
        IG_b( gridX + 2, gridY + 1, unitType );
        IG_b( gridX + 3, gridY + 1, unitType );
        IG_b( gridX - 3, gridY, unitType );
        IG_b( gridX - 2, gridY, unitType );
        IG_b( gridX - 1, gridY, unitType );
        IG_b( gridX, gridY, unitType );
        IG_b( gridX + 1, gridY, unitType );
        IG_b( gridX + 2, gridY, unitType );
        IG_b( gridX + 3, gridY, unitType );
        IG_b( gridX - 3, gridY - 1, unitType );
        IG_b( gridX - 2, gridY - 1, unitType );
        IG_b( gridX - 1, gridY - 1, unitType );
        IG_b( gridX, gridY - 1, unitType );
        IG_b( gridX + 1, gridY - 1, unitType );
        IG_b( gridX + 2, gridY - 1, unitType );
        IG_b( gridX + 3, gridY - 1, unitType );
        IG_b( gridX - 3, gridY - 2, unitType );
        IG_b( gridX - 2, gridY - 2, unitType );
        IG_b( gridX - 1, gridY - 2, unitType );
        IG_b( gridX, gridY - 2, unitType );
        IG_b( gridX + 1, gridY - 2, unitType );
        IG_b( gridX + 2, gridY - 2, unitType );
        IG_b( gridX + 3, gridY - 2, unitType );
        IG_b( gridX - 3, gridY - 3, unitType );
        IG_b( gridX - 2, gridY - 3, unitType );
        IG_b( gridX - 1, gridY - 3, unitType );
        IG_b( gridX, gridY - 3, unitType );
        IG_b( gridX + 1, gridY - 3, unitType );
        IG_b( gridX + 2, gridY - 3, unitType );
        IG_b( gridX + 3, gridY - 3, unitType );
    }
    else if( unitSize == 8 )
    {
        IG_b( gridX - 4, gridY + 3, unitType );
        IG_b( gridX - 3, gridY + 3, unitType );
        IG_b( gridX - 2, gridY + 3, unitType );
        IG_b( gridX - 1, gridY + 3, unitType );
        IG_b( gridX, gridY + 3, unitType );
        IG_b( gridX + 1, gridY + 3, unitType );
        IG_b( gridX + 2, gridY + 3, unitType );
        IG_b( gridX + 3, gridY + 3, unitType );
        IG_b( gridX - 4, gridY + 2, unitType );
        IG_b( gridX - 3, gridY + 2, unitType );
        IG_b( gridX - 2, gridY + 2, unitType );
        IG_b( gridX - 1, gridY + 2, unitType );
        IG_b( gridX, gridY + 2, unitType );
        IG_b( gridX + 1, gridY + 2, unitType );
        IG_b( gridX + 2, gridY + 2, unitType );
        IG_b( gridX + 3, gridY + 2, unitType );
        IG_b( gridX - 4, gridY + 1, unitType );
        IG_b( gridX - 3, gridY + 1, unitType );
        IG_b( gridX - 2, gridY + 1, unitType );
        IG_b( gridX - 1, gridY + 1, unitType );
        IG_b( gridX, gridY + 1, unitType );
        IG_b( gridX + 1, gridY + 1, unitType );
        IG_b( gridX + 2, gridY + 1, unitType );
        IG_b( gridX + 3, gridY + 1, unitType );
        IG_b( gridX - 4, gridY, unitType );
        IG_b( gridX - 3, gridY, unitType );
        IG_b( gridX - 2, gridY, unitType );
        IG_b( gridX - 1, gridY, unitType );
        IG_b( gridX, gridY, unitType );
        IG_b( gridX + 1, gridY, unitType );
        IG_b( gridX + 2, gridY, unitType );
        IG_b( gridX + 3, gridY, unitType );
        IG_b( gridX - 4, gridY - 1, unitType );
        IG_b( gridX - 3, gridY - 1, unitType );
        IG_b( gridX - 2, gridY - 1, unitType );
        IG_b( gridX - 1, gridY - 1, unitType );
        IG_b( gridX, gridY - 1, unitType );
        IG_b( gridX + 1, gridY - 1, unitType );
        IG_b( gridX + 2, gridY - 1, unitType );
        IG_b( gridX + 3, gridY - 1, unitType );
        IG_b( gridX - 4, gridY - 2, unitType );
        IG_b( gridX - 3, gridY - 2, unitType );
        IG_b( gridX - 2, gridY - 2, unitType );
        IG_b( gridX - 1, gridY - 2, unitType );
        IG_b( gridX, gridY - 2, unitType );
        IG_b( gridX + 1, gridY - 2, unitType );
        IG_b( gridX + 2, gridY - 2, unitType );
        IG_b( gridX + 3, gridY - 2, unitType );
        IG_b( gridX - 4, gridY - 3, unitType );
        IG_b( gridX - 3, gridY - 3, unitType );
        IG_b( gridX - 2, gridY - 3, unitType );
        IG_b( gridX - 1, gridY - 3, unitType );
        IG_b( gridX, gridY - 3, unitType );
        IG_b( gridX + 1, gridY - 3, unitType );
        IG_b( gridX + 2, gridY - 3, unitType );
        IG_b( gridX + 3, gridY - 3, unitType );
        IG_b( gridX - 4, gridY - 4, unitType );
        IG_b( gridX - 3, gridY - 4, unitType );
        IG_b( gridX - 2, gridY - 4, unitType );
        IG_b( gridX - 1, gridY - 4, unitType );
        IG_b( gridX, gridY - 4, unitType );
        IG_b( gridX + 1, gridY - 4, unitType );
        IG_b( gridX + 2, gridY - 4, unitType );
        IG_b( gridX + 3, gridY - 4, unitType );
    }
    else if( unitSize == 9 )
    {
        IG_b( gridX - 4, gridY + 4, unitType );
        IG_b( gridX - 3, gridY + 4, unitType );
        IG_b( gridX - 2, gridY + 4, unitType );
        IG_b( gridX - 1, gridY + 4, unitType );
        IG_b( gridX, gridY + 4, unitType );
        IG_b( gridX + 1, gridY + 4, unitType );
        IG_b( gridX + 2, gridY + 4, unitType );
        IG_b( gridX + 3, gridY + 4, unitType );
        IG_b( gridX + 4, gridY + 4, unitType );
        IG_b( gridX - 4, gridY + 3, unitType );
        IG_b( gridX - 3, gridY + 3, unitType );
        IG_b( gridX - 2, gridY + 3, unitType );
        IG_b( gridX - 1, gridY + 3, unitType );
        IG_b( gridX, gridY + 3, unitType );
        IG_b( gridX + 1, gridY + 3, unitType );
        IG_b( gridX + 2, gridY + 3, unitType );
        IG_b( gridX + 3, gridY + 3, unitType );
        IG_b( gridX + 4, gridY + 3, unitType );
        IG_b( gridX - 4, gridY + 2, unitType );
        IG_b( gridX - 3, gridY + 2, unitType );
        IG_b( gridX - 2, gridY + 2, unitType );
        IG_b( gridX - 1, gridY + 2, unitType );
        IG_b( gridX, gridY + 2, unitType );
        IG_b( gridX + 1, gridY + 2, unitType );
        IG_b( gridX + 2, gridY + 2, unitType );
        IG_b( gridX + 3, gridY + 2, unitType );
        IG_b( gridX + 4, gridY + 2, unitType );
        IG_b( gridX - 4, gridY + 1, unitType );
        IG_b( gridX - 3, gridY + 1, unitType );
        IG_b( gridX - 2, gridY + 1, unitType );
        IG_b( gridX - 1, gridY + 1, unitType );
        IG_b( gridX, gridY + 1, unitType );
        IG_b( gridX + 1, gridY + 1, unitType );
        IG_b( gridX + 2, gridY + 1, unitType );
        IG_b( gridX + 3, gridY + 1, unitType );
        IG_b( gridX + 4, gridY + 1, unitType );
        IG_b( gridX - 4, gridY, unitType );
        IG_b( gridX - 3, gridY, unitType );
        IG_b( gridX - 2, gridY, unitType );
        IG_b( gridX - 1, gridY, unitType );
        IG_b( gridX, gridY, unitType );
        IG_b( gridX + 1, gridY, unitType );
        IG_b( gridX + 2, gridY, unitType );
        IG_b( gridX + 3, gridY, unitType );
        IG_b( gridX + 4, gridY, unitType );
        IG_b( gridX - 4, gridY - 1, unitType );
        IG_b( gridX - 3, gridY - 1, unitType );
        IG_b( gridX - 2, gridY - 1, unitType );
        IG_b( gridX - 1, gridY - 1, unitType );
        IG_b( gridX, gridY - 1, unitType );
        IG_b( gridX + 1, gridY - 1, unitType );
        IG_b( gridX + 2, gridY - 1, unitType );
        IG_b( gridX + 3, gridY - 1, unitType );
        IG_b( gridX + 4, gridY - 1, unitType );
        IG_b( gridX - 4, gridY - 2, unitType );
        IG_b( gridX - 3, gridY - 2, unitType );
        IG_b( gridX - 2, gridY - 2, unitType );
        IG_b( gridX - 1, gridY - 2, unitType );
        IG_b( gridX, gridY - 2, unitType );
        IG_b( gridX + 1, gridY - 2, unitType );
        IG_b( gridX + 2, gridY - 2, unitType );
        IG_b( gridX + 3, gridY - 2, unitType );
        IG_b( gridX + 4, gridY - 2, unitType );
        IG_b( gridX - 4, gridY - 3, unitType );
        IG_b( gridX - 3, gridY - 3, unitType );
        IG_b( gridX - 2, gridY - 3, unitType );
        IG_b( gridX - 1, gridY - 3, unitType );
        IG_b( gridX, gridY - 3, unitType );
        IG_b( gridX + 1, gridY - 3, unitType );
        IG_b( gridX + 2, gridY - 3, unitType );
        IG_b( gridX + 3, gridY - 3, unitType );
        IG_b( gridX + 4, gridY - 3, unitType );
        IG_b( gridX - 4, gridY - 4, unitType );
        IG_b( gridX - 3, gridY - 4, unitType );
        IG_b( gridX - 2, gridY - 4, unitType );
        IG_b( gridX - 1, gridY - 4, unitType );
        IG_b( gridX, gridY - 4, unitType );
        IG_b( gridX + 1, gridY - 4, unitType );
        IG_b( gridX + 2, gridY - 4, unitType );
        IG_b( gridX + 3, gridY - 4, unitType );
        IG_b( gridX + 4, gridY - 4, unitType );
    }
    else if( unitSize == 10 )
    {
        IG_b( gridX - 5, gridY + 5, unitType );
        IG_b( gridX - 4, gridY + 5, unitType );
        IG_b( gridX - 3, gridY + 5, unitType );
        IG_b( gridX - 2, gridY + 5, unitType );
        IG_b( gridX - 1, gridY + 5, unitType );
        IG_b( gridX, gridY + 5, unitType );
        IG_b( gridX + 1, gridY + 5, unitType );
        IG_b( gridX + 2, gridY + 5, unitType );
        IG_b( gridX + 3, gridY + 5, unitType );
        IG_b( gridX + 4, gridY + 5, unitType );
        IG_b( gridX - 5, gridY + 4, unitType );
        IG_b( gridX - 4, gridY + 4, unitType );
        IG_b( gridX - 3, gridY + 4, unitType );
        IG_b( gridX - 2, gridY + 4, unitType );
        IG_b( gridX - 1, gridY + 4, unitType );
        IG_b( gridX, gridY + 4, unitType );
        IG_b( gridX + 1, gridY + 4, unitType );
        IG_b( gridX + 2, gridY + 4, unitType );
        IG_b( gridX + 3, gridY + 4, unitType );
        IG_b( gridX + 4, gridY + 4, unitType );
        IG_b( gridX - 5, gridY + 3, unitType );
        IG_b( gridX - 4, gridY + 3, unitType );
        IG_b( gridX - 3, gridY + 3, unitType );
        IG_b( gridX - 2, gridY + 3, unitType );
        IG_b( gridX - 1, gridY + 3, unitType );
        IG_b( gridX, gridY + 3, unitType );
        IG_b( gridX + 1, gridY + 3, unitType );
        IG_b( gridX + 2, gridY + 3, unitType );
        IG_b( gridX + 3, gridY + 3, unitType );
        IG_b( gridX + 4, gridY + 3, unitType );
        IG_b( gridX - 5, gridY + 2, unitType );
        IG_b( gridX - 4, gridY + 2, unitType );
        IG_b( gridX - 3, gridY + 2, unitType );
        IG_b( gridX - 2, gridY + 2, unitType );
        IG_b( gridX - 1, gridY + 2, unitType );
        IG_b( gridX, gridY + 2, unitType );
        IG_b( gridX + 1, gridY + 2, unitType );
        IG_b( gridX + 2, gridY + 2, unitType );
        IG_b( gridX + 3, gridY + 2, unitType );
        IG_b( gridX + 4, gridY + 2, unitType );
        IG_b( gridX - 5, gridY + 1, unitType );
        IG_b( gridX - 4, gridY + 1, unitType );
        IG_b( gridX - 3, gridY + 1, unitType );
        IG_b( gridX - 2, gridY + 1, unitType );
        IG_b( gridX - 1, gridY + 1, unitType );
        IG_b( gridX, gridY + 1, unitType );
        IG_b( gridX + 1, gridY + 1, unitType );
        IG_b( gridX + 2, gridY + 1, unitType );
        IG_b( gridX + 3, gridY + 1, unitType );
        IG_b( gridX + 4, gridY + 1, unitType );
        IG_b( gridX - 5, gridY, unitType );
        IG_b( gridX - 4, gridY, unitType );
        IG_b( gridX - 3, gridY, unitType );
        IG_b( gridX - 2, gridY, unitType );
        IG_b( gridX - 1, gridY, unitType );
        IG_b( gridX, gridY, unitType );
        IG_b( gridX + 1, gridY, unitType );
        IG_b( gridX + 2, gridY, unitType );
        IG_b( gridX + 3, gridY, unitType );
        IG_b( gridX + 4, gridY, unitType );
        IG_b( gridX - 5, gridY - 1, unitType );
        IG_b( gridX - 4, gridY - 1, unitType );
        IG_b( gridX - 3, gridY - 1, unitType );
        IG_b( gridX - 2, gridY - 1, unitType );
        IG_b( gridX - 1, gridY - 1, unitType );
        IG_b( gridX, gridY - 1, unitType );
        IG_b( gridX + 1, gridY - 1, unitType );
        IG_b( gridX + 2, gridY - 1, unitType );
        IG_b( gridX + 3, gridY - 1, unitType );
        IG_b( gridX + 4, gridY - 1, unitType );
        IG_b( gridX - 5, gridY - 2, unitType );
        IG_b( gridX - 4, gridY - 2, unitType );
        IG_b( gridX - 3, gridY - 2, unitType );
        IG_b( gridX - 2, gridY - 2, unitType );
        IG_b( gridX - 1, gridY - 2, unitType );
        IG_b( gridX, gridY - 2, unitType );
        IG_b( gridX + 1, gridY - 2, unitType );
        IG_b( gridX + 2, gridY - 2, unitType );
        IG_b( gridX + 3, gridY - 2, unitType );
        IG_b( gridX + 4, gridY - 2, unitType );
        IG_b( gridX - 5, gridY - 3, unitType );
        IG_b( gridX - 4, gridY - 3, unitType );
        IG_b( gridX - 3, gridY - 3, unitType );
        IG_b( gridX - 2, gridY - 3, unitType );
        IG_b( gridX - 1, gridY - 3, unitType );
        IG_b( gridX, gridY - 3, unitType );
        IG_b( gridX + 1, gridY - 3, unitType );
        IG_b( gridX + 2, gridY - 3, unitType );
        IG_b( gridX + 3, gridY - 3, unitType );
        IG_b( gridX + 4, gridY - 3, unitType );
        IG_b( gridX - 5, gridY - 4, unitType );
        IG_b( gridX - 4, gridY - 4, unitType );
        IG_b( gridX - 3, gridY - 4, unitType );
        IG_b( gridX - 2, gridY - 4, unitType );
        IG_b( gridX - 1, gridY - 4, unitType );
        IG_b( gridX, gridY - 4, unitType );
        IG_b( gridX + 1, gridY - 4, unitType );
        IG_b( gridX + 2, gridY - 4, unitType );
        IG_b( gridX + 3, gridY - 4, unitType );
        IG_b( gridX + 4, gridY - 4, unitType );
    }
    else
        //do nothing
    {
        return;
    }
}


unsigned char IsLineThrough( int posBeginX, int posBeginY, int posEndX, int posEndY, char unitSize, char unitType, ST_LINE_CHECK_RESULT *pOutRetPos )
{
    //WOOD_PRINT( "After InitMap Block Check [100,20 %d] [1,1 %d]\n", gstMapBlock.aBlock[100][20], gstMapBlock.aBlock[1][1] );

    Vec2 stVecTmp;

    PosToGrid( posBeginX, posBeginY, &stVecTmp );
    int beginGX = stVecTmp.x;
    int beginGY = stVecTmp.y;

    if( IsGridFree_i( beginGX, beginGY, unitSize, unitType ) == 0 )
    {
        return 0;
    }

    PosToGrid( posEndX, posEndY, &stVecTmp );
    int endGX = stVecTmp.x;
    int endGY = stVecTmp.y;

	if( IsGridFree_i( endGX, endGY, unitSize, unitType ) == 0 )
    {
        return 0;
    }

    //WOOD_PRINT( "Begin %d,%d %d,%d End %d,%d %d,%d \n", posBeginX, posBeginY, beginGX, beginGY, posEndX, posEndY, endGX, endGY );

    ST_LINE_CHECK_RESULT stCheckRet;
    ST_LINE_CHECK_RESULT *retPos = NULL;

    if( pOutRetPos != NULL )
    {
        memset( pOutRetPos, 0, sizeof( ST_LINE_CHECK_RESULT ) );
        retPos = pOutRetPos;
    }
    else
    {
        retPos = &stCheckRet;
    }


    int curGX = beginGX;
    int curGY = beginGY;
    int preGX = beginGX;
    int preGY = beginGY;

    int deltaGX = endGX - beginGX;
    int deltaGY = endGY - beginGY;

    float fOmega = 0.01f;
    float fEpsilon = 0.0001f;

    int curX = 0;
    int curY = 0;

    if( abs( deltaGX ) != 0 )
    {
        float k = ( float )( posEndY - posBeginY );
        k = k / ( posEndX - posBeginX );

        if( fabs( k ) <= 1.0f )
        {
            float fX = 0.0f;
            float fY = 0.0f;

            if( deltaGX > 0 )
            {
                fX = ( curGX + 1 ) * PER_CELL - fOmega;
            }
            else
            {
                fX = curGX * PER_CELL + fOmega;
            }

            fY = k * ( fX - posBeginX ) + posBeginY;
            curGY = ( int )( fY / PER_CELL + fEpsilon );

            curX = ( int )( ( curGX + 0.5 ) * PER_CELL );
            curY = ( int )( ( curGY + 0.5 ) * PER_CELL );

            if( curGY != preGY )
            {
				if( IsGridFree_i( curGX, curGY, unitSize, unitType ) == 0 )
                {
                    retPos->firstBlockPosX = curX;
                    retPos->firstBlockPosY = curY;
                    return 0;
                }

                preGY = curGY;
            }

            if( curGX == endGX && curGY == endGY )
            {
                retPos->lastFreePosX = posEndX;
                retPos->lastFreePosY = posEndY;
                return 1;
            }

            retPos->lastFreePosX = curX;
            retPos->lastFreePosY = curY;

            while( 1 )
            {
                if( deltaGX > 0 )
                {
                    curGX = curGX + 1;
                    fX = curGX * PER_CELL + fOmega;
                }
                else
                {
                    curGX = curGX - 1;
                    fX = ( curGX + 1 ) * PER_CELL - fOmega;
                }

                fY = k * ( fX - posBeginX ) + posBeginY;
                curGY = ( int )( ( fY / PER_CELL + fEpsilon ) );

                curX = ( int )( ( curGX + 0.5 ) * PER_CELL );
                curY = ( int )( ( curGY + 0.5 ) * PER_CELL );

				if( IsGridFree_i( curGX, curGY, unitSize, unitType ) == 0 )
                {
                    retPos->firstBlockPosX = curX;
                    retPos->firstBlockPosY = curY;
                    return 0;
                }

                if( curGX == endGX && curGY == endGY )
                {
                    retPos->lastFreePosX = posEndX;
                    retPos->lastFreePosY = posEndY;
                    return 1;
                }

                retPos->lastFreePosX = curX;
                retPos->lastFreePosY = curY;

                preGX = curGX;
                preGY = curGY;

                if( deltaGX > 0 )
                {
                    fX = ( curGX + 1 ) * PER_CELL + fOmega;
                }
                else
                {
                    fX = curGX * PER_CELL - fOmega;
                }

                fY = k * ( fX - posBeginX ) + posBeginY;
                curGY = ( int )( fY / PER_CELL + fEpsilon );

                curX = ( int )( ( curGX + 0.5 ) * PER_CELL );
                curY = ( int )( ( curGY + 0.5 ) * PER_CELL );

                if( curGY != preGY )
                {
					if( IsGridFree_i( curGX, curGY, unitSize, unitType ) == 0 )
                    {
                        retPos->firstBlockPosX = curX;
                        retPos->firstBlockPosY = curY;
                        return 0;
                    }

                    preGY = curGY;
                }

                if( curGX == endGX )
                {
                    if( curGY == endGY )
                    {
                        retPos->lastFreePosX = posEndX;
                        retPos->lastFreePosY = posEndY;
                        return 1;
                    }
                    else
                    {
                        retPos->firstBlockPosX = curX;
                        retPos->firstBlockPosY = curY;
                        return 0;
                    }
                }

                retPos->lastFreePosX = curX;
                retPos->lastFreePosY = curY;
            }
        }
        else
        {

            float fX = 0.0f;
            float fY = 0.0f;

            if( deltaGY > 0 )
            {
                fY = ( curGY + 1 ) * PER_CELL - fOmega;
            }
            else
            {
                fY = curGY * PER_CELL + fOmega;
            }

            fX = ( fY - posBeginY ) / k + posBeginX;
            curGX = ( int )( fX / PER_CELL + fEpsilon );

            curX = ( int )( ( curGX + 0.5 ) * PER_CELL );
            curY = ( int )( ( curGY + 0.5 ) * PER_CELL );

            if( curGX != preGX )
            {
				if( IsGridFree_i( curGX, curGY, unitSize, unitType ) == 0 )
                {
                    retPos->firstBlockPosX = curX;
                    retPos->firstBlockPosY = curY;
                    return 0;
                }

                preGX = curGX;
            }

            if( curGX == endGX && curGY == endGY )
            {
                retPos->lastFreePosX = posEndX;
                retPos->lastFreePosY = posEndY;
                return 1;
            }

            retPos->lastFreePosX = curX;
            retPos->lastFreePosY = curY;

            while( 1 )
            {

                if( deltaGY > 0 )
                {
                    curGY = curGY + 1;
                    fY = curGY * PER_CELL + fOmega;
                }
                else
                {
                    curGY = curGY - 1;
                    fY = ( curGY + 1 ) * PER_CELL - fOmega;
                }

                fX = ( fY - posBeginY ) / k + posBeginX;
                curGX = ( int )( fX / PER_CELL + fEpsilon );

                curX = ( int )( ( curGX + 0.5 ) * PER_CELL );
                curY = ( int )( ( curGY + 0.5 ) * PER_CELL );

				if( IsGridFree_i( curGX, curGY, unitSize, unitType ) == 0 )
                {
                    retPos->firstBlockPosX = curX;
                    retPos->firstBlockPosY = curY;
                    return 0;
                }

                if( curGX == endGX && curGY == endGY )
                {
                    retPos->lastFreePosX = posEndX;
                    retPos->lastFreePosY = posEndY;
                    return 1;
                }

                retPos->lastFreePosX = curX;
                retPos->lastFreePosY = curY;

                preGX = curGX;
                preGY = curGY;

                if( deltaGY > 0 )
                {
                    fY = ( curGY + 1 ) * PER_CELL - fOmega;
                }
                else
                {
                    //curGY = curGY - 1;
                    fY = curGY * PER_CELL + fOmega;
                }

                fX = ( fY - posBeginY ) / k + posBeginX;
                curGX = ( int )( fX / PER_CELL + fEpsilon );

                curX = ( int )( ( curGX + 0.5 ) * PER_CELL );
                curY = ( int )( ( curGY + 0.5 ) * PER_CELL );

                if( curGX != preGX )
                {
					if( IsGridFree_i( curGX, curGY, unitSize, unitType ) == 0 )
                    {
                        retPos->firstBlockPosX = curX;
                        retPos->firstBlockPosY = curY;
                        return 0;
                    }

                    preGX = curGX;
                }

                if( curGY == endGY )
                {
                    if( curGX == endGX )
                    {
                        retPos->lastFreePosX = posEndX;
                        retPos->lastFreePosY = posEndY;
                        return 1;
                    }
                    else
                    {
                        retPos->firstBlockPosX = curX;
                        retPos->firstBlockPosY = curY;
                        return 0;
                    }
                }

                retPos->lastFreePosX = curX;
                retPos->lastFreePosY = curY;
            }
        }
    }
    else
    {
        while( 1 )
        {

			if( IsGridFree_i( curGX, curGY, unitSize, unitType ) == 0 )
            {
                retPos->firstBlockPosX = posEndX;
                retPos->firstBlockPosY = curY;
                return 0;
            }


            if( curGY == endGY )
            {
                retPos->lastFreePosX = posEndX;
                retPos->lastFreePosY = posEndY;
                return 1;
            }

            if( deltaGY > 0 )
            {
                curGY = curGY + 1;
            }
            else
            {
                curGY = curGY - 1;
            }


            curY = ( int )( ( curGY + 0.5 ) * PER_CELL );

			if( IsGridFree_i( curGX, curGY, unitSize, unitType ) == 0 )
            {
                retPos->firstBlockPosX = posEndX;
                retPos->firstBlockPosY = curY;
                return 0;
            }

            preGY = curGY;

            retPos->lastFreePosX = posEndX;
            retPos->lastFreePosY = curY;
        }
    }
}


int BeautyPath( ST_RET_PATH *pstInOutPath, char unitSize, char unitType )
{
    if( pstInOutPath == NULL )
    {
        return -1;
    }

    if( pstInOutPath->len >= 3 )
    {
        int i = 2; //从第二个点开始美化
        int j = 0;

        for( j = 2; j < pstInOutPath->len; j++ )
        {
            if( IsLineThrough( pstInOutPath->astPoints[j].x, pstInOutPath->astPoints[j].y, pstInOutPath->astPoints[i - 2].x, pstInOutPath->astPoints[i - 2].y, unitSize, unitType, NULL ) == 1 )
            {
                pstInOutPath->astPoints[i - 1].x = pstInOutPath->astPoints[j].x;
                pstInOutPath->astPoints[i - 1].y = pstInOutPath->astPoints[j].y;
            }
            else
            {
                pstInOutPath->astPoints[i].x = pstInOutPath->astPoints[j].x;
                pstInOutPath->astPoints[i].y = pstInOutPath->astPoints[j].y;
                i++;
            }
        }

        pstInOutPath->len = i;
    }

    return 0;
}


int PushNodeIntoListReferG( const ST_PATH_NODE *nodeIn )
{
    int curNodeLen = gAStar.nodeListSize;

    if( curNodeLen >= MAX_NODE_NUM_EVER_IN_OPEN_LIST )
    {
        return -1;
    }

	if( nodeIn->G >= MAX_G_INCAL )
    {
        return -2;
    }

    if( nodeIn->G < gAStar.minG )
    {
        gAStar.minG = nodeIn->G;
    }

    if( nodeIn->G > gAStar.maxG )
    {
        gAStar.maxG = nodeIn->G;
    }

    gAStar.nodeList[curNodeLen].node = *nodeIn;

    if( gAStar.listHeadReferToG[nodeIn->G].seq64num != gAStar.seq64num )
    {
        gAStar.listHeadReferToG[nodeIn->G].seq64num = gAStar.seq64num;
        gAStar.nodeList[curNodeLen].nextIdx = -1;
    }
    else
    {
        gAStar.nodeList[curNodeLen].nextIdx = gAStar.listHeadReferToG[nodeIn->G].headNodeIdxInList;
    }

    gAStar.listHeadReferToG[nodeIn->G].headNodeIdxInList = curNodeLen;

    gAStar.nodeListSize++;

    return 0;
}


int RecycleFreeOpenListNodeIdx( short inShIdx )
{
    if( gAStar.freeOpenListNodeIdxSize >= MAX_OPEN_LIST_NODE )
    {
        return -1;
    }

    gAStar.freeOpenListNodeIdx[gAStar.freeOpenListNodeIdxSize].seq64num = gAStar.seq64num;
    gAStar.freeOpenListNodeIdx[gAStar.freeOpenListNodeIdxSize].shIdx = inShIdx;

    gAStar.freeOpenListNodeIdxSize++;

    return 0;
}

int SetCloseListNodeIdx( int gridID, short shIndex )
{
	if( gridID < 0 || gridID >= MAX_GRID_ID_INCAL )
    {
        return -1;
    }

    gAStar.shOpenCloseListIndex[gridID] = ( shIndex | ( 1 << ( sizeof( short ) * CHAR_BIT - 1 ) ) );

    return 0;
}

int SetOpenListNodeIdx( int gridID, short shIndex )
{
	if( gridID < 0 || gridID >= MAX_GRID_ID_INCAL )
    {
        return -1;
    }

    gAStar.shOpenCloseListIndex[gridID] = shIndex;

    return 0;
}


short GetCloseListNodeIdx( int gridID, const ST_PATH_NODE *inNode )
{
	if( gridID < 0 || gridID >= MAX_GRID_ID_INCAL )
    {
        return -1;
    }

    short shIdx = gAStar.shOpenCloseListIndex[gridID];

    if( -1 == shIdx )
    {
        return -2;
    }

    if( ( ( int )shIdx & ( 1 << ( sizeof( short )* CHAR_BIT - 1 ) ) ) == 0 )
    {
        return -3;
    }

    shIdx = ( shIdx & ( ~( 1 << ( sizeof( short ) * CHAR_BIT - 1 ) ) ) );

    if( shIdx < 0 || shIdx >= MAX_CLOSE_LIST_NODE )
    {
        return -4;
    }

    if( gAStar.closeList[( int )shIdx].seq64num != gAStar.seq64num )
    {
        return -5;
    }

    if( gAStar.closeList[( int )shIdx].node.x != inNode->x ||
        gAStar.closeList[( int )shIdx].node.y != inNode->y )
    {
        return -6;
    }

    return shIdx;
}


short GetOpenListNodeIdx( int gridID, const ST_PATH_NODE *inNode )
{
	if( gridID < 0 || gridID >= MAX_GRID_ID_INCAL )
    {
        return -1;
    }

    short shIdx = gAStar.shOpenCloseListIndex[gridID];

    if( -1 == shIdx )
    {
        return -1;
    }

    if( ( ( int )shIdx & ( 1 << ( sizeof( short )* CHAR_BIT - 1 ) ) ) > 0 )
    {
        return -1;
    }

    if( shIdx < 0 || shIdx >= MAX_OPEN_LIST_NODE )
    {
        return -1;
    }

    if( gAStar.openList[( int )shIdx].seq64num != gAStar.seq64num )
    {
        return -1;
    }

    if( gAStar.openList[( int )shIdx].node.x != inNode->x ||
        gAStar.openList[( int )shIdx].node.y != inNode->y )
    {
        return -1;
    }

    return shIdx;
}



short GetFreeOpenListNodeIdx()
{
    if( gAStar.freeOpenListNodeIdxSize <= 0 )
    {
        return -1;
    }

    --gAStar.freeOpenListNodeIdxSize; //数量先减

    if( gAStar.freeOpenListNodeIdx[gAStar.freeOpenListNodeIdxSize].seq64num != gAStar.seq64num )
    {
        return gAStar.freeOpenListNodeIdxSize;
    }
    else
    {
        return gAStar.freeOpenListNodeIdx[gAStar.freeOpenListNodeIdxSize].shIdx;
    }
}


int AddNodeToCloseList( const ST_PATH_NODE *inNode )
{
    if( gAStar.closeListSize >= MAX_CLOSE_LIST_NODE )
    {
        return -1;
    }

    gAStar.closeList[gAStar.closeListSize].node = *inNode;
    gAStar.closeList[gAStar.closeListSize].seq64num = gAStar.seq64num;

	SetCloseListNodeIdx( inNode->x * GCUR_MAPDATA->iMapWidth + inNode->y, ( short )gAStar.closeListSize );

    ++gAStar.closeListSize;

    return gAStar.closeListSize - 1;
}


int AddNodeToOpenList( const ST_PATH_NODE *inNode )
{
    //首先在hash表中插入
    if( gAStar.openListSize >= MAX_OPEN_LIST_NODE )
    {
        return -1;
    }

    short shIdx = GetFreeOpenListNodeIdx();

    //插入成功则记录在堆中记录位置
    if( shIdx >= 0 )
    {
        ++gAStar.openListSize;
        gAStar.openList[( int )shIdx].node = *inNode;
        gAStar.openList[( int )shIdx].seq64num = gAStar.seq64num;

		SetOpenListNodeIdx( inNode->x * GCUR_MAPDATA->iMapWidth + inNode->y, shIdx );

        if( PushNodeIntoListReferG( inNode ) >= 0 )
        {
            return shIdx;
        }
    }

    return -1;
}



int DelNodeInOpenList( const ST_PATH_NODE *inNode )
{
    if( gAStar.openListSize <= 0 )
    {
        return -1;
    }

	short shIdx = GetOpenListNodeIdx( inNode->x * GCUR_MAPDATA->iMapWidth + inNode->y, inNode );

    if( shIdx >= 0 )
    {
        --gAStar.openListSize;

		SetOpenListNodeIdx( inNode->x * GCUR_MAPDATA->iMapWidth + inNode->y, -1 );

        RecycleFreeOpenListNodeIdx( shIdx );

        return 0;
    }

    return -1;
}


int SquareDist( int posAX, int posAY, int posBX, int posBY )
{
    return ( posAX - posBX ) * ( posAX - posBX ) + ( posAY - posBY ) * ( posAY - posBY );
}


const ST_PATH_NODE *FindNodeWithMinG()
{
    while( gAStar.minG <= gAStar.maxG )
    {
        if( gAStar.listHeadReferToG[gAStar.minG].seq64num == gAStar.seq64num )
        {
            int headNodeIdxInListTmp = gAStar.listHeadReferToG[gAStar.minG].headNodeIdxInList;

            if( headNodeIdxInListTmp >= 0 )
            {
                gAStar.listHeadReferToG[gAStar.minG].headNodeIdxInList = gAStar.nodeList[headNodeIdxInListTmp].nextIdx;

                const ST_PATH_NODE *minGNode = &( gAStar.nodeList[headNodeIdxInListTmp].node );

                if( DelNodeInOpenList( minGNode ) >= 0 ) //在开放列表中删除
                {
                    return minGNode;
                }
            }
            else
            {
                ++gAStar.minG;
            }
        }
        else
        {
            ++gAStar.minG;
        }
    }

    return NULL;
}



ST_PATH_NODE *GetNodeInOpenList( const ST_PATH_NODE *inNode )
{
	short shIdx = GetOpenListNodeIdx( inNode->x * GCUR_MAPDATA->iMapWidth + inNode->y, inNode );

    if( shIdx >= 0 )
    {
        return &( gAStar.openList[( int )shIdx].node );
    }

    return NULL;
}


ST_PATH_NODE *GetNodeInCloseListByIdx( short shIdx )
{
    if( shIdx < 0 || shIdx >= MAX_CLOSE_LIST_NODE )
    {
        return NULL;
    }

    if( gAStar.closeList[( int )shIdx].seq64num != gAStar.seq64num )
    {
        return NULL;
    }

    return &( gAStar.closeList[( int )shIdx].node );
}


ST_PATH_NODE *GetNodeInCloseList( const ST_PATH_NODE *inNode )
{
	short shIdx = GetCloseListNodeIdx( inNode->x * GCUR_MAPDATA->iMapWidth + inNode->y, inNode );

    if( shIdx >= 0 )
    {
        return &( gAStar.closeList[( int )shIdx].node );
    }

    return NULL;
}


int DetectOnePoint_i( unsigned char gridX, unsigned char gridY, const ST_PATH_NODE *fatherNode, short fatherNodeIdx, int endX, int endY )
{
    int ret = 0;

	if( IsValidGrid_i( gridX, gridY ) == 0 )
    {
        return -1;
    }

#ifndef _GSDLL_
#ifdef WIN32
    DrawGridBlock( gridX, gridY, 8 );
#endif
#endif

    ST_PATH_NODE node;
    node.x = gridX;
    node.y = gridY;
    node.G = 0;
    node.father = -1;

    ST_PATH_NODE *nodeInOpen = GetNodeInOpenList( &node );
    const ST_PATH_NODE *nodeInClose = GetNodeInCloseList( &node );

    if( nodeInOpen == NULL && nodeInClose == NULL )
    {
        if( IsGridFree_i( node.x, node.y, gAStar.unitSize, gAStar.unitType ) == 0 )
        {
            ret = AddNodeToCloseList( &node );
        }
        else
        {
            node.father = fatherNodeIdx;
            //node.G = fatherNode->G + 1 - abs( fatherNode->x - endX ) - abs( fatherNode->y - endY ) + abs( gridX - endX ) + abs( gridY - endY );
            node.G = fatherNode->G + 1 - CACL_NODE_G( fatherNode->x, fatherNode->y, endX, endY ) + CACL_NODE_G( gridX, gridY, endX, endY );

            int iTmp = fatherNode->G;
            int iTmp2 = CACL_NODE_G( fatherNode->x, fatherNode->y, endX, endY );

            if( iTmp != iTmp2 )
            {
                ret = iTmp + iTmp2;
            }

            ret = AddNodeToOpenList( &node );
        }
    }
    else if( nodeInOpen != NULL )
    {
        //node.G = fatherNode->G + 1 - abs( fatherNode->x - endX ) - abs( fatherNode->y - endY ) + abs( gridX - endX ) + abs( gridY - endY );
        node.G = fatherNode->G + 1 - CACL_NODE_G( fatherNode->x, fatherNode->y, endX, endY ) + CACL_NODE_G( gridX, gridY, endX, endY );

        if( nodeInOpen->G > node.G )
        {
            nodeInOpen->G = node.G;
            nodeInOpen->father = fatherNodeIdx;
            ret = PushNodeIntoListReferG( nodeInOpen );
        }
    }

    return ret;
}


int WhereToGo( int stopDist, const Vec2 *posEnd, int endX, int endY, const ST_PATH_NODE **ppNodeOut, unsigned char *pIsFound )
{
    int ret = 0;

    const ST_PATH_NODE *fatherNode = FindNodeWithMinG();

    if( fatherNode == NULL )
    {
        //WOOD_PRINT( "FindNodeWithMinG Failed minG:%d\n", gAStar.minG );
        return -2;
    }

    *ppNodeOut = fatherNode;

    Vec2 tmpPos;
    tmpPos.x = fatherNode->x * PER_CELL + 1;
    tmpPos.y = fatherNode->y * PER_CELL + 1;

    if( gAStar.unitSize % 2 == 1 && gAStar.unitSize > 1 )
    {
        tmpPos.x = tmpPos.x + PER_CELL / 2;
        tmpPos.y = tmpPos.y + PER_CELL / 2;
    }

    long long squareDistTmp = SquareDist( tmpPos.x, tmpPos.y, posEnd->x, posEnd->y );

    if( squareDistTmp < gAStar.minDist || gAStar.minDist == -1 )
    {
        gAStar.minDist = squareDistTmp;
        gAStar.isFindNearPath = 1;
        gAStar.nearPathNode = *fatherNode;
    }

    if( squareDistTmp <= stopDist * stopDist )
    {
        if( IsLineThrough( tmpPos.x, tmpPos.y, posEnd->x, posEnd->y, gAStar.unitSize, gAStar.unitType, NULL ) == 1 )
        {
            *pIsFound = 1;
            return 0;
        }
    }

    if( gAStar.closeListSize >= MAX_CLOSE_LIST_NODE )
    {
        return -3;
    }

    if( gAStar.closeListSize >= FAILED_CHECK_MIN_CLOSE_LIST_NODE_NUM )  //预估为失败, 不再尝试
    {
        int expectOpenListNum = gAStar.closeListSize / FAILED_CHECK_BETWEEN_CLOSE_AND_OPEN_LIST_NODE_NUM;

        if( expectOpenListNum >= gAStar.openListSize )
        {
            return -4;
        }
    }

    short fatherNodeIdx = AddNodeToCloseList( fatherNode ); //放入关闭列表中

    if( fatherNodeIdx < 0 )
    {
        //WOOD_PRINT( "AddNodeToCloselist Failed closeListLen:%d\n", gAStar.closeListSize );
        return -5;
    }

    ret = DetectOnePoint_i( fatherNode->x - 1, fatherNode->y, fatherNode, fatherNodeIdx, endX, endY );

    if( ret >= 0 )
    {
        ret = DetectOnePoint_i( fatherNode->x + 1, fatherNode->y, fatherNode, fatherNodeIdx, endX, endY );
    }

    if( ret >= 0 )
    {
        ret = DetectOnePoint_i( fatherNode->x, fatherNode->y + 1, fatherNode, fatherNodeIdx, endX, endY );
    }

    if( ret >= 0 )
    {
        ret = DetectOnePoint_i( fatherNode->x, fatherNode->y - 1, fatherNode, fatherNodeIdx, endX, endY );
    }


    if( 1 )
    {
        if( ret >= 0 )
        {
            ret = DetectOnePoint_i( fatherNode->x - 1, fatherNode->y - 1, fatherNode, fatherNodeIdx, endX, endY );
        }

        if( ret >= 0 )
        {
            ret = DetectOnePoint_i( fatherNode->x + 1, fatherNode->y - 1, fatherNode, fatherNodeIdx, endX, endY );
        }


        if( ret >= 0 )
        {
            ret = DetectOnePoint_i( fatherNode->x + 1, fatherNode->y + 1, fatherNode, fatherNodeIdx, endX, endY );
        }


        if( ret >= 0 )
        {
            ret = DetectOnePoint_i( fatherNode->x - 1, fatherNode->y + 1, fatherNode, fatherNodeIdx, endX, endY );
        }

    }

    return ret;
}


void ReverserNode( ST_RET_PATH *pstRetPath )
{
    if( pstRetPath->len <= 0 )
    {
        return;
    }

    int i = 0;
    int j = pstRetPath->len - 1;
    int m = j >> 1;

    for( ; i <= m; i++ )
    {
        //WOOD_PRINT( "IN ReverserNode Before A x:%d y:%d B x:%d y:%d \n", pstRetPath->astPoints[i].x, pstRetPath->astPoints[i].y, pstRetPath->astPoints[j - i].x, pstRetPath->astPoints[j - i].y );
        Vec2 tmpPos = pstRetPath->astPoints[i];
        pstRetPath->astPoints[i] = pstRetPath->astPoints[j - i];
        pstRetPath->astPoints[j - i] = tmpPos;

        //WOOD_PRINT( "IN ReverserNode After A x:%d y:%d B x:%d y:%d \n", pstRetPath->astPoints[i].x, pstRetPath->astPoints[i].y, pstRetPath->astPoints[j - i].x, pstRetPath->astPoints[j - i].y );
    }
}

int NodeToPath( const Vec2 *posStart, int gridStartX, int gridStartY, const ST_PATH_NODE *inNode, ST_RET_PATH *retPath )
{
    retPath->len = 0;
    const ST_PATH_NODE *node = inNode;

    while( node != NULL )
    {
        if( retPath->len >= MAX_CLOSE_LIST_NODE )
        {
            return -1;
        }

        if( retPath->len >= MAX_RET_PATH_LEN )
        {
            return -2;
        }

        int ix = node->x * PER_CELL + 1;
        int iy = node->y * PER_CELL + 1;

        if( ( gAStar.unitSize % 2 ) == 1 && gAStar.unitSize > 1 )
        {
            ix = ix + PER_CELL / 2;
            iy = iy + PER_CELL / 2;
        }

        //WOOD_PRINT( "IN Debug Node x:%d y:%d Pos x:%d y:%d retPath->len:%d\n", node->x, node->y, ix, iy, retPath->len );

        retPath->astPoints[retPath->len].x = ix;
        retPath->astPoints[retPath->len].y = iy;
        ++retPath->len;

        if( node->father == -1 ) //最后,考虑把初始点归并进路径
        {
            ix = posStart->x;
            iy = posStart->y;

            if( retPath->len >= 1 && node->x == gridStartX && node->y == gridStartY )
            {
                retPath->astPoints[retPath->len - 1].x = ix;
                retPath->astPoints[retPath->len - 1].y = iy;
            }
            else
            {
                retPath->astPoints[retPath->len].x = ix;
                retPath->astPoints[retPath->len].y = iy;
                retPath->len++;
            }

            break;
        }
        else
        {
            node = GetNodeInCloseListByIdx( node->father );
        }
    }

    ReverserNode( retPath );

    return 0;
}


void InitGAStar( char unitSize, char unitType )
{
    gAStar.minDist = -1;
    gAStar.isFindNearPath = 0;

    gAStar.openListSize = 0;
    gAStar.closeListSize = 0;
    gAStar.freeOpenListNodeIdxSize = MAX_OPEN_LIST_NODE;

    gAStar.nodeListSize = 0;

    gAStar.minG = 0;
    gAStar.maxG = 0;
    gAStar.isInStaticBlock = 0;
    gAStar.seq64num = 1;

    gAStar.unitSize = unitSize;
    gAStar.unitType = unitType;

    memset( gAStar.shOpenCloseListIndex, 0xff, sizeof( gAStar.shOpenCloseListIndex ) );
    memset( gAStar.openList, 0, sizeof( gAStar.openList ) );
    memset( gAStar.closeList, 0, sizeof( gAStar.closeList ) );
    memset( gAStar.nodeList, 0, sizeof( gAStar.nodeList ) );
    memset( gAStar.freeOpenListNodeIdx, 0, sizeof( gAStar.freeOpenListNodeIdx ) );
    memset( gAStar.listHeadReferToG, 0, sizeof( gAStar.listHeadReferToG ) );
}


int FindPathByAStar( int iMapIndex, const Vec2 *pPosBegin, const Vec2 *pPosEnd, int stopDist, char unitSize, char unitType, ST_RET_PATH *pstRetPath )
{
    InitGAStar( unitSize, unitType );

	ST_MAPDATA * pstMapData = &(g_stMapData[iMapIndex]);
	SetCurMapDataBlock_i( iMapIndex );

    Vec2 gridStart;
    Vec2 gridEnd;

    PosToGrid( pPosBegin->x, pPosBegin->y, &gridStart );
    PosToGrid( pPosEnd->x, pPosEnd->y, &gridEnd );

    gAStar.seq64num = gAStar.seq64num + 1;

    if( IsGridInStaticBlock_i( gridStart.x, gridStart.y, gAStar.unitSize, gAStar.unitType ) )
    {
        gAStar.isInStaticBlock = 1;
        gAStar.unitSize = 1;
    }

	if( IsValidGrid_i( gridStart.x, gridStart.y ) == 0 )
    {
        return 0;
    }

	if( IsValidGrid_i( gridEnd.x, gridEnd.y ) == 0 )
    {
        return 0;
    }

    ST_PATH_NODE node;
    node.x = gridStart.x;
    node.y = gridStart.y;
    //node.G = abs( gridStart.x - gridEnd.x ) + abs( gridStart.y - gridEnd.y );
    node.G = CACL_NODE_G( gridStart.x, gridStart.y, gridEnd.x, gridEnd.y );
    node.father = -1;

    gAStar.minDist = SquareDist( pPosBegin->x, pPosBegin->y, pPosEnd->x, pPosEnd->y );
    gAStar.isFindNearPath = 0;
    gAStar.minG = node.G;

    int idxTmp = AddNodeToOpenList( &node );

    if( idxTmp < 0 )
    {
        return 0;
    }

    const ST_PATH_NODE *rnode;
    int findRet = 0;
    int totalCalNum = 0;
    unsigned char isFound = 0;

    while( findRet >= 0 )
    {
        findRet = WhereToGo( stopDist, pPosEnd, gridEnd.x, gridEnd.y, &rnode, &isFound );

        if( isFound == 1 )
        {
            break;
        }

        ++totalCalNum;
    }

    //WOOD_PRINT( "TotaoCalNum:%d\n", totalCalNum );

    if( isFound == 1 )
    {
        NodeToPath( pPosBegin, gridStart.x, gridStart.y, rnode, pstRetPath );
    }
    else
    {
        if( gAStar.isFindNearPath )
        {
            NodeToPath( pPosBegin, gridStart.x, gridStart.y, &gAStar.nearPathNode, pstRetPath );
        }
        else
        {
            pstRetPath->astPoints[0].x = pPosBegin->x;
            pstRetPath->astPoints[0].y = pPosBegin->y;
            pstRetPath->len = 1;
        }
    }

	if( pstMapData->iPathBeautyDistSquare > 0 )
    {
        BeautyPath( pstRetPath, gAStar.unitSize, gAStar.unitType );
    }

    if( isFound == 1 )
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


void InitTestMapBlock( int iMapIndex )
{
	SetCurMapDataBlock_i( iMapIndex );

	SetGridBlock( iMapIndex, 4, 4, 2, UNIT_GROUND );

    int i = 0;
    int orgX = 0;

    for( i = 1; i <= 10; i++ )
    {
        orgX = i * 100 + orgX + 300;
		SetGridBlock( iMapIndex, orgX, orgX, i, UNIT_GROUND );
    }

    orgX = 0;

    for( i = 1; i <= 10; i++ )
    {
        orgX = i * 100 + orgX + 300;
		SetGridBlock( iMapIndex, orgX + 800, orgX, i, UNIT_GROUND );
    }

    orgX = 0;

    for( i = 1; i <= 10; i++ )
    {
        orgX = i * 100 + orgX + 300;
		SetGridBlock( iMapIndex, orgX + 1000, orgX, 3, UNIT_GROUND );
    }

    orgX = 0;

    for( i = 1; i <= 10; i++ )
    {
        orgX = i * 1000 + 300;
		SetGridBlock( iMapIndex, orgX + 1000, orgX, 3, UNIT_GROUND );
    }
}


int FindPathAll( int iMapIndex, const Vec2 *pPosBegin, const Vec2 *pPosEnd, int stopDist, char unitSize, char unitType, ST_RET_PATH *pstRetPath )
{
	SetCurMapDataBlock_i( iMapIndex );

    ST_LINE_CHECK_RESULT stCheckRet;
    memset( &stCheckRet, 0, sizeof( stCheckRet ) );

    if( IsLineThrough( pPosBegin->x, pPosBegin->y, pPosEnd->x, pPosEnd->y, unitSize, unitType,  &stCheckRet ) == 1 )
    {
        pstRetPath->len = 2;
        pstRetPath->astPoints[0].x = pPosBegin->x;
        pstRetPath->astPoints[0].y = pPosBegin->y;
        pstRetPath->astPoints[1].x = pPosEnd->x;
        pstRetPath->astPoints[1].y = pPosEnd->y;
        return 1;
    }
    else
    {
        if( stCheckRet.lastFreePosX != 0 && stCheckRet.lastFreePosY != 0 )
        {
            unsigned int checkDist = stopDist * stopDist;
            unsigned int blockDist = ( stCheckRet.firstBlockPosX- pPosEnd->x ) * ( stCheckRet.firstBlockPosX - pPosEnd->x ) +
                                     ( stCheckRet.firstBlockPosY - pPosEnd->y ) * ( stCheckRet.firstBlockPosY - pPosEnd->y );

            if( checkDist >= blockDist )
            {
                pstRetPath->len = 2;
                pstRetPath->astPoints[0].x = pPosBegin->x;
                pstRetPath->astPoints[0].y = pPosBegin->y;
                pstRetPath->astPoints[1].x = stCheckRet.firstBlockPosX;
                pstRetPath->astPoints[1].y = stCheckRet.firstBlockPosY;            
                return 1;
            }            
        }
    }

	return FindPathByAStar( iMapIndex, pPosBegin, pPosEnd, stopDist, unitSize, unitType, pstRetPath );
}



int FindPathAllForCS( int iMapIndex, int posBeginX, int posBeginY, int posEndX, int posEndY,
	int radius, int unitSize, char unitType, ST_RET_PATH *output)
{
	SetCurMapDataBlock_i( iMapIndex );

	Vec2 posBegin;
	posBegin.x = posBeginX;
	posBegin.y = posBeginY;

	Vec2 posEnd;
	posEnd.x = posEndX;
	posEnd.y = posEndY;

	output->len = 0;

	int ret = FindPathAll( iMapIndex, &posBegin, &posEnd, radius, unitSize, unitType, output );
	
	return ret;
}

