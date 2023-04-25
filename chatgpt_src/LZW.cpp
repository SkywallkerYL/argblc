/*
compress函数实现了LZW算法的压缩部分，decompress函数实现了LZW算法的解压部分。
在compress函数中，我们使用了unordered_map来实现字典，其中键为字符串，值为编码。
在解压部分中，我们同样使用了unordered_map来实现字典，其中键为编码，值为字符串。

在compress函数中，我们首先初始化字典，将每个字符和其对应的ASCII码作为编码存入字典。
然后我们遍历原始字符串，对于每个字符，我们将其和前面的字符串拼接成一个新的字符串，如果该字符串在字典中已经存在，
则继续往后遍历；
否则，我们将前面的字符串的编码存入结果中，并将该字符串添加到字典中。
最后，我们将所有的编码拼接成一个字符串，作为压缩后的结果。

在decompress函数中，我们同样初始化字典，将每个字符和其对应的ASCII码作为编码存入字典。
然后我们将编码序列转换成字符串序列，对于每个编码，如果字典中已经存在该编码，则将其对应的字符串添加到输出中，
并将prev更新为该字符串；否则，我们将prev和prev[0]拼接成一个新的字符串，并将其添加到输出中。
最后，我们将所有的字符串拼接成一个字符串，作为解压后的结果。



*/



#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>

using namespace std;

// 压缩函数，输入为原始字符串，输出为编码后的字符串
string compress(string input) {
    unordered_map<string, int> dictionary; // 字典，用于存储字符串和对应的编码
    for (int i = 0; i < 256; i++) { // 初始化字典，将每个字符和其对应的ASCII码作为编码存入字典
        string s = "";
        s += char(i);
        dictionary[s] = i;
    }

    string s = "";
    vector<int> result; // 存储编码后的结果
    for (char c : input) {
        string s2 = s + c; // 将s和c拼接成一个新的字符串s2
        if (dictionary.count(s2)) { // 如果字典中已经存在s2，则继续往后遍历
            s = s2; // 将s更新为s2
        } else { // 如果字典中不存在s2，则将s的编码存入结果中，并将s2添加到字典中
            result.push_back(dictionary[s]);
            dictionary[s2] = dictionary.size(); // 将s2的编码存入字典
            s = string(1, c); // 将s更新为c
        }
    }
    if (!s.empty()) { // 处理最后一个字符
        result.push_back(dictionary[s]);
    }

    string output;
    for (int i : result) { // 将所有的编码拼接成一个字符串
        output += to_string(i) + " ";
    }
    return output;
}

// 解压函数，输入为编码后的字符串，输出为原始字符串
string decompress(string input) {
    unordered_map<int, string> dictionary; // 字典，用于存储编码和对应的字符串
    for (int i = 0; i < 256; i++) { // 初始化字典，将每个字符和其对应的ASCII码作为编码存入字典
        string s = "";
        s += char(i);
        dictionary[i] = s;
    }

    vector<int> codes; // 存储所有的编码
    string s = "";
    for (char c : input) { // 将字符串转换成编码序列
        if (c == ' ') { // 如果遇到空格，则将当前的编码存入codes中，并清空s
            codes.push_back(stoi(s));
            s = "";
        } else {
            s += c;
        }
    }
    if (!s.empty()) { // 处理最后一个编码
        codes.push_back(stoi(s));
    }

    string output;
    string prev = dictionary[codes[0]]; // 将第一个编码对应的字符串添加到输出中
    output += prev;
    int count = 256; // 从256开始分配新的编码
    for (int i = 1; i < codes.size(); i++) {
        int code = codes[i];
        string s;
        if (dictionary.count(code)) { // 如果字典中已经存在该编码，则将其对应的字符串添加到输出中，并将prev更新为该字符串
            s = dictionary[code];
        } else { // 如果字典中不存在该编码，则将prev和prev[0]拼接成一个新的字符串，并将其添加到输出中
            s = prev + prev[0];
        }
        output += s;
        dictionary[count] = prev + s[0]; // 将prev和s[0]拼接成一个新的字符串，并将其添加到字典中
        count++;
        prev = s; // 将prev更新为s
    }
    return output;
}

int main() {
    string input = "ababcbababaaaaaa"; // 原始字符串
    string compressed = compress(input); // 压缩后的字符串
    string decompressed = decompress(compressed); // 解压后的字符串

    cout << "Original string: " << input << endl;
    cout << "Compressed string: " << compressed << endl;
    cout << "Decompressed string: " << decompressed << endl;
    return 0;
}
