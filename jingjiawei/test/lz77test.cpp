/*************************************************************************
	> File Name: ../test/lz77test.cpp
	> Author: yangli
	> Mail: 577647772@qq.com 
	> Created Time: 2023年05月17日 星期三 10时30分55秒
 ************************************************************************/

#include <iostream>
#include <cstring>
#include <vector>
#include "../src/lz77.h"
using namespace std;



/*
int lz77Compress(const unsigned char* datain,int * size , unsigned char* cpdata){
	unsigned char data [*size];
	for (int i = 0;i< *size ; i++){
		data[i] = datain[i];
	}
	std::vector<unsigned char> compressed_data = compress(data, *size);
	for (int i = 0; i < compressed_data.size();i++){
		cpdata[i] = compressed_data[i];
	}
	*size = compressed_data.size();
	return 0;
}
int lz77Decompress(const unsigned char* cpdata,int size , unsigned char* origin ){
	vector<unsigned char> compressed_data;
	compressed_data.resize(size);	
	for (int i =0;i < size ;i++){
		compressed_data [i] = cpdata[i];
	}
    std::vector<unsigned char> decompressed_data = decompress(compressed_data.data(), compressed_data.size());
	int originsize = decompressed_data.size();
	for(int i = 0; i < originsize ; i++){
		origin[i] = decompressed_data[i];
	}
	return originsize;
}
*/

int main() {
    const unsigned char data[] = "Hello, World";
    int * size =new int (sizeof(data) / sizeof(data[0]));
	int localsize = *size;
	unsigned char cpdata [100];
	lz77Compress(data,size,cpdata);
	for(int i = 0 ; i < *size ; i++){
		cout << cpdata[i];
	}cout <<endl;
	unsigned char origin[100];
	int originsize = lz77Decompress(cpdata,*size,origin) ;
	for(int i = 0;i<originsize ; i++){
		cout << origin[i];
		if(origin[i] != data[i]){
			cout<<"Lz77 wrong!!!"<<endl;
			break;
		}
	}cout <<endl;
	cout << "origin size: "<<localsize << endl;
	cout <<"compress size: "<<*size << endl;
	cout << "decompress size: "<<originsize << endl;
	delete size;
    return 0;
}

