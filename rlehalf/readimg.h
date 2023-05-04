/*************************************************************************
	> File Name: readimg.h
	> Author: yangli
	> Mail: 577647772@qq.com 
	> Created Time: 2023年05月04日 星期四 19时25分00秒
 ************************************************************************/

#include<iostream>
#include <fstream>
#include <vector>
using namespace std;

int readpixel (uint8_t *  pixel, char *filename) {
	ifstream pixelFile(filename);
	if(!pixelFile.is_open()){
		printf("No file %s\n",filename);
		return -1;
	} else printf("file %s\n",filename);
	vector<int> pixelVec;
	int value;
	while (pixelFile >> value) {
		pixelVec.push_back(value);
	}
	int Pixsize = pixelVec.size();
	printf("file line :%d\n",Pixsize);
	for(int i = 0; i < Pixsize ; i++){
		pixel[i] = pixelVec[i];
		
	}
	return Pixsize;
}
