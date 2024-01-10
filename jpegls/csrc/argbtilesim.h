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
#define ENCODETEST 0
void compresstest(){
	//cpp compress
#if ENCODETEST
	int  sizex = 4;
	int number = sizex*sizex;
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
	int originsize = *size ;
	printf("origin size %d\n", *size  );
	//printf("origin size %d\n", *size  );
	unsigned char  compressed[5* (*size) ];
	//rlecompress(data,size ,compressed);
	//printf("origin size %d\n", *size  );
	compressARGB2tile(picdata,compressed,size); 
	int compsize = *size;
	printf("compsize %d\n",compsize ); 
	//for(int i =0 ; i < 20 ; i++){
	//	printf("%x ",compressed[i]);
	//}
	//printf("\n");
	// Verilog compress
	//for (int i = 0; i < compsize ; i++){
	//	tilecode[i] = compressed[i];
	//}
	//unsigned char * decomressed = (unsigned char *) malloc((originsize));   
    //rledecompress(compressed,decomressed,*size);
	//jlsdecode(0,4,4, compressed,decomressed);
	top->io_control_start  = 1;
	top->io_size_height = picysize;
	top->io_size_widthh = picxsize;
	while (!top->io_control_finish)
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
		}
	}
	printf("Verilog Wave :%d\n",wavecount);
	//for(int i =0 ; i < compsize ; i++){
		//if(i%sizex==0&&(i!=0)) printf("\n");
		//printf("%x ",tilecode[i]);
	//}
	printf("\n");
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
		int originsize = *size ;
		//printf("origin size %d\n", *size  );
		//printf("origin size %d\n", *size  );
		unsigned char  compressed[5* (*size) ];
		//rlecompress(data,size ,compressed);
		//printf("origin size %d\n", *size  );
		compressARGB2tile(picdata,compressed,size); 
		int compsize = *size;
		//printf("compsize %d\n",compsize ); 
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
#elif TESTMODULE == 2
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
#elif TESTMODULE == 3
#define ENCODETEST 0
void encodetest(){
	//cpp compress
#if ENCODETEST
	int  sizex = 4;
	int number = sizex*sizex;
	for (int i = 0; i < number*4 ; i ++) {
		tiledata[i] = (unsigned char)(rand() % 256);
		//if(i%sizex==0&&(i!=0)) printf("\n");
		printf("%d\t",tiledata[i]);
	}
	printf("\n");
	int *size  = new int(sizeof(tiledata)/sizeof(unsigned char));
    //int originsize = *size ;
	//cout << *size << endl ;
	int originsize = *size ;
	printf("origin size %d\n", *size  );
	unsigned char  compressed[10* (*size) ];
	//rlecompress(data,size ,compressed);
	jpeglscompress(tiledata,size,compressed,0,4,4); 
	int compsize = *size;
	printf("compsize %d\n",compsize ); 
	for(int i =0 ; i < compsize ; i++){
		printf("%x ",compressed[i]);
	}
	printf("\n");
	// Verilog compress
	//for (int i = 0; i < compsize ; i++){
	//	tilecode[i] = compressed[i];
	//}
	//unsigned char * decomressed = (unsigned char *) malloc((originsize));   
    //rledecompress(compressed,decomressed,*size);
	//jlsdecode(0,4,4, compressed,decomressed);
	top->io_tcontrol_start  = 1;
	while (!top->io_tcontrol_finish)
	{
		clockntimes(1);
	}
	printf("Verilog len:%d compsize:%d\n",top->io_len,compsize);
	printf("Verilog results\n");
	for(int i =0 ; i < compsize ; i++){
		//if(i%sizex==0&&(i!=0)) printf("\n");
		printf("%x ",tilecode[i]);
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
		for (int i = 0; i < number*4 ; i ++) {
			tiledata[i] = (unsigned char)(rand() % 256);
		//if(i%sizex==0&&(i!=0)) printf("\n");
			//printf("%d\t",tiledata[i]);
		}
		//printf("\n");
		int *size  = new int(sizeof(tiledata)/sizeof(unsigned char));
    	//int originsize = *size ;
		//cout << *size << endl ;
		int originsize = *size ;
		//printf("origin size %d\n", *size  );
		unsigned char  compressed[10* (*size) ];
		//rlecompress(data,size ,compressed);
		jpeglscompress(tiledata,size,compressed,0,4,4); 
		int compsize = *size;

		top->io_tcontrol_start  = 1;
		while (!top->io_tcontrol_finish)
		{
			clockntimes(1);
		}
		top->io_tcontrol_start  = 0;
		clockntimes(10);
		//reset(5);
		//printf("Verilog len:%d compsize:%d\n",top->io_len,compsize);
		//printf("Verilog results\n");
		//for(int i =0 ; i < compsize ; i++){
		//	//if(i%sizex==0&&(i!=0)) printf("\n");
		//	printf("%x ",tilecode[i]);
		//}
		//printf("\n");
		
		for (int i = 0; i < compsize ; i ++) {
			//if(i%4==0&&(i!=0)) printf("\n");
			//printf("%d\t",decomressed[i]);
			if(tilecode[i] != compressed[i]) {
				flag = 0;
			//	printf ("rle wrong !\n");
				//break;
			}
		}
		printf("count:%d\n",count);
		//for (int i = 0; i < number ; i ++) {
		//	if(i%sizex==0&&(i!=0)) printf("\n");
		//	printf("%d\t",data[i]);
		//}
		//printf("\n");
		if(!flag) {
			printf("count:%d\n",count);
			for (int i = 0; i < number*4 ; i ++) {
				//if(i%sizex==0&&(i!=0)) printf("\n");
				printf("%d\t",tiledata[i]);
			}
			printf("\n");
			printf("Verilog len:%d compsize:%d\n",top->io_len,compsize);
			for(int i =0 ; i < compsize ; i++){
			//if(i%sizex==0&&(i!=0)) printf("\n");
				printf("%x ",compressed[i]);
			}
			printf("\n");
			for(int i =0 ; i < compsize ; i++){
			//if(i%sizex==0&&(i!=0)) printf("\n");
				printf("%x ",tilecode[i]);
			}
			printf("\n");
			//for (int i = 0; i < number ; i ++) {
			//	if(i%sizex==0&&(i!=0)) printf("\n");
			//	printf("%d\t",data1[i]);
			//}
			//printf("\n");
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

#elif TESTMODULE == 4
#define DECODETEST 0
void decodetest(){
	//cpp compress
#if DECODETEST
	int  sizex = 4;
	int number = sizex*sizex;
	for (int i = 0; i < number*4 ; i ++) {
		tiledata[i] = (unsigned char)(rand() % 256);
		//if(i%sizex==0&&(i!=0)) printf("\n");
		printf("%d\t",tiledata[i]);
	}
	printf("\n");
	int *size  = new int(sizeof(tiledata)/sizeof(unsigned char));
    //int originsize = *size ;
	//cout << *size << endl ;
	int originsize = *size ;
	printf("origin size %d\n", *size  );
	unsigned char  compressed[10* (*size) ];
	//rlecompress(data,size ,compressed);
	jpeglscompress(tiledata,size,compressed,0,4,4); 
	int compsize = *size;
	printf("compsize %d\n",compsize ); 
	for(int i =0 ; i < compsize ; i++){
		printf("%x ",compressed[i]);
	}
	printf("\n");
	for(int i =0 ; i < compsize ; i++){
		tilecode[i] = compressed[i];
	}
	printf("hhhh\n");
	// Verilog compress
	//for (int i = 0; i < compsize ; i++){
	//	tilecode[i] = compressed[i];
	//}
	//unsigned char * decomressed = (unsigned char *) malloc((originsize));   
    //rledecompress(compressed,decomressed,*size);
	//jlsdecode(0,4,4, compressed,decomressed);
	top->io_tcontrol_start  = 1;
	while (!top->io_tcontrol_finish)
	{
		clockntimes(1);
	}
	printf("Verilog len:%d originsize:%d\n",top->io_len,originsize);
	printf("Verilog results\n");
	for(int i =0 ; i < originsize ; i++){
		//if(i%sizex==0&&(i!=0)) printf("\n");
		printf("%d\t",tiledata1[i]);
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
		for (int i = 0; i < number*4 ; i ++) {
			tiledata[i] = (unsigned char)(rand() % 256);
		//if(i%sizex==0&&(i!=0)) printf("\n");
			//printf("%d\t",tiledata[i]);
		}
		//printf("\n");
		int *size  = new int(sizeof(tiledata)/sizeof(unsigned char));
    	//int originsize = *size ;
		//cout << *size << endl ;
		int originsize = *size ;
		//printf("origin size %d\n", *size  );
		unsigned char  compressed[10* (*size) ];
		//rlecompress(data,size ,compressed);
		jpeglscompress(tiledata,size,compressed,0,4,4); 
		int compsize = *size;
		for (int i = 0; i < compsize ; i ++) {
			//if(i%4==0&&(i!=0)) printf("\n");
			//printf("%d\t",decomressed[i]);
			tilecode[i] = compressed[i];
		}
		top->io_tcontrol_start  = 1;
		while (!top->io_tcontrol_finish)
		{
			clockntimes(1);
		}
		//reset(10);
		top->io_tcontrol_start  = 0;
		clockntimes(10);
		//reset(5);
		//printf("Verilog len:%d compsize:%d\n",top->io_len,compsize);
		//printf("Verilog results\n");
		//for(int i =0 ; i < compsize ; i++){
		//	//if(i%sizex==0&&(i!=0)) printf("\n");
		//	printf("%x ",tilecode[i]);
		//}
		//printf("\n");
		
		for (int i = 0; i < originsize ; i ++) {
			//if(i%4==0&&(i!=0)) printf("\n");
			//printf("%d\t",decomressed[i]);
			if(tiledata[i] != tiledata1[i]) {
				flag = 0;
				//printf ("rle wrong !\n");
				//break;
			}
		}
		printf("count:%d\n",count);
		//for (int i = 0; i < number ; i ++) {
		//	if(i%sizex==0&&(i!=0)) printf("\n");
		//	printf("%d\t",data[i]);
		//}
		//printf("\n");
		if(!flag) {
			printf("count:%d\n",count);
			for (int i = 0; i < number*4 ; i ++) {
				//if(i%sizex==0&&(i!=0)) printf("\n");
				printf("%d\t",tiledata[i]);
			}
			printf("\n");
			printf("Verilog len:%d compsize:%d\n",top->io_len,compsize);
			for (int i = 0; i < number*4 ; i ++) {
				//if(i%sizex==0&&(i!=0)) printf("\n");
				printf("%d\t",tiledata1[i]);
			}
			printf("\n");
			//for (int i = 0; i < number ; i ++) {
			//	if(i%sizex==0&&(i!=0)) printf("\n");
			//	printf("%d\t",data1[i]);
			//}
			//printf("\n");
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

#endif 

#endif
