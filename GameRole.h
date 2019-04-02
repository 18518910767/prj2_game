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

	/*创建上线时的id和姓名消息*/
	GameMessage *MakeLogonSyncPid();
	/*创建广播聊天消息*/
	GameMessage *MakeTalkBroadcast(std::string _talkContent);
	/*创建广播出生位置消息*/
	GameMessage *MakeInitPosBroadcast();
	/*创建广播移动后新位置消息*/
	GameMessage *MakeNewPosBroadcast();
	/*创建下线时的id和姓名消息*/
	GameMessage *MakeLogoffSyncPid();
	/*创建周围玩家位置消息*/
	GameMessage *MakeSurPlays();
};

