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
	int size1 = readpixel(originpix,filename);
	if(size1 == -1) return 0;
//	printf("%d\n",data[217]);
	/************RLE编码***********/
	int n = size1; //sizeof(data )/sizeof(data[0]) - 1;
	uint8_t code[2*n];
	printf ("origin size: %d bytes\n", n );
	int m = rleEncode(data,n,code);
	printf ("rleEncode size: %d bytes\n",m);
	/***********RLE解码************/
	uint8_t res[n+1];
	int l = rleDecode(code,m,res);
	res [l] = (uint8_t)'\0';
	printf ("rleDecode size: %d bytes\n",l);
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
	//uint8_t data[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    int size = m;//sizeof(code) / sizeof(code[0]);
	if(size!= m) printf ("something maybe wrong!!! size:%d m:%d\n",size,m);
    // 计算频率
    unordered_map<uint8_t, int> freq;
    for (int i = 0; i < size; i++) {
        freq[code[i]]++;
    }

    // 建立哈夫曼树和编码表
    Node *root = buildHuffmanTree(freq);
    unordered_map<uint8_t, string> table;
    buildHuffmanTable(root, table, "");

    // 哈夫曼编码
    string encoded = huffmanEncode(code, size, table);
    //cout << "Encoded: " << encoded << endl;

    // 哈夫曼解码
    string decoded = huffmanDecode(encoded, root);
	bool halfflag = 1;
	if(decoded.size()!= size) printf ("something maybe wrong!!! size:%ld m:%d\n",decoded.size(),m);
	for (int i = 0 ; i < decoded.size();i++) {
		//printf("%d half:%d  code:%d\n",i,(uint8_t)decoded[i],code[i]);
		if((uint8_t)decoded[i] != (uint8_t)code[i]){
			halfflag = 0;
			printf("%d half:%d  code:%d\n",i,(uint8_t)decoded[i],code[i]);
			break;
		}
	}
	if(halfflag) {
		printf("haflman is right\n");
	}else printf ("halfman is wrong\n");
   // cout << "Decoded: ";
   // for (int i = 0; i < decoded.size(); i++) {
   //     cout << (int)decoded[i] << " ";
   // }
   // cout << endl;
	printf("rle encode size = %d * %d = %d bits\n",m,8,m*8);
	printf("halfman encode size = %ld bits\n",encoded.size());

	return 0;
}
