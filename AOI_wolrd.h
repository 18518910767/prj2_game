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
	/*�����ɸ����ڸø��ӵ����*/
	std::list <AOI_Player *> m_players;
	int iGID = 0;
};

class AOI_wolrd
{
private:
	int Xwidth();
	int Ywidth();
public:
	/*�洢���ɸ��������*/
	std::vector<AOI_Grid *> m_grids;
	AOI_wolrd(int _minx, int _maxx, int _miny, int _maxy, int _xcnt, int _ycnt);
	virtual ~AOI_wolrd();
	/*�����������ڼ�¼���񻮷ֵķ���*/
	int minX = 0;
	int maxX = 0;
	int minY = 0;
	int maxY = 0;
	int Xcnt = 0;
	int Ycnt = 0;
	/*��ȡ��Χ���*/
	std::list<AOI_Player *> GetSurPlayers(AOI_Player *_player);
	/*��Ӻ�ɾ�����*/
	void AddPlayer(AOI_Player *_player);
	void DelPlayer(AOI_Player *_player);
};

