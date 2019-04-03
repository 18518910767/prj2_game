#include <cstdio>
#include "GameChannel.h"
#include "AOI_wolrd.h"
#include <iostream>
#include "RandomeName.h"

using namespace std;

class samplePlayer : public AOI_Player {
public:
	float x;
	float y;
	int num = 0;

	// 通过 AOI_Player 继承
	virtual int GetX() override
	{
		return (int)x;
	}

	virtual int GetY() override
	{
		return (int)y;
	}

};
extern RandomeName r;

void daemonlize()
{
	/*启动守护进程*/
	int pid;
	if (0 > (pid=fork()))
	{
		exit(1);
	}
	else if (0 == pid)
	{
		/*子进程*/
		setsid();
		chdir("/");
		/*重定向 0 1 2*/
		int nullfd = open("/dev/null", O_RDWR);
		dup2(nullfd, 0);
		dup2(nullfd, 1);
		dup2(nullfd, 2);
		close(nullfd);
	}
	else {
		exit(0);
	}
	/*父进程循环监控子进程*/

	while (true)
	{
		if (0 < fork())
		{
			/*检查子进程退出，若退出则重新fork*/
			int iRet;
			wait(&iRet);
		}
		else
		{
			break;
		}
		
	}

}

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		cout << "Usage:" << argv[0] << " <debug|daemon>" << endl;
		return 0;
	}

	if (0 == strcmp("daemon", argv[1]))
	{
		daemonlize();

		LOG_SetStdOut("/home/marklion/game_std_log.txt");
		LOG_SetStdErr("/home/marklion/game_err_log.txt");
	}


	r.LoadFile();

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