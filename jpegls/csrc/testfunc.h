#ifndef CHECKNODE_H
#define CHECKNODE_H 
/*************************************************************************
	> File Name: checknode.h
	> Author: yangli
	> Mail: 577647772@qq.com 
	> Created Time: 2023年05月21日 星期日 11时06分44秒
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


#define ENCODETEST 0
using namespace std;
#if TESTMODULE == 1
  
void encodetest(){
	//cpp compress
#if ENCODETEST
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

	top->io_econtrol_start  = 1;
	while (!top->io_econtrol_finish)
	{
		clockntimes(1);
	}
	for(int i =0 ; i < compsize ; i++){
		printf("%x ",coded[i]);
	}
	printf("\n");
#else 
	int count = 0;
	int  sizex = 4;
	int number = sizex*sizex;
	bool flag = 1;
	while (flag && (count <= 100000))
	{
		/* code */
		srand(count*6+12358);
	//unsigned char data[number];
		for (int i = 0 ; i  <  number;i++){
			data[i] = (unsigned char)(rand() % 256);
		}
		int *size  = new int(sizeof(data)/sizeof(unsigned char));
		int originsize = *size ;
		unsigned char  compressed[10* (*size) ];
		int compsize = jpeglsencode(data, originsize , compressed,4,4 ) ;
		
		top->io_econtrol_start  = 1;
		while (!top->io_econtrol_finish)
		{
			clockntimes(1);
		}
		reset(5);
		top->io_econtrol_start  = 0;
		
		for (int i = 0; i < compsize ; i ++) {
			//if(i%4==0&&(i!=0)) printf("\n");
			//printf("%d\t",decomressed[i]);
			if(compressed[i] != coded[i]) {
				flag = 0;
			//	printf ("rle wrong !\n");
				//break;
			}
		}
		printf("count:%d\n",count);
		for (int i = 0; i < number ; i ++) {
			if(i%sizex==0&&(i!=0)) printf("\n");
			printf("%d\t",data[i]);
		}
		printf("\n");
		if(!flag) {
			printf("count:%d\n",count);
			for (int i = 0; i < number ; i ++) {
				if(i%sizex==0&&(i!=0)) printf("\n");
				printf("%d\t",data[i]);
			}
			printf("\n");
			printf("compsize %d\n",compsize ); 
			for(int i =0 ; i < compsize ; i++){
				printf("%x ",compressed[i]);
			}
			printf("\n");

			for(int i =0 ; i < compsize ; i++){
				printf("%x ",coded[i]);
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
#elif TESTMODULE == 9 
void toptest(){
	
	for (int i = 0; i < 11 ; i ++) {
		double sigma = sigmatable[i];
		top->io_IterInput = ITERMAX;
		top->io_maxError  = maxerrortime; 
		top->io_p0_0 = p_table[i][4];
		top->io_p0_1 = p_table[i][3];
		top->io_p0_2 = p_table[i][2];
		top->io_p0_3 = p_table[i][1];
		top->io_p0_4 = p_table[i][0];
		top->io_start = 1;
		top->io_nextready = 0;
		clockntimes(1); 
		top->io_start = 0;
		while(top->io_errorframe < maxerrortime) {
			clockntimes(1);
		//	printf("error:%d\n",top->io_errorframe);
		}
		int frame =(int) top->io_totalframe ;
		int errorframe = (int )top->io_errorframe ; 
		double fer = (double) errorframe / (double) frame;
		double rate = (double)1723/(double)2048;
		double snr  = 10*log10(1.0/(2.0*rate*sigma*sigma));
		Log("snr:%f sigma:%f errorframe:%d frame:%d Fer:%f",snr,sigma,errorframe,frame,fer);
		reset(10);
		top->io_start = 0;
		clockntimes(10);
		//top->io_nextready = 1; 
		//clockntimes(10);
		//top->io_nextready = 0;
	}
  
}




#endif 

#endif
