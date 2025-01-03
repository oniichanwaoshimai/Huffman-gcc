#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
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
			++frequency[(unsigned char)ch]; // ��� ������ �������
		}
	}
}

struct Node { // ����
	unsigned char ch{ 0 };
	int frequency{ 0 };
};

int main() {
	system("chcp 1251");
	system("cls");
	int frequency[ascii];
	readFile("test.txt", frequency);

	//for (int i = 0; i < ascii; ++i) { // ��������: ����� ������� ���� �������� ascii
	//	if (frequency[i] > 0) {
	//		cout << frequency[i] << " " << (char)i << endl;
	//	}
	//}

	priority_queue<Node, > queue;


//
//	vector<Node> tree; // ���� ������ (��������� �����)
//	map<char, int> charMap;
//	for (size_t i = 0; i < ascii; ++i)
//	{
//		if (weight[i] > 0)
//		{
//			tree.push_back(Node{ (char)i, -1, -1, -1, false }); // � ������ ������ ���� (����� ������� (�������))
//			charMap[i] = tree.size() - 1;
//			sortedWeight.insert(make_pair(weight[i], tree.size() - 1)); // tree.size-1 - ������ ������ ������. ����� ������ ������, ��� ��������� � ����� ������
//		}
//	}
//	while (sortedWeight.size() > 1) // ������� ��������� ����� (������ ��������)  
//	{
//		int w0 = begin(sortedWeight)->first;
//		int n0 = begin(sortedWeight)->second;
//		sortedWeight.erase(begin(sortedWeight));
//		int w1 = begin(sortedWeight)->first;
//		int n1 = begin(sortedWeight)->second;
//		sortedWeight.erase(begin(sortedWeight));
//		tree.push_back(Node{ '\0', -1, n0, n1, false });
//		tree[n0].parent = tree.size() - 1;
//		tree[n0].branch = false;
//		tree[n1].parent = tree.size() - 1;
//		tree[n1].branch = true;
//		sortedWeight.insert(make_pair(w0 + w1, tree.size() - 1));
//	}
//
//	vector<bool> data;
//
//	{ // ��� ������ - ������� ������ ����������� �����, ����� �� �������
//		ifstream file("text.txt");
//		char ch;
//		while (file.get(ch)) // ��������� 1 ������ �� �����
//		{
//			auto n = tree[charMap[ch]];
//			vector<bool> compressedChar;
//			while (n.parent != -1)
//			{
//				compressedChar.push_back(n.branch);
//				n = tree[n.parent];
//			}
//			data.insert(end(data), compressedChar.rbegin(), compressedChar.rend()); // ���������� � �������� �������, ������ ��� �� �������. (���������� � ������������ � ������ ��������)
//		}
//		/*file.close();*/
//	}
//	ofstream file("text.huff");
//	int treeSize = tree.size();
//	file.write((char*)&treeSize, sizeof(treeSize));
//	for (auto i : tree)
//		file.write((char*)&i, sizeof(i));
//
//	for (size_t i = 0; i <= data.size() / 8; ++i) // �� ���� ���� i <= data.size() / 8 ����� �� ���� ������ ��������, ������ ������ ������ (g++ �� ������)
//	{
//		unsigned char ch = 0;
//		for (int j = 0; j < 8; ++j)
//			if (data[i * 8 + j])
//				ch |= (1 << j);
//		file.write((char*)&ch, sizeof(ch));
//	}
}