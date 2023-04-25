#include <iostream>
#include <vector>
/*
RLE_encode函数用于对输入的数据进行压缩，RLE_decode函数用于对压缩后的数据进行解压。具体步骤如下：

定义一个空的vector encoded_data用于存储压缩后的数据。
定义变量count和current_num，分别用于计算连续重复的数字的个数和存储当前数字。
从第二个数字开始遍历输入的数据，如果当前数字和前一个数字相同，则计数器count加1；否则，将计数器的值和当前数字存入encoded_data中，并更新current_num和count。
继续遍历直到所有数字都处理完毕，最后将最后一个数字的计数器的值和数字本身存入encoded_data中。
RLE_decode函数的实现比较简单，就是依次读取压缩后的数据中的计数器和数字，将数字重复计数器次存入解压后的数据中。
在主函数中，定义一个原始数据data，压缩后的数据encoded_data和解压后的数据decoded_data，并分别调用RLE_encode和RLE_decode函数对数据进行压缩和解压。
最后输出原始数据、压缩后的数据和解压后的数据，以验证算法的正确性。


*/
using namespace std;

vector<int> RLE_encode(vector<int> data){
    vector<int> encoded_data; // 存储压缩后的数据
    int count = 1; // 用于计算连续重复的数字的个数
    int current_num = data[0]; // 当前数字
    for(int i = 1; i < data.size(); i++){
        if(data[i] == current_num){ // 如果当前数字和前一个数字相同
            count++; // 计数器加1
        }
        else{ // 如果当前数字和前一个数字不同
            encoded_data.push_back(count); // 将计数器的值存入压缩后的数据中
            encoded_data.push_back(current_num); // 将当前数字存入压缩后的数据中
            current_num = data[i]; // 更新当前数字
            count = 1; // 重置计数器
        }
    }
    // 处理最后一个数字
    encoded_data.push_back(count);
    encoded_data.push_back(current_num);
    return encoded_data;
}

vector<int> RLE_decode(vector<int> encoded_data){
    vector<int> decoded_data; // 存储解压后的数据
    for(int i = 0; i < encoded_data.size(); i += 2){
        int count = encoded_data[i]; // 获取计数器的值
        int num = encoded_data[i+1]; // 获取当前数字
        for(int j = 0; j < count; j++){
            decoded_data.push_back(num); // 将当前数字重复count次存入解压后的数据中
        }
    }
    return decoded_data;
}

int main(){
    vector<int> data = {1, 1, 2, 2, 2, 3, 4, 4, 4, 4, 5, 5}; // 原始数据
    vector<int> encoded_data = RLE_encode(data); // 压缩后的数据
    vector<int> decoded_data = RLE_decode(encoded_data); // 解压后的数据
    // 输出结果
    cout << "Original data:" << endl;
    for(int i = 0; i < data.size(); i++){
        cout << data[i] << " ";
    }
    cout << endl;
    cout << "Encoded data:" << endl;
    for(int i = 0; i < encoded_data.size(); i++){
        cout << encoded_data[i] << " ";
    }
    cout << endl;
    cout << "Decoded data:" << endl;
    for(int i = 0; i < decoded_data.size(); i++){
        cout << decoded_data[i] << " ";
    }
    cout << endl;
    return 0;
}
