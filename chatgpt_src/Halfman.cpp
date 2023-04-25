#include <iostream>
#include <queue>
#include <unordered_map>
/*
buildHuffmanTree函数用于构建哈夫曼树，huffmanEncode函数用于对输入的字符串进行哈夫曼编码，huffmanDecode函数用于对编码后的字符串进行解码。具体步骤如下：

定义HuffmanNode类表示哈夫曼树节点，其中freq表示节点的频率，ch表示节点代表的字符，left和right分别是左子树和右子树的指针。
定义Compare类作为比较函数，用于构建小根堆。
在buildHuffmanTree函数中，统计输入字符串中每个字符出现的频率，然后构建小根堆。
从小根堆中取出两个频率最小的节点，组成一个新的父节点，父节点的频率为两个子节点的频率之和，然后将父节点插入小根堆中。
重复步骤4直到只剩下一个节点，这个节点就是哈夫曼树的根节点。
在encodeHelper函数中，递归遍历哈夫曼树，对每个叶子节点生成对应的编码，并将字符和编码存入code_map中。
在huffmanEncode函数中，调用buildHuffmanTree函数构建哈夫曼树，然后调用encodeHelper函数生成哈夫曼编码，并将字符和编码存入code_map中。
在huffmanDecode函数中，根据编码和哈夫曼树进行解码，直到遇到叶子节点，将叶子节点代表的字符存入结果中。
在主函数中，定义一个原始字符串str，调用huffmanEncode函数对其进行编码，并将编码后的字符拼接成字符串encoded_str。然后调用buildHuffmanTree函数构建哈夫曼树，再调用huffmanDecode函数对encoded_str进行解码，得到解码后的字符串decoded_str。
最后输出原始字符串、编码后的字符串和解码后的字符串，以验证算法的正确性。


*/
using namespace std;

// 定义哈夫曼树节点
class HuffmanNode{
public:
    int freq; // 节点的频率
    char ch; // 节点代表的字符
    HuffmanNode* left; // 左子树指针
    HuffmanNode* right; // 右子树指针
    HuffmanNode(int f, char c){
        freq = f;
        ch = c;
        left = nullptr;
        right = nullptr;
    }
};

// 定义比较函数，用于构建小根堆
class Compare{
public:
    bool operator()(HuffmanNode* a, HuffmanNode* b){
        return a->freq > b->freq;
    }
};

// 构建哈夫曼树
HuffmanNode* buildHuffmanTree(string str){
    // 统计字符的频率
    unordered_map<char, int> freq_map;
    for(char c : str){
        freq_map[c]++;
    }
    // 构建小根堆
    priority_queue<HuffmanNode*, vector<HuffmanNode*>, Compare> min_heap;
    for(auto it = freq_map.begin(); it != freq_map.end(); it++){
        HuffmanNode* node = new HuffmanNode(it->second, it->first);
        min_heap.push(node);
    }
    // 构建哈夫曼树
    while(min_heap.size() > 1){
        HuffmanNode* left = min_heap.top();
        min_heap.pop();
        HuffmanNode* right = min_heap.top();
        min_heap.pop();
        HuffmanNode* parent = new HuffmanNode(left->freq + right->freq, '\0');
        parent->left = left;
        parent->right = right;
        min_heap.push(parent);
    }
    return min_heap.top();
}

// 递归实现哈夫曼编码
void encodeHelper(HuffmanNode* root, string code, unordered_map<char, string>& code_map){
    if(root == nullptr){
        return;
    }
    if(root->left == nullptr && root->right == nullptr){
        code_map[root->ch] = code;
    }
    encodeHelper(root->left, code + "0", code_map);
    encodeHelper(root->right, code + "1", code_map);
}

// 哈夫曼编码
unordered_map<char, string> huffmanEncode(string str){
    unordered_map<char, string> code_map; // 存储字符对应的编码
    HuffmanNode* root = buildHuffmanTree(str); // 构建哈夫曼树
    encodeHelper(root, "", code_map); // 递归实现哈夫曼编码
    return code_map;
}

// 哈夫曼解码
string huffmanDecode(string str, HuffmanNode* root){
    string res = "";
    HuffmanNode* curr = root;
    for(char c : str){
        if(c == '0'){
            curr = curr->left;
        }
        else{
            curr = curr->right;
        }
        if(curr->left == nullptr && curr->right == nullptr){
            res += curr->ch;
            curr = root;
        }
    }
    return res;
}

int main(){
    string str = "hello world"; // 原始字符串
    unordered_map<char, string> code_map = huffmanEncode(str); // 哈夫曼编码
    string encoded_str = "";
    for(char c : str){
        encoded_str += code_map[c]; // 将编码后的字符拼接成字符串
    }
    HuffmanNode* root = buildHuffmanTree(str); // 构建哈夫曼树
    string decoded_str = huffmanDecode(encoded_str, root); // 哈夫曼解码
    // 输出结果
    cout << "Original string: " << str << endl;
    cout << "Encoded string: " << encoded_str << endl;
    cout << "Decoded string: " << decoded_str << endl;
    return 0;
}
