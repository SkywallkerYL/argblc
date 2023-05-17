/*************************************************************************
	> File Name: ../test/lz4test.cpp
	> Author: yangli
	> Mail: 577647772@qq.com 
	> Created Time: 2023年05月17日 星期三 10时15分38秒
 ************************************************************************/

#include<iostream>
#include "../src/lz4.h"
using namespace std;

int main() {
    const unsigned char input[] = "abcfh_abcfh_abcfh";
    int input_size = sizeof(input)/sizeof(unsigned char );

    vector<unsigned char> compressed = compressLZ4(input, input_size);
    vector<unsigned char> decompressed = decompressLZ4(compressed.data(), compressed.size());

    cout << "Input size: " << input_size << endl;
    cout << "Compressed size: " << compressed.size() << endl;
    cout << "Decompressed size: " << decompressed.size() << endl;
	for (int i= 0 ; i < input_size ; i++){
		//printf("%c\n",(char)decompressed[i]);
		if(decompressed[i] !=input[i]) {
			printf("lz4wrong\n");
			break;
		}
	}
    return 0;
}
