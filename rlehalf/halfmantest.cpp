/*************************************************************************
	> File Name: haflmantest.cpp
	> Author: yangli
	> Mail: 577647772@qq.com 
	> Created Time: 2023年05月04日 星期四 18时15分38秒
 ************************************************************************/

#include<iostream>
#include "halfman.h"
using namespace std;
int main() {
	int n = 65536;
    uint8_t data [n];
	for (int i = 0; i < n ; i ++ ){
		data[i] = i %256;
	}
	data[0] = 255;data[1] = 255; data[2] = 216;
	// uint8_t data[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    int size = sizeof(data) / sizeof(data[0]);

    // 计算频率
    unordered_map<uint8_t, int> freq;
    for (int i = 0; i < size; i++) {
        freq[data[i]]++;
    }

    // 建立哈夫曼树和编码表
    Node *root = buildHuffmanTree(freq);
    unordered_map<uint8_t, string> table;
    buildHuffmanTable(root, table, "");

    // 哈夫曼编码
    string encoded = huffmanEncode(data, size, table);
    //cout << "Encoded: " << encoded << endl;

    // 哈夫曼解码
    string decoded = huffmanDecode(encoded, root);
	for (int i = 0; i < decoded.size(); i ++ ){
		if((uint8_t)decoded[i] != (uint8_t) data[i]){
			printf("halfman is wrong \n");
			break;
		}
	}
	if(decoded.size()!=n) printf("halfman maybe wrong \n");
   // cout << "Decoded: ";
   // for (int i = 0; i < decoded.size(); i++) {
   //     cout << (int)decoded[i] << " ";
   // }
   // cout << endl;

    return 0;
}
