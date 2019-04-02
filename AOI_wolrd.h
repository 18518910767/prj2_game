#pragma once

#include <list>
#include <vector>

class AOI_Player {
public:
	virtual int GetX() = 0;
	virtual int GetY() = 0;
};

class AOI_Grid {
public:
	AOI_Grid(int _gid):iGID(_gid)
	{

	}
	/*存若干个属于该格子的玩家*/
	std::list <AOI_Player *> m_players;
	int iGID = 0;
};

class AOI_wolrd
{
private:
	int Xwidth();
	int Ywidth();
public:
	/*存储若干个网格对象*/
	std::vector<AOI_Grid *> m_grids;
	AOI_wolrd(int _minx, int _maxx, int _miny, int _maxy, int _xcnt, int _ycnt);
	virtual ~AOI_wolrd();
	/*六个变量用于记录网格划分的方法*/
	int minX = 0;
	int maxX = 0;
	int minY = 0;
	int maxY = 0;
	int Xcnt = 0;
	int Ycnt = 0;
	/*获取周围玩家*/
	std::list<AOI_Player *> GetSurPlayers(AOI_Player *_player);
	/*添加和删除玩家*/
	void AddPlayer(AOI_Player *_player);
	void DelPlayer(AOI_Player *_player);
};

