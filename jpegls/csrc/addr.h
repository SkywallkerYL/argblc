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


unsigned char picdata[3000*3000*4];
unsigned char piccompress[3000*3000*4*5];
unsigned char picdedata[3000*3000*4];
unsigned char * compressedC;
unsigned char * picdataC;
bool difftestflag = 1;
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
        //if(waddr == 0x40000004) 
        //printf("wave:%d addr:0x%016x data:0x%02x data:0x%02x \n",wavecount,waddr,piccompress[waddr-0x40000000],compressedC[waddr-0x40000000]);
        //piccompress[waddr-0x40000000] = (unsigned char)(wdata & 0xffull);
                
        //printf("addr:0x%016x data:0x%016x mask:0b%02b \n",waddr,wdata,wmask);
        //这个mask的功能不完善,当没有进行包含写文件信息的压缩仿真的时候,不要用mask,因为那个时候只会写一个byte
        //但是chisel生成的Verilog出来的有问题,导致mask一直为0
        for (int i = 0; i < 8; i++)
        {
            //判断mask的i位是否为1,从地到高。
            if (wmask & (0x1<<i))
            {
                piccompress[waddr+i-0x40000000] = (unsigned char)(wdata & 0xffull);
                wdata = wdata >> 8;
                //(uint8_t *)((uint8_t *)vmem + waddr - FB_ADDR)= write_data&0xffull;
            }
        }
        //if(piccompress[waddr-0x40000000] != compressedC[waddr-0x40000000]){
        //    difftestflag = 0;
        //        // if(!(TRACE_CONDITION(wavecount,WAVE_BEGIN,WAVE_END))) difftestflag = 0;
        //    //printf("wave:%d addr:0x%016x data:0x%02x data:0x%02x \n",wavecount,waddr,piccompress[waddr+i-1-0x40000000],compressedC[waddr+i-1-0x40000000]);
        //    printf("addr:0x%016x data:0x%016x datain:0x%016x mask:0b%08b \n",waddr,wdata,piccompress[waddr-0x40000000],wmask);
        //    printf("wave:%d addr:0x%016x data:0x%02x data:0x%02x \n",wavecount,waddr,piccompress[waddr-0x40000000],compressedC[waddr-0x40000000]);
        //}
        //if(waddr == (0x400f95a1-24-8*1920*1056/4/4)){
        //    printf("addr:0x%016x data:0x%016x datain:0x%016x mask:0b%08b \n",waddr,wdata,piccompress[waddr-0x40000000],wmask);
        //    printf("wave:%d addr:0x%016x data:0x%02x data:0x%02x \n",wavecount,waddr,piccompress[waddr-0x40000000],compressedC[waddr-0x40000000]);
        //        
        //}
        //piccompress[waddr-0x40000000] = (unsigned char)(wdata & 0xffull);
    }else if (waddr >= 0x50000000 && waddr < 0x60000000){
        picdata[waddr-0x50000000] = (unsigned char)(wdata & 0xffull);
        //printf("wave:%d addr:0x%016x data:0x%02x  \n",wavecount,waddr,picdata[waddr-0x50000000]);
        //if(picdata[waddr-0x50000000] != picdataC[waddr-0x50000000]){
        //    difftestflag = 0;
        //    printf("wave:%d addr:0x%016x data:0x%02x data:0x%02x \n",wavecount,waddr,picdata[waddr-0x50000000],picdataC[waddr-0x50000000]);
       //
        //}
        //if(waddr == 0x50000090){
        //     printf("wave:%d addr:0x%016x data:0x%02x\n",wavecount,waddr,picdata[waddr-0x50000000]);
        //}
    }else if (waddr >= 0x30000000 && waddr < 0x40000000){
        picdedata[waddr-0x30000000] = (unsigned char)(wdata & 0xffull);
        //printf("wave:%d addr:0x%016x data:0x%02x  \n",wavecount,waddr,picdata[waddr-0x50000000]);
        //if(picdata[waddr-0x50000000] != picdataC[waddr-0x50000000]){
        //    difftestflag = 0;
        //    printf("wave:%d addr:0x%016x data:0x%02x data:0x%02x \n",wavecount,waddr,picdata[waddr-0x50000000],picdataC[waddr-0x50000000]);
       //
        //}
        //if(waddr == 0x50000090){
        //     printf("wave:%d addr:0x%016x data:0x%02x\n",wavecount,waddr,picdata[waddr-0x50000000]);
        //}
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
        //printf("addr:0x%016x data:0x%016x\n",raddr,*rdata);
    }else if (raddr >= 0x40000000 && raddr < 0x50000000){
        //注意一下读这个pic额,要读4个Byte.... 这里出错了  通过原来的方式写进取只剩一个Byte了
        
        *rdata = *(uint64_t *)(&piccompress[raddr-0x40000000]);
        //long long dataread = 0;
        //memcpy(&dataread, piccompress[raddr-0x40000000], 8);
        ////(long long) (unsigned char)piccompress[raddr-0x40000000];
        //*rdata = dataread;
        //printf("addr:0x%016x data:0x%016x\n",raddr,*rdata);
    }else if (raddr >= 0x30000000 && raddr < 0x40000000){
        //注意一下读这个pic额,要读4个Byte.... 这里出错了  通过原来的方式写进取只剩一个Byte了
        
        *rdata = *(uint64_t *)(&picdedata[raddr-0x30000000]);
        //long long dataread = 0;
        //memcpy(&dataread, piccompress[raddr-0x40000000], 8);
        ////(long long) (unsigned char)piccompress[raddr-0x40000000];
        //*rdata = dataread;
        //printf("addr:0x%016x data:0x%016x\n",raddr,*rdata);
    }
    //printf("addr:0x%016x data:%d read:%d\n",raddr,*rdata,data[raddr]);
}

/*
* compress ARGB data to TILE
*/
int compressARGB2tile(unsigned char * origindata,unsigned char * compressionbuffer,int * compresssize,int height , int width ,int TILE_HEIGHT ,int TILE_WIDTH ,unsigned char * filetile) {
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
            //if( posInCompressionBuffer<0x1d89) printf("tile:%d\n",tileIndex);
           
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
            if(tileIndex == -1){
                printf("row:%d col:%d\n",tileRowIndex,tileColumnIndex);
                pClr = pARGB;
                for (int i = 0; i < TILE_WIDTH * TILE_HEIGHT * BYTES_PER_PIXEL ; i ++) {
		            //tiledata[i] = filetile[i];//(unsigned char)(rand() % 256);
		            //if(i%sizex==0&&(i!=0)) printf("\n");
                    filetile[i] =pClr[i];
		            //printf("%d ",pClr[i]);
	            }
                //printf("\n");
                return 0;
            }
            //printf("hhhh\n");
            int *size =new int(TILE_HEIGHT*TILE_WIDTH*4);

            // compress
            jpeglscompress (pARGB, size, pCompressionBuffer+ posInCompressionBuffer ,0,TILE_HEIGHT,TILE_WIDTH);
	
            //argb2tile(pARGB, pCompressionBuffer + posInCompressionBuffer, &pTCInfos[tileIndex].tileSize);
            posInCompressionBuffer += *size;
        }
    }
    std::cout << "compression ratio = " << (float)posInCompressionBuffer / (float)(width * height * BYTES_PER_PIXEL) * 100 << "%" << std::endl;
    
    *compresssize = posInCompressionBuffer;
    return 0;
}

unsigned char * getpixdata(char const * inFileName,int * width, int * height){
    int     widthin, heightin, nrChannels;
    unsigned char *Filedata = stbi_load(inFileName, &widthin, &heightin, &nrChannels, STBI_rgb_alpha);
    if (Filedata == NULL) {
        std::cout << "cannot open file: " << inFileName << std::endl;
        return Filedata;
    }
    
    *width = widthin ;
    *height = heightin ; 
    return Filedata;
}
void getcompressbuffer(char const * compressedFileName,int * width, int * height,unsigned char * compressBuffer){
    std::ifstream ifs;
    ifs.open(compressedFileName, std::ios::binary | std::ios::in);

    if (!ifs.is_open()) {
        std::cout << "fail to open output file: " << compressedFileName << std::endl;
        return ;
    }
    char readBuffer[1024];
    // read "JLCD"
    ifs.read(readBuffer, 24);
    memcpy(compressBuffer,readBuffer,24);
    if (strncmp(readBuffer, "JLCD", 4) != 0) {
        ifs.close();
        std::cout << "ERROR: INVALID tile file: " << compressedFileName << std::endl;
        return ;
    }

    const int BYTES_PER_PIXEL = 4;
    int imgWidth, imgHeight, tileWidth, tileHeight, tileCount;
    // read image width
    ifs.seekg(4);
    ifs.read(reinterpret_cast<char*>(&imgWidth), 4);
    ifs.read(reinterpret_cast<char*>(&imgHeight), 4);
    ifs.read(reinterpret_cast<char*>(&tileWidth), 4);
    ifs.read(reinterpret_cast<char*>(&tileHeight), 4);
    ifs.read(reinterpret_cast<char*>(&tileCount), 4);
    *width = imgWidth;
    *height = imgHeight;
    std::cout << "imgWidth = " << imgWidth 
        << ", imgHeight = " << imgHeight
        << ", tileWidth = " << tileWidth
        << ", tileHeight = " << tileHeight
        << std::endl;
    int tileRowCount = imgHeight / tileHeight;
    int tileColumnCount = imgWidth / tileWidth;
    int tileDataStartPos = 24 + 8 * tileCount;
    unsigned char* Compressin = compressBuffer + tileDataStartPos;
    for (int row = 0; row < tileRowCount; row++) {
        for (int col = 0; col < tileColumnCount; col++) {
            int tileIndex = row * tileColumnCount + col;
            int tileInfoOffset = 24 + 8 * tileIndex;
            unsigned char* CompressHeader = compressBuffer + tileInfoOffset;
            int tileDataOffset = 0;
            int tileDataBytes = 0;
            ifs.seekg(tileInfoOffset);
            ifs.read(reinterpret_cast<char *>(&tileDataOffset), 4);
            ifs.read(reinterpret_cast<char *>(&tileDataBytes), 4);
			ifs.seekg(tileDataStartPos + tileDataOffset);
            ifs.read(readBuffer, tileDataBytes);
            memcpy(Compressin,readBuffer,tileDataBytes);
            Compressin += tileDataBytes;
            ifs.seekg(tileInfoOffset);
            ifs.read(readBuffer, 8);
            memcpy(CompressHeader,readBuffer,8);
        }
    }
    ifs.close();
    return ;
}
void writefile(char const * outFileName,unsigned char * compressionbuffer, int filesize){
    std::ofstream ofs;
    ofs.open(outFileName, std::ios::binary | std::ios::out);
    if (ofs.is_open()) {
        // all tile data
        ofs.write(reinterpret_cast<const char *>(compressionbuffer), filesize);
        ofs.close();
    } else {
        std::cout << "fail to open output file(" << outFileName << ")" << std::endl;
    }
    return ;
}

/*
* compress ARGB data to TILE
*/
unsigned char *  compressARGBfile(unsigned char *data, int width, int  height,int * compressionsize) {
    //int     width, height, nrChannels;
    //unsigned char *data = getpixdata(inFileName,&width, &height);
    std::cout << "image info: width = " << width 
        << ", height = " << height
        << ", channels = " << 4
        << std::endl;

    const int TILE_WIDTH = 4;
    const int TILE_HEIGHT = 4;
    int numRows = height / TILE_HEIGHT;
    int numColumns = width / TILE_WIDTH;
    const int BYTES_PER_PIXEL = 4;
    int rowStride = width * BYTES_PER_PIXEL; // 4 bytes per pixel
    unsigned char pARGB[TILE_WIDTH * TILE_HEIGHT * BYTES_PER_PIXEL] = { 0u };
    unsigned char pCompressed[TILE_WIDTH * TILE_HEIGHT * BYTES_PER_PIXEL] = { 0u };
    unsigned char * compressionbuffer = new unsigned char [numRows * numColumns * TILE_WIDTH * TILE_HEIGHT * BYTES_PER_PIXEL * 6] ; 
    //tileSetSize(TILE_WIDTH, TILE_HEIGHT);
    int Headeroffset = 24 + 8*numRows*numColumns;
    int tileCount = numRows * numColumns;
    unsigned char * pCompressionBuffer = compressionbuffer + Headeroffset  ;
    unsigned char * pHeaderBuffer = compressionbuffer;
    //new unsigned char [numRows * numColumns * TILE_WIDTH * TILE_HEIGHT * BYTES_PER_PIXEL];
    //write head
    pHeaderBuffer[0] = 'J';
    //printf("%x\n",pHeaderBuffer[0]);
    pHeaderBuffer[1] = 'L';
    pHeaderBuffer[2] = 'C';
    pHeaderBuffer[3] = 'D';
    pHeaderBuffer += 4;
    memcpy(pHeaderBuffer, &width, 4);
    pHeaderBuffer += 4;
    memcpy(pHeaderBuffer, &height, 4);
    pHeaderBuffer += 4;
    memcpy(pHeaderBuffer, &TILE_WIDTH, 4);
    pHeaderBuffer += 4;
    memcpy(pHeaderBuffer, &TILE_HEIGHT, 4);
    pHeaderBuffer += 4;
    //printf("%x\n",tileCount);
    memcpy(pHeaderBuffer, &tileCount, 4);
    pHeaderBuffer += 4;
    int tileRowIndex = 0;
    int tileColumnIndex = 0;
    int totalBitsAfterCompression = 0;
    int posInCompressionBuffer = 0;
    unsigned char* pClr = NULL;

    for (tileRowIndex = 0; tileRowIndex < numRows; tileRowIndex++) {
        for (tileColumnIndex = 0; tileColumnIndex < numColumns; tileColumnIndex++) {
            int tileIndex = tileRowIndex * numColumns + tileColumnIndex;
            pClr = pARGB;
            for (int i = 0; i < TILE_HEIGHT; i++) {
                for (int j = 0; j < TILE_WIDTH; j++) {
                    int row = tileRowIndex * TILE_HEIGHT + i;
                    int col = tileColumnIndex * TILE_WIDTH + j;
                    int pixelDataOffset = rowStride * row + col * BYTES_PER_PIXEL;
                    pClr[0] = data[pixelDataOffset];     // b
                    pClr[1] = data[pixelDataOffset + 1]; // g
                    pClr[2] = data[pixelDataOffset + 2]; // r
                    pClr[3] = data[pixelDataOffset + 3]; // a
                    pClr += 4;
                }
            }
            //pTCInfos[tileIndex].tilePosition = posInCompressionBuffer;

            // compress
            int *size =new int(TILE_HEIGHT*TILE_WIDTH*4);

            jpeglscompress (pARGB, size, pCompressionBuffer+ posInCompressionBuffer ,0,TILE_HEIGHT,TILE_WIDTH);
            //
            int tilesize = *size ;
            memcpy(pHeaderBuffer, &posInCompressionBuffer, 4);
            pHeaderBuffer+=4;
            memcpy(pHeaderBuffer, &tilesize, 4);
            pHeaderBuffer+=4;

            //argb2tile(pARGB, pCompressionBuffer + posInCompressionBuffer, &pTCInfos[tileIndex].tileSize);
            posInCompressionBuffer += *size;
        }
    }
    std::cout << "compression ratio = " << (float)posInCompressionBuffer / (float)(width * height * BYTES_PER_PIXEL) * 100 << "%" << std::endl;
    * compressionsize = posInCompressionBuffer+Headeroffset;
    // save compressed data to JLCD file
    //writefile(outFileName,compressionbuffer,posInCompressionBuffer+Headeroffset);
    //stbi_image_free(data);
    return compressionbuffer;
}

void compressinC(char const * inFileName,char const * outFileName){
    int     width, height, nrChannels;
    unsigned char *pic = getpixdata(inFileName,&width, &height);
    int * compressionsize = new int(0) ;
    unsigned char * compressed = compressARGBfile(pic, width, height,compressionsize);
    writefile(outFileName,compressed,* compressionsize);
}
void writeCoeFile(char const * inFileName,char const * outFileName){
    int     width, height, nrChannels;
    unsigned char *pic = getpixdata(inFileName,&width, &height);
    int numRows = height;
    int numCols = width;

    // 构造 COE 文件名，包含行数和列数
    std::string coeFileName = "output_" + std::to_string(numRows) + "x" + std::to_string(numCols) + ".coe";

    // 打开 COE 文件以写入数据
    std::ofstream coeFile(coeFileName);

    if (coeFile.is_open()) {
        // 写入 COE 文件头部
        coeFile << "Memory_Initialization_Radix=16;\r\n";
        coeFile << "Memory_Initialization_Vector=\r\n";

        // 写入数据
        for (size_t i = 0; i < numRows * numCols * 4; ++i) {
            coeFile << std::hex << static_cast<int>(pic[i]);

            // 添加逗号（除了最后一个元素）
            if (i < numRows * numCols * 4 - 1) {
                coeFile << ",";
            }else{
                coeFile << ";";
            }

            // 每行一个数据元素，换行
            coeFile << "\r\n";
        }

        // 关闭 COE 文件
        coeFile.close();

        std::cout << "COE file created successfully: " << coeFileName << std::endl;
    } else {
        std::cerr << "Unable to open COE file: " << coeFileName << std::endl;
    }

    return ;
}


/*
* decompress tile data to ARGB data

*/
unsigned char *  decompressARGBfile(unsigned char *compressbuffer, int width, int  height,int tileWidth,int tileHeight,char const * outFileName,unsigned char * filetile) {
    //int     width, height, nrChannels;
    //unsigned char *data = getpixdata(inFileName,&width, &height);
    int imgWidth = width;
    int imgHeight = height;
    int tileRowCount = height / tileHeight;
    int tileColumnCount = width / tileWidth;
    int tileCount = tileRowCount* tileColumnCount;
    int tileDataStartPos = 24 + 8 * tileCount;
    unsigned char *pDecompressedARGB = new unsigned char [imgWidth * imgHeight * 4];
    unsigned char* CompressIn = compressbuffer;
    unsigned char pTempDecompressionBuffer[1024];
    char readBuffer[1024];
    for (int row = 0; row < tileRowCount; row++) {
        for (int col = 0; col < tileColumnCount; col++) {
            int tileIndex = row * tileColumnCount + col;
            int tileInfoOffset = 24 + 8 * tileIndex;
            int tileDataOffset = 0;
            int tileDataBytes = 0;
            unsigned char* CompressInFo = CompressIn + tileInfoOffset;
            memcpy(reinterpret_cast<char *>(&tileDataOffset),CompressInFo,4);
            CompressInFo+=4;
            memcpy(reinterpret_cast<char *>(&tileDataBytes),CompressInFo,4);
            unsigned char* CompressPic = CompressIn+tileDataStartPos + tileDataOffset;
            memcpy(readBuffer,CompressPic,tileDataBytes);
            // decompress
            //printf("originsize : %d\n", tileDataBytes);
            //tile2argb((unsigned char*)readBuffer, tileDataBytes, pTempDecompressionBuffer);
            jpeglsdecompress((unsigned char*)readBuffer,pTempDecompressionBuffer,tileDataBytes);
            for (int i = 0; i < tileHeight; i++) {
                for (int j = 0; j < tileWidth; j++) {
                    int globalRow = row * tileHeight + i;
                    int globalCol = col * tileWidth + j;
                    int indexInTile = i * tileWidth + j;
                    memcpy(&pDecompressedARGB[(globalRow * imgWidth + globalCol) * 4],
                            &pTempDecompressionBuffer[indexInTile * 4],
                            4);
                }
            }
            //((0x90)/(tileHeight*tileWidth*4))
            if(row ==-1 && col==-1){
                
                printf("tileinf:%x tiledataoffset:%x first byte:%x\n ",tileInfoOffset,tileDataOffset,readBuffer[0]);
                printf("row:%d col:%d\n",row,col);
                //pClr = pARGB;
                for (int i = 0; i < tileWidth * tileHeight * 4 ; i ++) {
		            //tiledata[i] = filetile[i];//(unsigned char)(rand() % 256);
		            //if(i%sizex==0&&(i!=0)) printf("\n");
                    filetile[i] =pTempDecompressionBuffer[i];
		            //printf("%d ",pClr[i]);
	            }
                //printf("\n");
                //return pDecompressedARGB;
            }
        }
    }
    //stbi_write_bmp(outFileName, width, height, STBI_rgb_alpha, reinterpret_cast<char const *>(pDecompressedARGB));

    return pDecompressedARGB;
}

void decompressinC(char const * compressedFileName,char const * outFileName){
    int     width, height, nrChannels;
    unsigned char * compressBuffer = new unsigned char [3000 * 3000 * 4 *5];
    
    getcompressbuffer(compressedFileName,&width, &height,compressBuffer);
    unsigned char* realcompbuffer = compressBuffer + 24;
    unsigned char* filetile = new unsigned char [64];
    unsigned char * Argb = decompressARGBfile(compressBuffer, width, height,4,4,outFileName,filetile);
    delete[] compressBuffer;
    stbi_write_bmp(outFileName, width, height, STBI_rgb_alpha, reinterpret_cast<char const *>(Argb));

}


#endif