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
	/*�������������ID��Ϊ��Ա����*/
	std::string szName;
	int pid = 0;

	float x;
	float y;//��
	float z;//��
	float v;
	virtual ~GameRole();
	virtual bool init() override;
	virtual void fini() override;

	// ͨ�� AOI_Player �̳�
	virtual int GetX() override;
	virtual int GetY() override;

	GameMessage *MakeLogonSyncPid();
	GameMessage *MakeTalkBroadcast(std::string _talkContent);
	GameMessage *MakeInitPosBroadcast();
	GameMessage *MakeNewPosBroadcast();
	GameMessage *MakeLogoffSyncPid();
	GameMessage *MakeSurPlays();
};

