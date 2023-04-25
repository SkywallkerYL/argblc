/*
LZ4是一种无损数据压缩算法，它采用了LZ77算法和哈希表来实现高效的压缩和解压缩。LZ4算法的主要思想是将输入数据分成两种类型：字面量和匹配。字面量是指无法与之前的任何数据匹配的单个字符或字符串，而匹配是指与之前的某些数据匹配的一段字符串。

LZ4算法的压缩过程如下：

1初始化滑动窗口和哈希表。
2从输入数据中读取一小段数据，并将其视为字面量。
3从输入数据中读取一小段数据，并在哈希表中查找与之前的数据匹配的最长字符串。
4如果找到匹配，则将匹配信息写入输出缓冲区，并将匹配的数据从输入数据中删除。
5如果没有找到匹配，则将当前的字符视为字面量，并将其写入输出缓冲区。
重复步骤2-5，直到所有输入数据都被处理完毕。
如果输出缓冲区太大，则截断它。
LZ4算法的解压缩过程如下：

1从输入数据中读取压缩标志。
2如果压缩标志无效，则退出解压缩过程。
3从输入数据中读取一小段数据，并根据压缩标志解析出其中的字面量和匹配信息。
4如果是字面量，则将其写入输出缓冲区。
5如果是匹配信息，则将匹配的数据从输出缓冲区中复制出来，并将其写入输出缓冲区。
重复步骤3-5，直到所有输入数据都被处理完毕。



*/


#include <iostream>
#include <vector>
#include <cstring>
using namespace std;

// 哈希表大小
const int HASH_SIZE = 4096;

// 匹配信息结构体
struct MatchInfo {
    int distance;       // 距离
    int length;         // 长度
};

// 压缩函数
vector<char> compressLZ4(const char* input, int input_size) {
    // 初始化输出缓冲区
    vector<char> output;

    // 初始化滑动窗口和哈希表
    const int window_size = 64 * 1024;
    char window[window_size] = { 0 };
    int window_pos = 0;
    vector<int> hash_table[HASH_SIZE];

    // 遍历输入缓冲区
    for (int i = 0; i < input_size; ) {
        // 查找最长的匹配
        MatchInfo match = { 0, 0 };
        int hash = ((input[i] << 8) | input[i + 1]) % HASH_SIZE;

        for (int j = 0; j < hash_table[hash].size(); j++) {
            int k = hash_table[hash][j];
            int distance = i - k;
            int length = 0;

            while (input[i + length] == window[k + length] && length < 255 && i + length < input_size && k + length < window_pos + 256) {
                length++;
            }

            if (length > match.length) {
                match.distance = distance;
                match.length = length;
            }
        }

        // 如果找到匹配，则写入匹配信息
        if (match.length > 4) {
            output.push_back((match.distance >> 8) & 0xff);
            output.push_back(match.distance & 0xff);
            output.push_back(match.length - 4);
            i += match.length;
        }
        // 如果没有找到匹配，则写入字面量
        else {
            output.push_back(0);
            int literal_length = min(input_size - i, 255);
            output.push_back(literal_length - 1);

            for (int j = 0; j < literal_length; j++) {
                output.push_back(input[i + j]);
            }

            // 将字面量添加到滑动窗口和哈希表中
            for (int j = 0; j < literal_length; j++) {
                int k = (window_pos + j) % window_size;
                window[k] = input[i + j];
                hash_table[((input[i + j] << 8) | input[i + j + 1]) % HASH_SIZE].push_back(k);
            }

            window_pos = (window_pos + literal_length) % window_size;
            i += literal_length;
        }

        // 更新哈希表
        while (i - hash_table[((input[i - 2] << 8) | input[i - 1]) % HASH_SIZE][0] > 4095) {
            int k = hash_table[((input[i - 2] << 8) | input[i - 1]) % HASH_SIZE][0];
            hash_table[((input[k] << 8) | input[k + 1]) % HASH_SIZE].erase(hash_table[((input[k] << 8) | input[k + 1]) % HASH_SIZE].begin());
        }
    }

    // 将压缩标志写入输出缓冲区
    output.insert(output.begin(), 0x04);

    return output;
}

// 解压缩函数
vector<char> decompressLZ4(const char* input, int input_size) {
    // 初始化输出缓冲区
    vector<char> output;

    // 遍历输入缓冲区
    for (int i = 1; i < input_size; ) {
        // 读取压缩标志
        if (input[i] != 0) {
            break;
        }

        i++;

        // 读取字面量
        int literal_length = input[i] + 1;
        i++;

        for (int j = 0; j < literal_length; j++) {
            output.push_back(input[i + j]);
        }

        i += literal_length;

        // 读取匹配信息
        while (i < input_size && input[i] != 0) {
            int distance = (input[i] << 8) | input[i + 1];
            int length = (input[i + 2] & 0x0f) + 4;
            int skip = (input[i + 2] >> 4) + 1;
            i += 3;

            for (int j = 0; j < length; j++) {
                output.push_back(output[output.size() - distance - 1 + j]);
            }

            i += length - skip;
        }
    }

    return output;
}

// 测试函数
int main() {
    const char* input = "abcfh_abcfh_abcfh";
    int input_size = strlen(input);

    vector<char> compressed = compressLZ4(input, input_size);
    vector<char> decompressed = decompressLZ4(compressed.data(), compressed.size());

    cout << "Input size: " << input_size << endl;
    cout << "Compressed size: " << compressed.size() << endl;
    cout << "Decompressed size: " << decompressed.size() << endl;

    return 0;
}
