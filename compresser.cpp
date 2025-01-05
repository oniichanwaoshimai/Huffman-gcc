#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <memory>
#include <algorithm>
#include <bitset>
#define ascii 0x100
using namespace std;

void readFile(string fileName, int (& frequency)[ascii]) {
	for (auto& i : frequency)
		i = 0;
	{ 
		ifstream file(fileName);
		if (!file.is_open()) {
			cerr << "Error: Cannot open file " << fileName << endl;
			return;
		}

		char ch;
		while (file.get(ch)) {
			++frequency[(unsigned char)ch]; // вес кажого символа
		}
	}
}

struct Node { // узел	
	unsigned char ch{ 0 };
	int frequency{ 0 };
	shared_ptr<Node> right{ nullptr };
	shared_ptr<Node> left{ nullptr };

public:
	Node(unsigned char character, int freq) : ch(character), frequency(freq) {}
	Node(shared_ptr<Node> l, shared_ptr<Node> r) : left(l), right(r), frequency(l->frequency + r->frequency) {}
};

struct CompareNode { // чтобы приоритетная очередь корректно сравнивала УКАЗАТЕЛИ на узлы
	bool operator()(const shared_ptr<Node>& left, const shared_ptr<Node>& right) const {
		return left->frequency > right->frequency;
	}
};

void makeCodes(const shared_ptr<Node>& node, const string& prefix, vector<string>& codes) {
	if (!node) return;
	if (node->left == nullptr && node->right == nullptr)
		codes[(int)node->ch] = prefix;
	makeCodes(node->left, prefix + "0", codes);
	makeCodes(node->right, prefix + "1", codes);
}

void printTree(const shared_ptr<Node>& node, const string& prefix = "") { // ПРОВЕРКА: дерево Хаффмана
	if (!node) return;
	if (node->left == nullptr && node->right == nullptr)
		cout << "'" << node->ch << "' (" << node->frequency << ") : " << prefix << endl;
	printTree(node->left, prefix + "0");
	printTree(node->right, prefix + "1");
}

using queue_t = priority_queue<shared_ptr<Node>, vector<shared_ptr<Node>>, CompareNode>;

void queuePushing(int(&frequency)[ascii], queue_t& queue) {
	for (int i = 0; i < ascii; ++i) {
		if (frequency[i] > 0) {
			queue.push(make_shared<Node>((unsigned char)i, frequency[i]));
		}
	}
}

void buildTree(queue_t& queue) {
	while (queue.size() > 1) { // тк последняя итерация 2 эл-нта => 1 эл-нт
		shared_ptr<Node> left = queue.top(); // можно юзать auto и все ок, но я явно написал тип для лучшего понимания
		queue.pop();
		shared_ptr<Node> right = queue.top();
		queue.pop();

		queue.push(make_shared<Node>(left, right));
	}
}

void frequencyTest(int(&frequency)[ascii]) { // ПРОВЕРКА: вывод сколько веса символов ascii
	for (int i = 0; i < ascii; ++i) {
		if (frequency[i] > 0) {
			cout << frequency[i] << " " << (char)i << endl;
		}
	}
}

void codesTest(vector<string> codes) { // ПРОВЕРКА: символов и их кодов в векторе
	for (int i = 0; i < codes.size(); ++i) {
		cout << i << ": " << codes[i] << endl;
	}
}

string messageToCode(const string& filename, const vector<string>& codes) {
	string msg{ "" }; 
	{
		ifstream file(filename);
		if (!file.is_open()) {
			cerr << "Error: Cannot open file " << filename << endl;
			return msg;
		}

		char ch;
		while (file.get(ch)) {
			msg += codes[(unsigned char)ch];
		}
		return msg;
	}
}

void writeFile(const string& filename, int(&frequency)[ascii], const queue_t& queue, const string& message) {
	string newFilename = filename + ".huff";
	ofstream file(newFilename, ios::binary);
	if (!file.is_open()) {
		cerr << "Error: Cannot open file " << filename << endl;
		return;
	}

	unsigned char count = count_if(frequency, frequency+ascii, [](const int& value) { return (value != 0); });
	cout << "Count: " << (int)count << endl;

	file.write(reinterpret_cast<char*>(&count), sizeof count);

	for (int i = 0; i < ascii; ++i) {
		if (frequency[i] > 0) {
			cout << "[" << (unsigned char)i << "]" << " = " << frequency[i] << endl;
			unsigned char ch = static_cast<unsigned char>(i);
			file.write(reinterpret_cast<char*>(&ch), sizeof ch);
			file.write(reinterpret_cast<char*>(&frequency[i]), sizeof frequency[i]);
		}
	}

	int byte_count = message.size() / 8;
	unsigned char modulo = message.size() % 8;

	file.write(reinterpret_cast<char*>(&byte_count), sizeof byte_count);
	file.write(reinterpret_cast<char*>(&modulo), sizeof modulo);

	int i = 0;
	for (; i < byte_count; ++i) {
		bitset<8> b(message.substr(i * 8, 8));
		unsigned char value = static_cast<unsigned char>(b.to_ulong());
		file.write(reinterpret_cast<char*>(&value), sizeof value);
	}
	if (modulo > 0) {
		bitset<8> b(message.substr(i * 8, modulo));
		unsigned char value = static_cast<unsigned char>(b.to_ulong());
		file.write(reinterpret_cast<char*>(&value), sizeof value);
	}
}

int main() {
	system("chcp 1251");
	system("cls");
	int frequency[ascii];
	string filename = "test.txt";
	readFile(filename, frequency);

	//frequencyTest(frequency); // ПРОВЕРКА: вывод сколько веса символов ascii

	queue_t queue;

	queuePushing(frequency, queue);
	buildTree(queue);

	shared_ptr<Node> root = queue.top(); // последний выживший эл-нт. Само дерево
	
	//printTree(root); // ПРОВЕРКА: дерево Хаффмана

	vector<string> codes(0x100, "");

	makeCodes(root, "", codes);

	//codesTest(codes); // ПРОВЕРКА: символов и их кодов в векторе

	string message = messageToCode(filename, codes);
	cout << message << endl;

	writeFile(filename, frequency, queue, message);
}