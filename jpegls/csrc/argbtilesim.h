#ifndef ARGBTILE_H
#define ARGBTILE_H 
/*************************************************************************
 ************************************************************************/
#include "common.h"
#include "channel.h"
#include "simualte.h"
#include <ctime>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>
#include "addr.h"



using namespace std;
#if TESTMODULE == 5
#define ENCODETEST 1
void compresstest(){
	//cpp compress
#if ENCODETEST
	int  sizex = 4;
	int number = sizex*sizex;
	srand(0*7+348);
	for (int i = 0; i < picxsize*picysize*4 ; i ++) {
		picdata[i] = (unsigned char)(rand() % 256);
	}
	for (int i = 0; i < 20; i ++) {
		printf("%d\t",picdata[i]);
	}
	printf("\n");
	int *size  = new int(sizeof(picdata)/sizeof(unsigned char));
    //int originsize = *size ;
	//cout << *size << endl ;
	int originsize = picxsize*picysize*4 ;
	printf("origin size %d\n", originsize );
	//printf("origin size %d\n", *size  );
	unsigned char  *compressed = new unsigned char[originsize*5];
	//rlecompress(data,size ,compressed);
	//printf("origin size %d\n", *size  );
	unsigned char * filetile = new unsigned char[number*4];
	compressARGB2tile(picdata,compressed,size,picysize,picxsize,sizex,sizex,filetile); 
	compressedC = compressed;
	int compsize = *size;
	printf("compsize %d\n",compsize ); 

	top->io_control_start  = 1;
	top->io_size_height = picysize;
	top->io_size_widthh = picxsize;
	while (!top->io_control_finish && difftestflag)
	{
		clockntimes(1);
	}
	printf("Verilog len:%d compsize:%d\n",top->io_len,compsize);
	//printf("Verilog results\n");
	//for(int i =0 ; i < 20 ; i++){
	//	printf("%x ",piccompress[i]);
	//}
	for(int i =0 ; i < compsize ; i++){
		if(piccompress[i]!=compressed[i]){
			printf("Verilog Module Wrong\n");
			break;
		}
	}
	delete[] compressed;
	printf("Verilog Wave :%d\n",wavecount);
	//for(int i =0 ; i < compsize ; i++){
		//if(i%sizex==0&&(i!=0)) printf("\n");
		//printf("%x ",tilecode[i]);
	//}
	//printf("\n");
#else 
	int count = 0;
	int  sizex = 4;
	int number = sizex*sizex;
	bool flag = 1;
	while (flag && (count <= 10))
	{
		/* code */
		srand(count*7+348);
	//unsigned char data[number];
		for (int i = 0; i < picxsize*picysize*4 ; i ++) {
			picdata[i] = (unsigned char)(rand() % 256);
		}
		int *size  = new int(sizeof(picdata)/sizeof(unsigned char));
		int originsize = picxsize*picysize*4 ;
		printf("origin size %d\n", originsize );
		//printf("origin size %d\n", *size  );
		unsigned char  *compressed = new unsigned char[originsize*5];
		//rlecompress(data,size ,compressed);
		//printf("origin size %d\n", *size  );
		unsigned char * filetile = new unsigned char[number*4];
		compressARGB2tile(picdata,compressed,size,picysize,picxsize,4,4,filetile);
		compressedC = compressed; 
		int compsize = *size;
		printf("compsize %d\n",compsize ); 
		top->io_control_start  = 1;
		top->io_size_height = picysize;
		top->io_size_widthh = picxsize;
		while (!top->io_control_finish)
		{
			clockntimes(1);
		}
		int length = top->io_len;
		top->io_control_start  = 0;
		clockntimes(10);
		for(int i =0 ; i < compsize ; i++){
			if(piccompress[i]!=compressed[i]){
				flag = 0;
				//printf("Verilog Module Wrong\n");
			}
		}
		
		printf("count:%d\n",count);
		printf("Verilog len:%d compsize:%d\n",length,compsize);
		//for (int i = 0; i < number ; i ++) {
		//	if(i%sizex==0&&(i!=0)) printf("\n");
		//	printf("%d\t",data[i]);
		//}
		//printf("\n");
		if(!flag) {
			printf("Verilog Module Wrong at count:%d\n",count);
			//printf("compsize %d\n",compsize ); 
			printf("Verilog len:%d compsize:%d\n",length,compsize);
		}
		count++;
		//printf("\n");

		delete size ;
	}
	if(flag){
		printf("%d random samples check pass\n",count);
	}


#endif
	//
}
void compressfiletest(char const * inFileName){
	//cpp compress

	int  sizex = 4;
	int number = sizex*sizex;
	int width, height;
	unsigned char *pic = getpixdata(inFileName,&width, &height);
	for(int i = 0; i < width*height*4;i++){
		picdata[i] = pic[i];
	}
	for (int i = 0; i < 30; i ++) {
		printf("%d ",picdata[i]);
	}
	picxsize = width;
	picysize = height;
	printf("\n");
	int *size  = new int(sizeof(picdata)/sizeof(unsigned char));
    //int originsize = *size ;
	//cout << *size << endl ;
	int originsize = picxsize*picysize*4 ;
	printf("origin size %d\n", originsize );
	//printf("origin size %d\n", *size  );
	unsigned char  *compressed = new unsigned char[originsize*5];
	//rlecompress(data,size ,compressed);
	//printf("origin size %d\n", *size  );
	unsigned char * filetile = new unsigned char[number*4];
	compressARGB2tile(picdata,compressed,size,picysize,picxsize,4,4,filetile); 
	compressedC = compressed;
	int compsize = *size;
	printf("compsize %d\n",compsize ); 

	top->io_control_start  = 1;
	top->io_size_height = picysize;
	top->io_size_widthh = picxsize;
	while (!top->io_control_finish && difftestflag)
	{
		clockntimes(1);
	}
	printf("Verilog len:%d compsize:%d\n",top->io_len,compsize);
	//printf("Verilog results\n");
	//for(int i =0 ; i < 20 ; i++){
	//	printf("%x ",piccompress[i]);
	//}
	for(int i =0 ; i < compsize ; i++){
		if(piccompress[i]!=compressed[i]){
			printf("Verilog Module Wrong\n");
			break;
		}
	}
	delete[] compressed;
	printf("Verilog Wave :%d\n",wavecount);
	//for(int i =0 ; i < compsize ; i++){
		//if(i%sizex==0&&(i!=0)) printf("\n");
		//printf("%x ",tilecode[i]);
	//}
	//printf("\n");
	//
}
#elif TESTMODULE == 121
#define DECODETEST 1
void decodetest(){
	//cpp compress
#if DECODETEST
	int  sizex = 4;
	int number = sizex*sizex;
	for (int i = 0; i < number ; i ++) {
		if(i%sizex==0&&(i!=0)) printf("\n");
		printf("%d\t",data[i]);
	}
	printf("\n");
	int *size  = new int(sizeof(data)/sizeof(unsigned char));
    //int originsize = *size ;
	//cout << *size << endl ;
	int originsize = *size ;
	printf("origin size %d\n", *size  );
	unsigned char  compressed[10* (*size) ];
	//rlecompress(data,size ,compressed);
	int compsize = jpeglsencode(data, originsize , compressed,4,4 ) ;
	printf("compsize %d\n",compsize ); 
	for(int i =0 ; i < compsize ; i++){
		printf("%x ",compressed[i]);
	}
	printf("\n");
	// Verilog compress
	for (int i = 0; i < compsize ; i++){
		coded[i] = compressed[i];
	}
	unsigned char * decomressed = (unsigned char *) malloc((originsize));   
    //rledecompress(compressed,decomressed,*size);
	jlsdecode(0,4,4, compressed,decomressed);
	top->io_dcontrol_start  = 1;
	while (!top->io_dcontrol_finish)
	{
		clockntimes(1);
	}
	printf("Verilog len:%d compsize:%d\n",top->io_len,compsize);
	printf("Verilog results\n");
	for(int i =0 ; i < originsize ; i++){
		if(i%sizex==0&&(i!=0)) printf("\n");
		printf("%d\t",data1[i]);
	}
	printf("\n");
#else 
	int count = 0;
	int  sizex = 4;
	int number = sizex*sizex;
	bool flag = 1;
	while (flag && (count <= 10000))
	{
		/* code */
		srand(count*7+12258);
	//unsigned char data[number];
		for (int i = 0 ; i  <  number;i++){
			data[i] = (unsigned char)(rand() % 256);
		}
		//printf("\n");
		//for (int i = 0; i < number ; i ++) {
		//		if(i%sizex==0&&(i!=0)) printf("\n");
		//		printf("%d\t",data[i]);
		//	}
		//printf("\n");
		int *size  = new int(sizeof(data)/sizeof(unsigned char));
		int originsize = *size ;
		unsigned char  compressed[10* (*size) ];
		int compsize = jpeglsencode(data, originsize , compressed,4,4 ) ;
		for (int i = 0; i < compsize ; i++){
			coded[i] = compressed[i];
		}
		unsigned char * decomressed = (unsigned char *) malloc((originsize));   
    	//rledecompress(compressed,decomressed,*size);
		jlsdecode(0,4,4, compressed,decomressed);
		top->io_dcontrol_start  = 1;
		while (!top->io_dcontrol_finish)
		{
			clockntimes(1);
		}
		reset(5);
		top->io_dcontrol_start  = 0;
		
		for (int i = 0; i < originsize ; i ++) {
			//if(i%4==0&&(i!=0)) printf("\n");
			//printf("%d\t",decomressed[i]);
			if(data[i] != data1[i]) {
				flag = 0;
			//	printf ("rle wrong !\n");
				//break;
			}
		}
		//printf("count:%d\n",count);
		//for (int i = 0; i < number ; i ++) {
		//	if(i%sizex==0&&(i!=0)) printf("\n");
		//	printf("%d\t",data[i]);
		//}
		//printf("\n");
		if(!flag) {
			printf("count:%d\n",count);
			for (int i = 0; i < number ; i ++) {
				if(i%sizex==0&&(i!=0)) printf("\n");
				printf("%d\t",data[i]);
			}
			printf("\n");
			printf("compsize %d\n",compsize ); 
			for (int i = 0; i < number ; i ++) {
				if(i%sizex==0&&(i!=0)) printf("\n");
				printf("%d\t",data1[i]);
			}
			printf("\n");
		}
		count++;
		//printf("\n");

		delete size ;
	}
	if(flag){
		printf("%d random samples check pass\n",count);
	}


#endif
	//
}
#elif TESTMODULE == 6
#define ENCODETEST 1
void encodetest(char const * inFileName,char const * outFileName){
	//cpp compress
#if ENCODETEST
	int width, height, nrChannels;
    unsigned char *pic = getpixdata(inFileName,&width, &height);
	
	int *size  = new int(sizeof(pic)/sizeof(unsigned char));
	int originsize = *size ;
	printf("width:%d height:%d\n", width,height);
	//compress use C 
	int *compressionsize = new int (0);
	compressedC = compressARGBfile(pic, width, height,compressionsize);
	unsigned char * compressed  = compressedC;
	int compsize = *compressionsize;
	// Verilog compress
	//for (int i = 0; i < compsize ; i++){
	//	tilecode[i] = compressed[i];
	//}
	for(int i = 0; i < width*height*4;i++){
		picdata[i] = pic[i];
	}
	//unsigned char * decomressed = (unsigned char *) malloc((originsize));   
    //rledecompress(compressed,decomressed,*size);
	//jlsdecode(0,4,4, compressed,decomressed);
	top->io_control_start  = 1;
	top->io_size_height = height;
	top->io_size_widthh = width;
	difftestflag = 1;
	//clockntimes(100000);
	while (!top->io_control_finish && difftestflag)
	{
		clockntimes(1);
	}
	printf("Verilog len:%d compsize:%d\n",top->io_len,compsize);
	printf("Verilog results wave:%d\n",wavecount);
	for(int i =0 ; i < compsize ; i++){
		if(piccompress[i]!=compressed[i]){
			printf("Verilog Module Wrong at i:%x V:%x C:%x\n",i,piccompress[i],compressed[i]);
			break;
		}
	}
	writefile(outFileName,piccompress,compsize);


#endif
	//
}

#elif TESTMODULE == 7
#define DECODETEST 1
void decodetest(char const * compressedFileName,char const * outFileName){
	//cpp decompress
	int     width, height, nrChannels;
    unsigned char * compressBuffer =piccompress ;
    getcompressbuffer(compressedFileName,&width, &height,compressBuffer);
	//printf("%x\n",(int )compressBuffer[2]);
	//unsigned char* realcompbuffer = compressBuffer + 24;
	unsigned char tilefile[64];
    unsigned char * Argb = decompressARGBfile(compressBuffer, width, height,4,4,outFileName,tilefile);
    picdataC = Argb;
	top->io_control_start  = 1;
	top->io_size_height = height;
	top->io_size_widthh = width;
	//printf("hhhhh\n");
	difftestflag = 1;
	//clockntimes(100000);
	while (!top->io_control_finish && difftestflag)
	{
		clockntimes(1);
	}
	//printf("Verilog len:%d compsize:%d\n",top->io_len);
	printf("Verilog results wave:%d\n",wavecount);
	bool flag = 1;
	for(int i =0 ; i < width*height*4 ; i++){
		if(Argb[i]!=picdata[i]){
			flag = 0;
			printf("Verilog Module Wrong at i:%d C:%x V:%x\n",i,Argb[i],picdata[i]);
			break;
		}
	}
	if(flag){
		stbi_write_bmp(outFileName, width, height, STBI_rgb_alpha, reinterpret_cast<char const *>(picdata));
	}
	//
}
#elif TESTMODULE == 8
void fpgaSimulate(char const * inFileName,char const * outFileName){
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
	/*************压缩解压仿真****************/
	int *size  = new int(sizeof(pic)/sizeof(unsigned char));
	int originsize = *size ;
	printf("width:%d height:%d\n", width,height);
	//compress use C 
	int *compressionsize = new int (0);
	compressedC = compressARGBfile(pic, width, height,compressionsize);
	unsigned char * compressed  = compressedC;
	int compsize = *compressionsize;
	// Verilog compress
	//for (int i = 0; i < compsize ; i++){
	//	tilecode[i] = compressed[i];
	//}
	for(int i = 0; i < width*height*4;i++){
		picdata[i] = pic[i];
	}
	clockntimes(1);
	//unsigned char * decomressed = (unsigned char *) malloc((originsize));   
    //rledecompress(compressed,decomressed,*size);
	//jlsdecode(0,4,4, compressed,decomressed);
	top->io_control_start  = 1;
	top->io_size_height = height;
	top->io_size_widthh = width;
	difftestflag = 1;
	//clockntimes(100000);
	while (!top->io_control_finish && difftestflag)
	{
		clockntimes(1);
	}
	bool flag = top->io_success;
	printf("Verilog results wave:%lld\n",wavecount);
	if(flag){
		const char extension[] = "Verilog.bmp";
    
    	// 计算新字符串的长度
    	size_t newLength = std::strlen(outFileName) + std::strlen(extension);

    	// 创建一个新的字符数组来存储新字符串
    	char newFilename[newLength + 1];  // 加 1 用于存储字符串结束符 '\0'

    	// 复制原始文件名到新数组
    	std::strcpy(newFilename, outFileName);

    	// 连接文件名和后缀
   	 	std::strcat(newFilename, extension);

		stbi_write_bmp(newFilename, width, height, STBI_rgb_alpha, reinterpret_cast<char const *>(picdedata));
	}
}

#endif 

#endif
