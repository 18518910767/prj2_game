#pragma once
#include <string>
#include <vector>

class FirstName {
public:
	std::string First;
	std::vector<std::string> m_last;
};

class RandomeName
{
public:
	/*�����յ�����*/
	std::vector <FirstName *> m_FirstPool;
	/*�����ļ������������ݽṹ*/
	void LoadFile();
	std::string GetName();
	void ReleaseName(std::string _name);
	RandomeName();
	virtual ~RandomeName();
};

