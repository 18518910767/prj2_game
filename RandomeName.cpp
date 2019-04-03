#include "RandomeName.h"
#include <fstream>
#include <random>
#include <algorithm>

using namespace std;

void RandomeName::LoadFile()
{
	ifstream fFirst;
	ifstream fLast;

	/*打开*/
	fFirst.open(RANDOM_FIRST_FILE);
	fLast.open(RANDOM_LAST_FILE);
	if (true == fFirst.is_open() && true == fLast.is_open())
	{
		string szFirst;
		/*循环姓文件每一行，生成FirstName对象*/
		while (getline(fFirst, szFirst))
		{
			FirstName *pxFirst = new FirstName();
			pxFirst->First = szFirst;
			/*循环读取名文件每一行，追加到FirstName对象的向量中*/
			string szLast;
			while (getline(fLast, szLast))
			{
				pxFirst->m_last.push_back(szLast);
			}
			m_FirstPool.push_back(pxFirst);
			/*恢复文件流状态*/
			fLast.clear(ios::goodbit);
			/*恢复文件流指针*/
			fLast.seekg(ios::beg);
		}
		
		/*关闭*/
		fFirst.close();
		fLast.close();
	}
	


}

extern default_random_engine e;

std::string RandomeName::GetName()
{
	if (m_FirstPool.size() <= 0)
	{
		return "Not Support";
	}
	/*获取随机姓*/
	int n = e() % m_FirstPool.size();
	auto pxFirst = m_FirstPool[n];
	string szFirst = pxFirst->First;
	/*获取随机名*/
	string szLast = pxFirst->m_last[e() % pxFirst->m_last.size()];

	/*若名向量取完，则摘掉姓对象*/
	if (pxFirst->m_last.size() <= 0)
	{
		m_FirstPool.erase(m_FirstPool.begin() +  n);
	}

	return szFirst + " " + szLast;
}

void RandomeName::ReleaseName(std::string _name)
{
	/*分离姓和名*/
	/*找到空格*/
	auto space_pos = _name.find(" ");
	string szFirst = _name.substr(0, space_pos);
	string szLast = _name.substr(space_pos + 1, _name.size() - space_pos);
	
	FirstName *pxFirst = NULL;
	/*追加姓到姓池*/
	for (auto first : m_FirstPool)
	{
		if (szFirst == first->First)
		{
			pxFirst = first;
			break;
		}
	}

	if (NULL == pxFirst)
	{
		/*创建姓对象*/
		FirstName *pxFirst = new FirstName();
		pxFirst->First = szFirst;
	}

	/*追加名到姓对象的名向量*/
	pxFirst->m_last.push_back(szLast);
}

RandomeName::RandomeName()
{
}


RandomeName::~RandomeName()
{
}
