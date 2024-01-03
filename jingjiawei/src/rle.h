/*************************************************************************
	> File Name: rlenew.h
	> Author: yangli
	> Mail: 577647772@qq.com 
	> Created Time: 2023年05月04日 星期四 09时33分22秒
 ************************************************************************/
#ifndef RLE_H
#define RLE_H 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <cstdlib>
#include <cstdint>
#include <cstdio>

const int MAX_SIZE_C = 1024;
using namespace std;
/*
 * 改进的rle 算法，最大计数128个，计数位最高为标记后面的数据是重复的还是非重复的
 * 为1 表示重复，为0表示非重复。
 *
 *
 * 
 */
int rleEncode (const unsigned char* data , int n, unsigned char * code) {

	int k = 0;
	for(int i = 0; i < n ; i ++ ){
		unsigned char c = data[i];
		//printf ("%c ",c);
		//为了能够对rle的数据也进行压缩，count从0开始，这样子经过rle处理过的数据
		//第一个数据就不可能是0，这样子来判断新的数据，是否是再次压缩的，但这样子就只能计数127个了。
		//注意 rletest.cpp 不能用，因为对8*8的块作了特殊处理。
		uint8_t count = 1;    // 0 的时候表示1个，这样子就能计数128个了
		while ((i < n - 1) && (data[i] == data[i+1]) &&( count < 127)){
			count ++ ;
			i ++ ;
		}
		//printf ("i:%d\n",i);
		if (count >= 2) {
			//printf ("posr: %d %d\n", k,count );
			code[k++] = count | 0x80;
			code[k++] = c ;
		} else {
			// count < 1 当作非重复
			int countpos = k ; // 记录当前非重复的位置，统计完成之后还要回来修改
			//printf ("pos: %d ", k );
			code [k++] = count;
			while (i < n - 1 &&data[i] != data[i+1] && count < 127) {
				unsigned char c_next = data[i];
				//printf ("%d %c %d\n",i,data[i],count);
				code [k++] = c_next  ;
				count++ ;
			 	i ++ ;
			} 
			if(i == n - 1) {
				code [k++] = data[i];
			//	count ++ ;
			}
			else {
				//因为相同停下来 count要退回去
				count --;
				i -- ; 
			}
			code[countpos] = count; 
			//printf("%d\n",count);
		}
	}
	return k; 
}
int rleDecode(const unsigned char *code, int n, unsigned char * res){
	int k = 0;
//	for (int i = 0 ; i < n ; i ++ ){
//		bool sign = (code[i] & (0x80) )== 0x80;
//		if(sign) printf("repeat %d ",i);
//		printf("%d %x %x\n",i,code[i],code[i] & 0x80);
//	}

	for (int i = 0; i < n ; i ++){
		int count = code[i] & 0x7f; // 低7位的计数器
		//printf ("pos: %d count:%d\n", i,count );
		//注意 == 的优先级比 & 高，这里前面要加括号。
		if((code[i] & 0x80) == 0x80 ) {
			//重复数据  添加count+1次，
			
			i++;
			//printf ("pos: %d  ", i );
			for(int j = 1; j <= count; j++){
				res[k++] = code[i];
			} 
		}else {
			i++;
			//非重复数据 直接往后读count+1个字符
			for(int j = 1 ; j <= count; j++){
				res[k++] = code[i];
		//		printf("%d ",i);
				i++;
			}
			i--;

			//printf("norepeat i %d\n",i);
		}
	}	
	return k;
}
int rlecompress (const unsigned char* data, int *size, unsigned char* compressed ){
	//分离出 b g r a
	//*size = 
	int onesize = *size / 4 ; 	
//	cout << onesize << endl ;
//	printf("onesize : %d\n",*size );
	unsigned char b[onesize],g[onesize],r[onesize],a[onesize];
	int colorind = 0;
	for (int i = 0 ;i < onesize ; i ++ ){
		b[i] = data[colorind];
		g[i] = data[colorind+1];
		r[i] = data[colorind+2];
		a[i] = data[colorind+3];
		colorind += 4;
	}
	unsigned char bcode[2*onesize],gcode[2*onesize],rcode[2*onesize],acode[2*onesize];
	int bsize = rleEncode(b,onesize,bcode);
	int gsize = rleEncode(g,onesize,gcode);
	int rsize = rleEncode(r,onesize,rcode);
	int asize = rleEncode(a,onesize,acode);

//	char * bptr = (char*) malloc(bsize * sizeof(char));	
//	char * gptr = (char*) malloc(gsize * sizeof(char));	
//	char * rptr = (char*) malloc(rsize * sizeof(char));	
//	char * aptr = (char*) malloc(asize * sizeof(char));
//	
//	memcpy(bptr,bcode,bsize);
//	memcpy(gptr,gcode,gsize);
//	memcpy(rptr,rcode,rsize);
//	memcpy(aptr,acode,asize);
	// 将压缩完成的字符按顺序存入compressed ;
	unsigned char * compr = compressed;
	if(bsize >= onesize || gsize >= onesize || rsize >= onesize || asize>=onesize){
		//cout << onesize <<endl;
		memcpy(compr,b,onesize); 
		compr +=onesize;

		memcpy(compr,g,onesize);
		compr +=onesize;

		memcpy(compr,r,onesize);
		compr += onesize;

		memcpy(compr,a,onesize);
		compr += onesize ;

		*size = 4*onesize;
	} else {
		memcpy(compr,bcode,bsize); 
		compr +=bsize;
	
		memcpy(compr,gcode,gsize);
		compr +=gsize;
	
		memcpy(compr,rcode,rsize);
		compr += rsize ;
	
		memcpy(compr,acode,asize);
		compr += asize ;
	
		*size = bsize+gsize+rsize+asize;
	}
	return 0;
//	free(bptr);free(gptr);free(rptr);free(aptr);
}
//extern int g_nTileWidth ;
//extern int g_nTileHeight;
int rledecompress(const unsigned char * code , unsigned char *data , int size){
	unsigned char res [ MAX_SIZE_C];
	int originsize = 0 ;
	//printf("hhhh\n");
	if(size == 8 * 8 * 4){
		//cout << W << " "<<H << endl;
		memcpy(res,code,size);
		originsize = size ;
	} else {
		originsize = rleDecode(code,size,res);
	}
	//int originsize  = rleDecode(code,size,res);	
	//按照 b g r a 恢复 
	//根据压缩过程可以知道 code是按顺序存放bgra压缩后的数据
	//解压后也是按顺序存放bgra 压缩前的数据，只要按照顺序恢复即可 。
	int onesize = originsize / 4; 
	for (int i = 0 ; i < onesize; i++  ){
		data[i*4]   = res[i];
		data[i*4+1] = res[onesize+i];
		data[i*4+2] = res[onesize*2+i];
		data[i*4+3] = res[onesize*3+i];
	}
	return 0 ;

}
#endif
