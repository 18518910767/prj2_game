#include <cstdio>
#include "GameChannel.h"
#include "AOI_wolrd.h"
#include <iostream>

using namespace std;

class samplePlayer : public AOI_Player {
public:
	float x;
	float y;
	int num = 0;

	// Í¨¹ý AOI_Player ¼Ì³Ð
	virtual int GetX() override
	{
		return (int)x;
	}

	virtual int GetY() override
	{
		return (int)y;
	}

};

int main()
{

	Server *pxServer = Server::GetServer();

	pxServer->init();

	pxServer->install_channel(new GameChannel());

	pxServer->run();
	/*
	AOI_world w(20, 200, 50, 230, 6, 6);

	samplePlayer p1;
	samplePlayer p2;
	samplePlayer p3;

	p1.x = 60.2;
	p1.y = 107.2;
	p1.num = 1;
	p2.x = 91.5;
	p2.y = 118.3;
	p2.num = 2;
	p3.x = 147.6;
	p3.y = 133.6;
	p3.num = 3;

	w.AddPlayer(&p1);
	w.AddPlayer(&p2);
	w.AddPlayer(&p3);

	for (auto player : w.GetSurPlayers(&p1))
	{
		auto myplayer = dynamic_cast<samplePlayer *> (player);
		cout << myplayer->num << endl;
	}

    return 0;
	*/
}