#include "RandomeName.h"
#include <fstream>
#include <random>
#include <algorithm>

using namespace std;

void RandomeName::LoadFile()
{
	ifstream fFirst;
	ifstream fLast;

	/*��*/
	fFirst.open(RANDOM_FIRST_FILE);
	fLast.open(RANDOM_LAST_FILE);
	if (true == fFirst.is_open() && true == fLast.is_open())
	{
		string szFirst;
		/*ѭ�����ļ�ÿһ�У�����FirstName����*/
		while (getline(fFirst, szFirst))
		{
			FirstName *pxFirst = new FirstName();
			pxFirst->First = szFirst;
			/*ѭ����ȡ���ļ�ÿһ�У�׷�ӵ�FirstName�����������*/
			string szLast;
			while (getline(fLast, szLast))
			{
				pxFirst->m_last.push_back(szLast);
			}
			m_FirstPool.push_back(pxFirst);
			/*�ָ��ļ���״̬*/
			fLast.clear(ios::goodbit);
			/*�ָ��ļ���ָ��*/
			fLast.seekg(ios::beg);
		}
		
		/*�ر�*/
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
	/*��ȡ�����*/
	int n = e() % m_FirstPool.size();
	auto pxFirst = m_FirstPool[n];
	string szFirst = pxFirst->First;
	/*��ȡ�����*/
	string szLast = pxFirst->m_last[e() % pxFirst->m_last.size()];

	/*��������ȡ�꣬��ժ���ն���*/
	if (pxFirst->m_last.size() <= 0)
	{
		m_FirstPool.erase(m_FirstPool.begin() +  n);
	}

	return szFirst + " " + szLast;
}

void RandomeName::ReleaseName(std::string _name)
{
	/*�����պ���*/
	/*�ҵ��ո�*/
	auto space_pos = _name.find(" ");
	string szFirst = _name.substr(0, space_pos);
	string szLast = _name.substr(space_pos + 1, _name.size() - space_pos);
	
	FirstName *pxFirst = NULL;
	/*׷���յ��ճ�*/
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
		/*�����ն���*/
		FirstName *pxFirst = new FirstName();
		pxFirst->First = szFirst;
	}

	/*׷�������ն����������*/
	pxFirst->m_last.push_back(szLast);
}

RandomeName::RandomeName()
{
}


RandomeName::~RandomeName()
{
}
