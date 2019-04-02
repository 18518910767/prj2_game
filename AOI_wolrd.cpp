#include "AOI_wolrd.h"

using namespace std;

int AOI_wolrd::Xwidth()
{
	return (maxX - minX) / Xcnt;
}

int AOI_wolrd::Ywidth()
{
	return (maxY - minY) / Ycnt;
}

AOI_wolrd::AOI_wolrd(int _minx, int _maxx, int _miny, int _maxy, int _xcnt, int _ycnt):minX(_minx), maxX(_maxx), minY(_miny), maxY(_maxy), Xcnt(_xcnt), Ycnt(_ycnt)
{
	/*添加若干网格到向量*/
	int n = Xcnt * Ycnt;
	for (int i = 0; i < n; i++)
	{
		m_grids.push_back(new AOI_Grid(i));
	}
}

AOI_wolrd::~AOI_wolrd()
{
	for (auto grid : m_grids)
	{
		delete grid;
	}
}

std::list<AOI_Player*> AOI_wolrd::GetSurPlayers(AOI_Player * _player)
{
	/*计算玩家所属的网格*/
	int x = _player->GetX();
	int y = _player->GetY();
	int n = (x - minX) / Xwidth() + (y - minY) / Ywidth()*Xcnt;

	int xNum = n % Xcnt;
	int yNum = n / Xcnt;

	list <AOI_Player *> lRet;

	if (xNum > 0 && yNum > 0)
	{
		/*将左上网格内所有玩家存到lRet里*/
		lRet.insert(lRet.begin(),m_grids[n - 1 - Xcnt]->m_players.begin(), m_grids[n - 1 - Xcnt]->m_players.end());
	}

	if (yNum > 0)
	{
		lRet.insert(lRet.begin(), m_grids[n - Xcnt]->m_players.begin(), m_grids[n - Xcnt]->m_players.end());
	}

	if (xNum < Xcnt - 1 && yNum > 0)
	{
		lRet.insert(lRet.begin(), m_grids[n + 1- Xcnt]->m_players.begin(), m_grids[n +1 - Xcnt]->m_players.end());
	}

	if (xNum > 0)
	{
		lRet.insert(lRet.begin(), m_grids[n - 1]->m_players.begin(), m_grids[n - 1]->m_players.end());
	}

	lRet.insert(lRet.begin(), m_grids[n]->m_players.begin(), m_grids[n]->m_players.end());

	if (xNum < Xcnt - 1)
	{
		lRet.insert(lRet.begin(), m_grids[n + 1]->m_players.begin(), m_grids[n + 1]->m_players.end());
	}
	if (xNum > 0 && yNum < Ycnt - 1)
	{
		lRet.insert(lRet.begin(), m_grids[n - 1 + Xcnt]->m_players.begin(), m_grids[n -1 + Xcnt]->m_players.end());
	}

	if (yNum < Ycnt - 1)
	{
		lRet.insert(lRet.begin(), m_grids[n + Xcnt]->m_players.begin(), m_grids[n + Xcnt]->m_players.end());
	}

	if (xNum < Xcnt - 1 && yNum < Ycnt - 1)
	{
		lRet.insert(lRet.begin(), m_grids[n +1 + Xcnt]->m_players.begin(), m_grids[n +1 + Xcnt]->m_players.end());
	}

	return lRet;
}

void AOI_wolrd::AddPlayer(AOI_Player * _player)
{
	/*计算玩家所属的网格*/
	int x = _player->GetX();
	int y = _player->GetY();
	int n = (x - minX) / Xwidth() + (y - minY) / Ywidth()*Xcnt;
	/*把玩家放到格子里*/
	m_grids[n]->m_players.push_back(_player);
}

void AOI_wolrd::DelPlayer(AOI_Player * _player)
{
	/*计算玩家所属的网格*/
	int x = _player->GetX();
	int y = _player->GetY();
	int n = (x - minX) / Xwidth() + (y - minY) / Ywidth()*Xcnt;

	m_grids[n]->m_players.remove(_player);
}
