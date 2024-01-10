#ifndef ADDRH
#define ADDRH 
#include "common.h"
#include "testfunc.h"


//unsigned char data[16];
//unsigned char coded[160];


unsigned char data[] = {
		(unsigned char)103	,(unsigned char)81    ,(unsigned char)41    ,(unsigned char)242     ,
		(unsigned char)124	,(unsigned char)27    ,(unsigned char)118   ,(unsigned char)51      ,
		(unsigned char)102	,(unsigned char)49    ,(unsigned char)37    ,(unsigned char)88     ,
		(unsigned char)171	,(unsigned char)155   ,(unsigned char)14    ,(unsigned char)33 
	};
unsigned char data1[16];
unsigned char coded[160];
unsigned char tiledata[16*4];
unsigned char tilecode[640];
unsigned char tiledata1[16*4];


unsigned char picdata[320*320*4];
unsigned char piccompress[320*320*4*5];

extern "C" void pmem_write(long long waddr, long long wdata,char wmask){
    //printf("addr:0x%016x data:0x%016x mask:0b%02b \n",waddr,wdata,wmask);
    //char c = wdata&0xff;
    //uint64_t write_data = wdata;
    if(waddr >= 0x80000000){
        coded[waddr-0x80000000] = (unsigned char)(wdata & 0xffull);
    }else if (waddr < 0x10000000) {
        data1[waddr] =  (unsigned char)(wdata & 0xffull);
    }else if (waddr >= 0x70000000 && waddr < 0x80000000){
        tilecode[waddr-0x70000000] = (unsigned char)(wdata & 0xffull);
    }else if (waddr >= 0x60000000 && waddr < 0x70000000){
        tiledata1[waddr-0x60000000] = (unsigned char)(wdata & 0xffull);
    }else if (waddr >= 0x40000000 && waddr < 0x50000000){
        //if(waddr == 0x40000004) printf("addr:0x%016x data:0x%016x mask:0b%02b \n",waddr,wdata,wmask);
        piccompress[waddr-0x40000000] = (unsigned char)(wdata & 0xffull);
    }
    //for (char i = 0; i < 8; i++)
    //{
        //判断mask的i位是否为1,从地到高。
        //if (wmask & (0x1<<i))
        //{
            ////默认对齐低位
            //coded[]
           //*(uint8_t *)((uint8_t *)vmem + waddr - FB_ADDR+i)= write_data&0xffull;
       // }
    //}
    
}
extern "C" void pmem_read(long long raddr, long long *rdata){
    //printf("addr:0x%016x data:0x%016x\n",raddr,*rdata);
    if(raddr >= 0x80000000){
        *rdata = (long long) (unsigned char)coded[raddr-0x80000000];
    }else if (raddr < 0x10000000) {
        *rdata = (long long) (unsigned char)data[raddr];
    }else if (raddr >= 0x60000000 && raddr < 0x70000000){
        *rdata = (long long) (unsigned char)tiledata[raddr-0x60000000];
    }else if (raddr >= 0x70000000 && raddr < 0x80000000){
        *rdata = (long long) (unsigned char)tilecode[raddr-0x70000000];
    }else if (raddr >= 0x50000000 && raddr < 0x60000000){
        *rdata = (long long) (unsigned char)picdata[raddr-0x50000000];
    }
    
    //printf("addr:0x%016x data:%d read:%d\n",raddr,*rdata,data[raddr]);
}

/*
* compress ARGB data to TILE
*/
int compressARGB2tile(unsigned char * origindata,unsigned char * compressionbuffer,int * compresssize,int height = 320, int width = 320 ,int TILE_HEIGHT = 4,int TILE_WIDTH = 4) {
    int numRows = height / TILE_HEIGHT;
    int numColumns = width / TILE_WIDTH;
    const int BYTES_PER_PIXEL = 4;
    int rowStride = width * BYTES_PER_PIXEL; // 4 bytes per pixel
    //printf("hhhh\n");
    unsigned char pARGB[TILE_WIDTH * TILE_HEIGHT * BYTES_PER_PIXEL] = { 0u };
    //printf("hhhh\n");
    unsigned char * pCompressionBuffer = compressionbuffer;
    //new unsigned char [numRows * numColumns * TILE_WIDTH * TILE_HEIGHT * BYTES_PER_PIXEL];
    //if (NULL == pCompressionBuffer) {
    //    return 1;
    //}

    int tileRowIndex = 0;
    int tileColumnIndex = 0;
    int totalBitsAfterCompression = 0;
    int posInCompressionBuffer = 0;
    unsigned char* pClr = NULL;
    //printf("hhhh\n");
    for (tileRowIndex = 0; tileRowIndex < numRows; tileRowIndex++) {
        for (tileColumnIndex = 0; tileColumnIndex < numColumns; tileColumnIndex++) {
            int tileIndex = tileRowIndex * numColumns + tileColumnIndex;
            pClr = pARGB;
            for (int i = 0; i < TILE_HEIGHT; i++) {
                for (int j = 0; j < TILE_WIDTH; j++) {
                    int row = tileRowIndex * TILE_HEIGHT + i;
                    int col = tileColumnIndex * TILE_WIDTH + j;
                    int pixelDataOffset = rowStride * row + col * BYTES_PER_PIXEL;
                    pClr[0] = origindata[pixelDataOffset];     // b
                    //if(tileRowIndex ==0 && tileColumnIndex == 0) printf("%d ", origindata[pixelDataOffset]);
                    pClr[1] = origindata[pixelDataOffset + 1]; // g
                    pClr[2] = origindata[pixelDataOffset + 2]; // r
                    pClr[3] = origindata[pixelDataOffset + 3]; // a
                    pClr += 4;
                }
            }
            //printf("hhhh\n");
            int *size =new int(TILE_HEIGHT*TILE_WIDTH*4);

            // compress
            jpeglscompress (pARGB, size, pCompressionBuffer+ posInCompressionBuffer ,0,TILE_HEIGHT,TILE_WIDTH);
	
            //argb2tile(pARGB, pCompressionBuffer + posInCompressionBuffer, &pTCInfos[tileIndex].tileSize);
            posInCompressionBuffer += *size;
        }
    }
    *compresssize = posInCompressionBuffer;

    return 0;
}

#endif