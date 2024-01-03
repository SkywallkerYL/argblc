/*************************************************************************
	> File Name: rletest.cpp
	> Author: yangli
	> Mail: 577647772@qq.com 
	> Created Time: 2023年05月16日 星期二 13时11分49秒
 ************************************************************************/

#include<iostream>
#include "../src/rle.h"
using namespace std;
int main () {
	unsigned char data[] = {
					(unsigned char)245,(unsigned char)1,(unsigned char)0,(unsigned char)255,
					(unsigned char)255,(unsigned char)0,(unsigned char)2,(unsigned char)235,
					(unsigned char)255,(unsigned char)0,(unsigned char)0,(unsigned char)255,
					(unsigned char)255,(unsigned char)0,(unsigned char)0,(unsigned char)255
	};
	int *size  = new int(sizeof(data)/sizeof(unsigned char));
    //int originsize = *size ;
	//cout << *size << endl ;
	int originsize = *size ;
	printf("origin size %d\n", *size  );
	unsigned char  compressed[2* (*size) ];
	rlecompress(data,size ,compressed);
	printf("rle size %d\n",*size ); 
	unsigned char * decomressed = (unsigned char *) malloc((originsize));   
    rledecompress(compressed,decomressed,*size);
	for (int i = 0; i < originsize ; i ++) {
		printf("%d \n",decomressed[i]);
		if(decomressed[i] != data[i]) {
			printf ("rle wrong !\n");
			//break;
		}
	}
	delete size ;
}
