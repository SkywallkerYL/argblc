/* rgbTileProc.cpp
*  implementation of TILE COMPRESSION
*/
#include <stdio.h>
#include <math.h>
#include <memory.h>
#include <assert.h>
#include "rgbTileProc.h"
#include "rle.h"
#include "lz77.h"
int halforiginsize = 1024;
static int g_nTileWidth = 0;
static int g_nTileHeight = 0;

void tileSetSize(int nTileWidth, int nTileHeight)
{
	g_nTileWidth = nTileWidth;
	g_nTileHeight = nTileHeight;
}

/* compress ARGB data to tile
*  param:
*    pClrBlk      -- IN, pixel's ARGB data
*    pTile        -- OUT, tile data
*    pTileSize    -- OUT, tile's bytes
*  return:
*    0  -- succeed
*   -1  -- failed
*/
int argb2tile(const unsigned char* pClrBlk, unsigned char* pTile, int* pTileSize)
{
	assert(g_nTileWidth > 0 && g_nTileHeight > 0);
	*pTileSize = g_nTileWidth * g_nTileHeight * 4;
	
	//memcpy(pTile, pClrBlk, *pTileSize);
	//unsigned char rlepClr[(*pTileSize*8)];
	rlecompress(pClrBlk,pTileSize,pTile);
	//lz77Compress(pClrBlk,pTileSize,pTile);
	//rlecompress(pClrBlk,pTileSize,rlepClr);
	//lz77Compress(rlepClr,pTileSize,pTile);
	//halfmanCompress(rlepClr,pTileSize,pTile);
	return 0;
}

/* decompress tile data to ARGB
*  param:
*    pTile        -- IN, tile data
*    pTileSize    -- IN, tile's bytes
*    pClrBlk      -- OUT, pixel's ARGB data
*  return:
*    0  -- succeed
*   -1  -- failed
*/
int tile2argb(const unsigned char* pTile, int nTileSize, unsigned char* pClrBlk)
{
	rledecompress(pTile ,pClrBlk, nTileSize);
	//unsigned char lz77de[1024];
	//int size = lz77Decompress(pTile,nTileSize,lz77de);
	//rledecompress(lz77de,pClrBlk,size);
	//memcpy(pClrBlk, pTile, nTileSize);
	//unsigned char halfmandecompress[4096]; 
	
	//int halfdecodesize = halfmanDecompress(pTile,halfmandecompress,nTileSize,root);

	//rledecompress(halfmandecompress,pClrBlk, halfdecodesize);
	
	return 0;
}
