/*************************************************************************
	> File Name: halfman.h
	> Author: yangli
	> Mail: 577647772@qq.com 
	> Created Time: 2023年05月17日 星期三 10时48分14秒
 ************************************************************************/
#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>

struct Node {
    unsigned char value;
    unsigned int frequency;
    Node* left;
    Node* right;
};

struct Compare {
    bool operator()(Node* a, Node* b) {
        return a->frequency > b->frequency;
    }
};

Node* build_huffman_tree(unsigned char* data, size_t size) {
    std::unordered_map<unsigned char, unsigned int> frequency_map;
    for (size_t i = 0; i < size; i++) {
        frequency_map[data[i]]++;
    }
    std::priority_queue<Node*, std::vector<Node*>, Compare> pq;
    for (auto it = frequency_map.begin(); it != frequency_map.end(); it++) {
        Node* node = new Node();
        node->value = it->first;
        node->frequency = it->second;
        node->left = nullptr;
        node->right = nullptr;
        pq.push(node);
    }
    while (pq.size() > 1) {
        Node* left = pq.top();
        pq.pop();
        Node* right = pq.top();
        pq.pop();
        Node* parent = new Node();
        parent->value = 0;
        parent->frequency = left->frequency + right->frequency;
        parent->left = left;
        parent->right = right;
        pq.push(parent);
    }
    return pq.top();
}

void build_huffman_codes(Node* node, std::vector<unsigned char>& code, std::unordered_map<unsigned char, std::vector<unsigned char>>& code_map) {
    if (node->left == nullptr && node->right == nullptr) {
        code_map[node->value] = code;
        return;
    }
    code.push_back(0);
    build_huffman_codes(node->left, code, code_map);
    code.pop_back();
    code.push_back(1);
    build_huffman_codes(node->right, code, code_map);
    code.pop_back();
}

std::vector<unsigned char> encode_huffman(unsigned char* data, size_t size, std::unordered_map<unsigned char, std::vector<unsigned char>>& code_map) {
    std::vector<unsigned char> output;
    std::vector<unsigned char> buffer;
    for (size_t i = 0; i < size; i++) {
        auto it = code_map.find(data[i]);
        buffer.insert(buffer.end(), it->second.begin(), it->second.end());
        while (buffer.size() >= 8) {
            unsigned char byte = 0;
            for (int j = 0; j < 8; j++) {
                byte |= buffer[j] << (7 - j);
            }
            output.push_back(byte);
            buffer.erase(buffer.begin(), buffer.begin() + 8);
        }
    }
    if (!buffer.empty()) {
        unsigned char byte = 0;
        for (size_t i = 0; i < buffer.size(); i++) {
            byte |= buffer[i] << (7 - i);
        }
        output.push_back(byte);
    }
    return output;
}

std::vector<unsigned char> decode_huffman(unsigned char* data, size_t size, Node* root) {
    std::vector<unsigned char> output;
    Node* node = root;
    for (size_t i = 0; i < size; i++) {
        for (int j = 7; j >= 0; j--) {
            if (node->left == nullptr && node->right == nullptr) {
                output.push_back(node->value);
                node = root;
            }
            if ((data[i] >> j) & 1) {
                node = node->right;
            } else {
                node = node->left;
            }
        }
    }
    if (node->left == nullptr && node->right == nullptr) {
        output.push_back(node->value);
    }
    return output;
}

void encode_huffman_tree(Node* node, std::vector<unsigned char>& output) {
    if (node->left == nullptr && node->right == nullptr) {
        output.push_back(1);
        output.push_back(node->value);
        return;
    }
    output.push_back(0);
    encode_huffman_tree(node->left, output);
    encode_huffman_tree(node->right, output);
}

Node* decode_huffman_tree(unsigned char* data, size_t size, size_t& index) {
    if (index >= size) {
        return nullptr;
    }
    if (data[index++] == 1) {
        Node* node = new Node();
        node->value = data[index++];
        node->frequency = 0;
        node->left = nullptr;
        node->right = nullptr;
        return node;
    }
    Node* node = new Node();
    node->value = 0;
    node->frequency = 0;
    node->left = decode_huffman_tree(data, size, index);
    node->right = decode_huffman_tree(data, size, index);
    return node;
}
int halfmanCompress(const unsigned char* data,int *size,unsigned char* cpdata ){
	 
	unsigned char datain[(*size)];
	Node* root = build_huffman_tree(datain, *size);

    std::unordered_map<unsigned char, std::vector<unsigned char>> code_map;
    std::vector<unsigned char> code;
    build_huffman_codes(root, code, code_map);
    std::vector<unsigned char> compressed_data = encode_huffman(datain, *size, code_map);
    std::vector<unsigned char> huffman_tree;
    encode_huffman_tree(root, huffman_tree);
	//huffmantree 内部记录的是root的压缩信息 

	for (int i = 0;i ++ ;i<huffman_tree.size() ){
		cpdata[i] = huffman_tree[i];	
	}
	*size = huffman_tree.size();
	return 0;
}
int halfmanDecompress(const unsigned char* code, int size , unsigned char* data){
	//size_t root_index = 0;
    //Node* root_node = decode_huffman_tree(huffman_tree.data(), huffman_tree.size(),root_index);
    //std::vector<unsigned char> decompressed_data = decode_huffman(compressed_data.data(), compressed_data.size(), root_node);

	return 0;
}
