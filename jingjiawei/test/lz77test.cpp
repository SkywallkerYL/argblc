/*************************************************************************
	> File Name: ../test/lz77test.cpp
	> Author: yangli
	> Mail: 577647772@qq.com 
	> Created Time: 2023年05月17日 星期三 10时30分55秒
 ************************************************************************/

#include <iostream>
#include <vector>
using namespace std;
struct Match {
    unsigned int offset;
    unsigned int length;
};

std::vector<unsigned char> compress(unsigned char* data, size_t size) {
    std::vector<unsigned char> output;
    size_t i = 0;
    while (i < size) {
        Match longest_match = {0, 0};
        for (unsigned int j = 1; j <= i && j <= 0xFFFF; j++) {
            unsigned int match_length = 0;
            while (i - j + match_length < i && data[i - j + match_length] == data[i + match_length] && match_length < 0x0F) {
                match_length++;
            }
            if (match_length > longest_match.length) {
                longest_match.offset = j;
                longest_match.length = match_length;
            }
        }
        if (longest_match.length > 2) {
            output.push_back(((longest_match.offset >> 8) & 0x0F) | (longest_match.length - 2));
            output.push_back(longest_match.offset & 0xFF);
            i += longest_match.length;
        } else {
            output.push_back(0);
            output.push_back(data[i++]);
        }
    }
    return output;
}

std::vector<unsigned char> decompress(unsigned char* data, size_t size) {
    std::vector<unsigned char> output;
    size_t i = 0;
    while (i < size) {
        unsigned int token = data[i++];
        if (token == 0) {
            output.push_back(data[i++]);
        } else {
            unsigned int match_offset = ((token >> 4) & 0x0F) << 8;
            match_offset |= data[i++];
            unsigned int match_length = (token & 0x0F) + 2;
            for (unsigned int j = 0; j < match_length; j++) {
                output.push_back(output[output.size() - match_offset]);
            }
        }
    }
    return output;
}
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

