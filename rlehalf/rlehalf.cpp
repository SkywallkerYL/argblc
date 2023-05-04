/*************************************************************************
	> File Name: rlehalf.cpp
	> Author: yangli
	> Mail: 577647772@qq.com 
	> Created Time: 2023年05月04日 星期四 18时34分30秒
 ************************************************************************/

#include<iostream>
#include <cstring>
#include "halfman.h"
#include "rlenew.h"
#include "readimg.h"
using namespace std;
int dataMaxSize = 0x7fffffff;
int main (){
	
	// rle test 
	//char data[] = "aaaaaaawdwndehaskdnmbckjhaweeeeeeeeeeeebadxxaxwedasssssss9810278hhh832asdasascnkewhfdaknbsjbcbsdjbvweuyfgaskjdhakjsdhkjcnbxjncbsdvchksbjvgyue8778787878887";
	/*****读取文件*******/
	char filename[] = "./imgsrc/r.txt";
	uint8_t *data = (uint8_t *) malloc(dataMaxSize * sizeof(uint8_t)); 
	uint8_t *originpix = data;
	int size = readpixel(originpix,filename);
	if(size == -1) return 0;
	/************RLE编码***********/
	int n = size; //sizeof(data )/sizeof(data[0]) - 1;
	uint8_t code[2*n];
	printf ("origin size: %d\n", n );
	int m = rleEncode(data,n,code);
	printf ("rleEncode size: %d\n",m);
	/***********RLE解码************/
	uint8_t res[n+1];
	int l = rleDecode(code,m,res);
	res [l] = (uint8_t)'\0';
	printf ("rleDecode size: %d\n",l);
	/************验证***********/
	bool sameflag = 1;
	for(int i = 0 ; i < l ; i ++){
		if(data[i] != res[i]) {
			sameflag = 0;
			break;
		}
	}
	if(sameflag) {
		printf ("rle decode is right\n");
	}
	else printf ("rle decode is wrong\n");

	/************HALFMAN 目前还有问题*************/
	char forhalfres[l+1];
	for(int i = 0 ; i < l +1 ; i++){
		forhalfres[i] =(char) res[l];
	}
	forhalfres[l] = '\0';
	// halfman 
	printf("halfman begin \n");
	string str(forhalfres,forhalfres+l);// = "hello world"; // 原始字符串
	printf("Halfman hhh\n");
    unordered_map<uint8_t, string> code_map = huffmanEncode(str); // 哈夫曼编码
    string encoded_str = "";
    for(uint8_t c : str){
        encoded_str += code_map[c]; // 将编码后的字符拼接成字符串
    }
    HuffmanNode* root = buildHuffmanTree(str); // 构建哈夫曼树
    string decoded_str = huffmanDecode(encoded_str, root); // 哈夫曼解码
    // 输出结果
    //cout << "Original string: " << str << endl;
    //cout << "Encoded string: " << encoded_str << endl;
    //cout << "Decoded string: " << decoded_str << endl;
	printf("rle encode size = %d * %d = %d bits\n",m,8,m*8);
	printf("halfman encode size = %ld bits\n",encoded_str.size());

	return 0;
}
