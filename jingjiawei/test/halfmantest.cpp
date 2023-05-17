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
/*
	unsigned char data0 [] = "hello_world";
	int *size  = new int(sizeof(data0)/sizeof(unsigned char)); 
	int originsize = *size ;
	printf("origin size %d\n",*size);
	unsigned char compressed[(*size)];
	Node * root =  halfmanCompress(data0,size,compressed);

	printf("halfman size %d\n",*size);
	unsigned char * decomressed = (unsigned char *) malloc(originsize);
	halfmanDecompress(compressed,decomressed,*size,root);
	for (int i = 0; i < originsize ; i++){
		if(decomressed[i] != data0[i]){
			printf("halfman wrong!\n");
			break;
		}
		//cout<<decomressed[i]<<" ";
	}
	//cout <<endl;
	delete size ;
 */
    // 哈夫曼解码
   // string decoded = huffmanDecode(encoded, root);
	//	for (int i = 0; i < decoded.size(); i ++ ){
	//	if((uint8_t)decoded[i] != (uint8_t) data[i]){
	//		printf("halfman is wrong \n");
	//		break;
	//	}
	//}
	//if(decoded.size()!=n) printf("halfman maybe wrong \n");

    //cout << "Decoded: ";
    //for (int i = 0; i < decoded.size(); i++) {
     //   cout << (char)decoded[i] << " ";
    // }
     //cout << endl;
    unsigned char data[] = "Hello, World";
    size_t size = sizeof(data) / sizeof(data[0]);
    Node* root = build_huffman_tree(data, size);
    std::unordered_map<unsigned char, std::vector<unsigned char>> code_map;
    std::vector<unsigned char> code;
    build_huffman_codes(root, code, code_map);
    std::vector<unsigned char> compressed_data = encode_huffman(data, size, code_map);
    std::vector<unsigned char> huffman_tree;
    encode_huffman_tree(root, huffman_tree);
    size_t root_index = 0;
    Node* root_node = decode_huffman_tree(huffman_tree.data(), huffman_tree.size(),root_index);
    std::vector<unsigned char> decompressed_data = decode_huffman(compressed_data.data(), compressed_data.size(), root_node);

    std::cout << "Original data: " << data << std::endl;
    //std::cout << "Compressed data: ";
    //for (auto c : compressed_data) {
    //    std::cout << std::hex << (int)c << " ";
    //}
    //std::cout << std::endl;
	cout << "Huffman_tree size : "<< huffman_tree.size()<<endl;
	for(int i = 0 ; i < huffman_tree.size();i++){
		cout<< huffman_tree[i]<<" ";
	}cout <<endl;
    std::cout << "Decompressed data: " << decompressed_data.data() << std::endl;
	cout << "originsize : "<< size << " compresssize: "<<compressed_data.size()<<endl;
    return 0;
}
