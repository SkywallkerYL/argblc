/*************************************************************************
	> File Name: rletest.cpp
	> Author: yangli
	> Mail: 577647772@qq.com 
	> Created Time: 2023年05月04日 星期四 10时36分04秒
 ************************************************************************/
#include "rlenew.h"
#include<iostream>
#include <cstring>
using namespace std;
int main () {
	// rle test 
	char data[] = "aaaaaaawdwndehaskdnmbckjhaweeeeeeeeeeeebadxxaxwedasssssss9810278hhh832asdasascnkewhfdaknbsjbcbsdjbvweuyfgaskjdhakjsdhkjcnbxjncbsdvchksbjvgyue8778787878887";
	//char data[256]; 
	//for(int i = 0 ; i < 256 ; i ++ ){
	//	data [i] = 'a';	
	//}
	//data[255] = '\0';
	int n = sizeof(data )/sizeof(data[0]) - 1;
	uint8_t code[2*n];
	printf ("origin size: %d\n", n );
//	printf("%s\n",data);
	uint8_t datai[n];
	for(int i = 0 ; i < n ; i ++ ){
		datai[i] = (uint8_t ) data[i];
	}
	int m = rleEncode(datai,n,code);
	printf ("rleEncode size: %d\n",m);
	//for (int i = 0 ; i < m ; i ++ ){
	//	printf("%x ",code[i]);
	//}printf("\n");
	uint8_t res[n+1];
	int l = rleDecode(code,m,res);
	res [l] = (uint8_t)'\0';
	printf ("rleDecode size: %d\n",l);
//	printf ("rleDecode string: %s\n",res);
	bool sameflag = 1;
	for(int i = 0 ; i < l ; i ++){
		if(datai[i] != res[i]) {
			sameflag = 0;
			break;
		}
	}
	if(sameflag) {
		printf ("rle decode is right\n");
	}
	else printf ("rle decode is wrong\n");
	//	uint8_t c = code[i] ; 		
    //}
}
