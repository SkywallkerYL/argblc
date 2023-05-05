#include <iostream>
#include <queue>
#include <unordered_map>
#include <vector>
#include <bitset>

using namespace std;

// 哈夫曼树节点
struct Node {
    uint8_t data; // 数据
    int freq; // 频率
    Node *left, *right; // 左右子节点

    Node(uint8_t data, int freq) {
        this->data = data;
        this->freq = freq;
        left = right = nullptr;
    }

    // 比较函数，用于优先队列排序
    bool operator<(const Node &other) const {
        return freq > other.freq;
    }
};

// 建立哈夫曼树
Node *buildHuffmanTree(const unordered_map<uint8_t, int> &freq) {
    // 构建优先队列，按照频率从小到大排序
    priority_queue<Node> pq;
    for (auto p : freq) {
        pq.push(Node(p.first, p.second));
    }

    // 构建哈夫曼树
    while (pq.size() > 1) {
        Node *left = new Node(0, 0);
        Node *right = new Node(0, 0);
        *left = pq.top(); pq.pop();
        *right = pq.top(); pq.pop();
        Node *parent = new Node(0, left->freq + right->freq);
        parent->left = left;
        parent->right = right;
        pq.push(*parent);
    }

    return new Node((pq.top()));
	//return pq.top();
}

// 建立哈夫曼编码表
void buildHuffmanTable(Node *root, unordered_map<uint8_t, string> &table, string code) {
    if (!root) return;
    if (!root->left && !root->right) {
        table[root->data] = code;
        return;
    }
    buildHuffmanTable(root->left, table, code + "0");
    buildHuffmanTable(root->right, table, code + "1");
}

// 哈夫曼编码
string huffmanEncode(const uint8_t *data, int size, const unordered_map<uint8_t, string> &table) {
    string result = "";
    for (int i = 0; i < size; i++) {
        result += table.at(data[i]);
    }
    return result;
}

// 哈夫曼解码
string huffmanDecode(const string &data, Node *root) {
    string result = "";
    Node *node = root;
    for (int i = 0; i < data.size(); i++) {
        if (data[i] == '0') node = node->left;
        else node = node->right;
        if (!node->left && !node->right) {
            result += node->data;
            node = root;
        }
    }
    return result;
}

