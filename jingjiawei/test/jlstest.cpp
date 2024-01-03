/*************************************************************************
	> File Name: rletest.cpp
	> Author: yangli
	> Mail: 577647772@qq.com 
	> Created Time: 2023年05月16日 星期二 13时11分49秒
 ************************************************************************/

#include<iostream>
#include <cstdlib>
#include <cstring>
#include "../src/jlsdecode.h"
#include "../src/jlsencode.h"

#define FIND 1
//#include "./jlsdecode.h"
//#include "./jlsencode.h"
using namespace std;
#if FIND 
int main () {
	//unsigned char data[] = {
	//				(unsigned char)0  ,(unsigned char)0  ,(unsigned char)90 ,(unsigned char)74 ,
	//				(unsigned char)68 ,(unsigned char)50 ,(unsigned char)43 ,(unsigned char)205,
	//				(unsigned char)64 ,(unsigned char)145,(unsigned char)145,(unsigned char)145,
	//				(unsigned char)100,(unsigned char)145,(unsigned char)145,(unsigned char)145
	//};
	//unsigned char data[] = {
	//				(unsigned char)255  ,(unsigned char)133  ,(unsigned char)23 ,(unsigned char)25 ,
	//				(unsigned char)255 ,(unsigned char)255 ,(unsigned char)255 ,(unsigned char)252,
	//				(unsigned char)235 ,(unsigned char)23,(unsigned char)125,(unsigned char)135,
	//				(unsigned char)225,(unsigned char)179,(unsigned char)215,(unsigned char)163
	//};
	int count = 0;
	int  sizex = 8;
	int number = sizex*sizex;
	bool flag = 1;
	while (flag && (count <= 100000))
	{
		/* code */
		srand(count*6+12358);
	unsigned char data[number];
	for (int i = 0 ; i  <  number;i++){
		data[i] = (unsigned char)(rand() % 256);
	}
	//for (int i = 0; i < 16 ; i ++) {
	//	if(i%4==0&&(i!=0)) printf("\n");
	//	printf("%d\t",data[i]);
	//}
	//printf("\n");
	int *size  = new int(sizeof(data)/sizeof(unsigned char));
    //int originsize = *size ;
	//cout << *size << endl ;
	int originsize = *size ;
	//printf("origin size %d\n", *size  );
	unsigned char  compressed[10* (*size) ];
	//rlecompress(data,size ,compressed);
	int compsize = jpeglsencode(data, originsize , compressed ) ;
	//printf("compsize %d\n",compsize ); 
	//for(int i =0 ; i < compsize ; i++){
	//	printf("%x ",compressed[i]);
	//}
	//printf("\n");
	unsigned char * decomressed = (unsigned char *) malloc((originsize));   
    //rledecompress(compressed,decomressed,*size);
	jlsdecode(0,sizex,sizex, compressed,decomressed);
	for (int i = 0; i < originsize ; i ++) {
		//if(i%4==0&&(i!=0)) printf("\n");
		//printf("%d\t",decomressed[i]);
		if(decomressed[i] != data[i]) {
			flag = 0;
		//	printf ("rle wrong !\n");
			//break;
		}
	}
	if(!flag) {
		for (int i = 0; i < number ; i ++) {
			if(i%sizex==0&&(i!=0)) printf("\n");
			printf("%d\t",data[i]);
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
}
#else
int main () {
	unsigned char data[] = {
					(unsigned char)0  ,(unsigned char)0  ,(unsigned char)90 ,(unsigned char)74 ,
					(unsigned char)68 ,(unsigned char)50 ,(unsigned char)43 ,(unsigned char)205,
					(unsigned char)64 ,(unsigned char)145,(unsigned char)145,(unsigned char)145,
					(unsigned char)100,(unsigned char)145,(unsigned char)145,(unsigned char)145
	};
	//unsigned char data[] = {
	//				(unsigned char)6	,(unsigned char)0  ,(unsigned char)13 ,(unsigned char)217,
	//				(unsigned char)6	,(unsigned char)233,(unsigned char)187,(unsigned char)212,
	//				(unsigned char)227	,(unsigned char)0  ,(unsigned char)117,(unsigned char)214,
	//				(unsigned char)125	,(unsigned char)154,(unsigned char)42 ,(unsigned char)189
	//};
	//unsigned char data[] = {
	//				(unsigned char)64 ,(unsigned char)211,(unsigned char)0  ,(unsigned char)0  ,
	//				(unsigned char)66 ,(unsigned char)171,(unsigned char)224,(unsigned char)55 ,
	//				(unsigned char)78 ,(unsigned char)236,(unsigned char)168,(unsigned char)179,
	//				(unsigned char)123,(unsigned char)53 ,(unsigned char)191,(unsigned char)233
	//};
	//unsigned char data[] = {
	//				(unsigned char)161  ,(unsigned char)149,
	//				(unsigned char)113  ,(unsigned char)126
	//				//(unsigned char)161  ,(unsigned char)149,
	//};
	int count = 0;
	bool flag = 1;
	
		/* code */
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
	unsigned char * decomressed = (unsigned char *) malloc((originsize));   
    //rledecompress(compressed,decomressed,*size);
	jlsdecode(0,sizex,sizex, compressed,decomressed);
	for (int i = 0; i < originsize ; i ++) {
		if(i%sizex==0&&(i!=0)) printf("\n");
		printf("%d\t",decomressed[i]);
		if(decomressed[i] != data[i]) {
			flag = 0;
		//	printf ("rle wrong !\n");
			//break;
		}
	}
	count++;
	printf("\n");

	delete size ;
}
#endif