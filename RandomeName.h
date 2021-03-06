#pragma once
#include <string>
#include <vector>

#define RANDOM_FIRST_FILE "/random_first.txt"
#define RANDOM_LAST_FILE "/random_last.txt"

class FirstName {
public:
	std::string First;
	std::vector<std::string> m_last;
};

class RandomeName
{
public:
	/*所有姓的向量*/
	std::vector <FirstName *> m_FirstPool;
	/*基于文件构建姓名数据结构*/
	void LoadFile();
	std::string GetName();
	void ReleaseName(std::string _name);
	RandomeName();
	virtual ~RandomeName();
};

