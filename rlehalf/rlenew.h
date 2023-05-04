/*************************************************************************
	> File Name: rlenew.h
	> Author: yangli
	> Mail: 577647772@qq.com 
	> Created Time: 2023年05月04日 星期四 09时33分22秒
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <set>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>
#include <random>

using namespace std;
/*
 * 改进的rle 算法，最大计数128个，计数位最高为标记后面的数据是重复的还是非重复的
 * 为1 表示重复，为0表示非重复。
 *
 *
 * 
 */
int rleEncode (const uint8_t* data , int n, uint8_t * code) {

	int k = 0;
	for(int i = 0; i < n ; i ++ ){
		uint8_t c = data[i];
		//printf ("%c ",c);
		uint8_t count = 0;    // 0 的时候表示1个，这样子就能计数128个了
		while ((i < n - 1) && (data[i] == data[i+1]) &&( count < 127)){
			count ++ ;
			i ++ ;
		}
		//printf ("i:%d\n",i);
		if (count >= 1) {
			//printf ("posr: %d %d\n", k,count );
			code[k++] = count | 0x80;
			code[k++] = c ;
		} else {
			// count < 1 当作非重复
			int countpos = k ; // 记录当前非重复的位置，统计完成之后还要回来修改
			//printf ("pos: %d ", k );
			code [k++] = count;
			while (i < n - 1 &&data[i] != data[i+1] && count < 127) {
				uint8_t c_next = data[i];
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
int rleDecode(const uint8_t *code, int n, uint8_t* res){
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
			for(int j = 0; j <= count; j++){
				res[k++] = code[i];
			} 
		}else {
			i++;
			//非重复数据 直接往后读count+1个字符
			for(int j = 0 ; j <= count; j++){
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
