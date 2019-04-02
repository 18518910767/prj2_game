#pragma once
#include <zinx/zinx.h>
#include <string>
#include "AOI_wolrd.h"
#include "GameMessage.h"


class GameRole :
	public IdMsgRole, public AOI_Player
{
public:
	GameRole();
	/*定义玩家姓名和ID作为成员变量*/
	std::string szName;
	int pid = 0;

	float x;
	float y;//高
	float z;//宽
	float v;
	virtual ~GameRole();
	virtual bool init() override;
	virtual void fini() override;

	// 通过 AOI_Player 继承
	virtual int GetX() override;
	virtual int GetY() override;

	GameMessage *MakeLogonSyncPid();
	GameMessage *MakeTalkBroadcast(std::string _talkContent);
	GameMessage *MakeInitPosBroadcast();
	GameMessage *MakeNewPosBroadcast();
	GameMessage *MakeLogoffSyncPid();
	GameMessage *MakeSurPlays();
};

