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

void readFile(string fileName, int(&frequency)[ascii]) {
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
	unsigned char getCh() const { return ch; }
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

	unsigned char count = count_if(frequency, frequency + ascii, [](const int& value) { return (value != 0); });
	//cout << "Count: " << (int)count << endl;

	file.write(reinterpret_cast<char*>(&count), sizeof count);

	for (int i = 0; i < ascii; ++i) {
		if (frequency[i] > 0) {
			//cout << "[" << (unsigned char)i << "]" << " = " << frequency[i] << endl;
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

void readCompressedText(string& filename, int(&frequency)[ascii], string& message) {
	string newFilename = filename + ".huff";
	ifstream file(newFilename, ios::binary);
	if (!file.is_open()) {
		cerr << "Error: Cannot open file " << filename << endl;
		return;
	}

	unsigned char count = 0;
	file.read(reinterpret_cast<char*>(&count), sizeof count);
	//cout << "New count: " << (int)count << endl;

	int i = 0;
	while (i < count) {
		unsigned char ch;
		file.read(reinterpret_cast<char*>(&ch), sizeof ch);

		int freq = 0;
		file.read(reinterpret_cast<char*>(&freq), sizeof freq);
		frequency[ch] = freq;
		//cout << "[" << ch << "]" << " = " << frequency[ch] << endl;
		++i;
	}

	/*for (int j = 0; j < ascii; ++j) {
		if (frequency[j] > 0) {
			cout << "[" << (char)j << "]" << " = " << frequency[j] << endl;
		}
	}*/

	int byte_count = 0;
	unsigned char modulo = 0;

	file.read(reinterpret_cast<char*>(&byte_count), sizeof byte_count);
	file.read(reinterpret_cast<char*>(&modulo), sizeof modulo);

	i = 0;
	for (; i < byte_count; ++i) {
		unsigned char byte;
		file.read(reinterpret_cast<char*>(&byte), sizeof byte);

		bitset<8> b(byte);
		message += b.to_string();
	}
	if (modulo > 0) {
		unsigned char byte;
		file.read(reinterpret_cast<char*>(&byte), sizeof byte);

		bitset<8> b(byte);
		message += b.to_string().substr(8 - modulo, 8);
	}

	//cout << message << endl;
}

void makeChar(const shared_ptr<Node>& root, const string& message, string& text) {
	shared_ptr<Node> node = root;

	for (int i = 0; i < message.size(); ++i) {
		char ch = message[i];
		if (ch == '0') {
			if (node->left != nullptr) {
				node = node->left;
				if (node->left == nullptr && node->right == nullptr) {
					text += node->getCh();
					node = root;
				}
			}
		}
		else if (ch == '1') {
			if (node->right != nullptr) {
				node = node->right;
				if (node->left == nullptr && node->right == nullptr) {
					text += node->getCh();
					node = root;
				}
			}
		}
	}
	cout << "Text: \"" << text << "\"" << endl;
}

int main() {
	system("chcp 1251");
	system("cls");
	bool Flag = true;
	string ans = "";
	string filename = "test.txt";

	while (Flag) {
		cout << "1) Compresser" << endl;
		cout << "2) Decompresser" << endl;
		cout << "3) Filename" << endl;
		cout << "4) Exit: ";
		cin >> ans;
		if (ans == "1") {
			int frequency[ascii] = { 0 };
			readFile(filename, frequency);

			queue_t queue;

			queuePushing(frequency, queue);
			buildTree(queue);

			shared_ptr<Node> root = queue.top(); // последний выживший эл-нт. Само дерево

			vector<string> codes(0x100, "");

			makeCodes(root, "", codes);

			string message = messageToCode(filename, codes);

			writeFile(filename, frequency, queue, message);

			cout << "Done, Compressed!" << endl;
		}
		else if (ans == "2") {
			int frequency2[ascii] = { 0 };
			string message2 = "";

			readCompressedText(filename, frequency2, message2);

			queue_t queue2;

			queuePushing(frequency2, queue2);
			buildTree(queue2);

			shared_ptr<Node> root2 = queue2.top();
			string text = "";
			makeChar(root2, message2, text);
		}
		else if (ans == "3") {
			cout << "Enter filename: ";
			cin >> filename;
		}
		else if (ans == "4") {
			Flag = false;
			cout << "Goodbye." << endl;
		}
		else cout << "Incorrect input!" << endl;
	}

	return 0;
}