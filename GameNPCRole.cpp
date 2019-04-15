#include "GameNPCRole.h"
class id500proc :public IIdMsgProc
{
public:


	// Í¨¹ý IIdMsgProc ¼Ì³Ð
	virtual bool ProcMsg(IdMsgRole * _pxRole, IdMessage * _pxMsg) override
	{
		Response stResp;
		stResp.pxMsg = dynamic_cast<GameRole *>(_pxRole)->MakeTalkBroadcast("BYE");
		auto plist = Server::GetServer()->GetRoleListByCharacter("GameRole");
		if (NULL != plist)
		{
			for (auto single : *plist)
			{
				stResp.pxSender = single;
				Server::GetServer()->send_resp(&stResp);
			}
		}
		return true;
	}

};
extern TimerChannel *pxTimerChannel;
GameNPCRole::GameNPCRole()
{
	x = 100;
	y = 2;
	z = 100;
	szName = "NPC";
	pxTimerChannel->Add_Task(this);
}


GameNPCRole::~GameNPCRole()
{
	pxTimerChannel->Del_Task(this);
}

void GameNPCRole::NotifyNewPlayer(GameRole * pxRole)
{
	Response stResp;
	stResp.pxMsg = MakeTalkBroadcast("welcome " + pxRole->szName);
	auto plist = Server::GetServer()->GetRoleListByCharacter("GameRole");
	if (NULL != plist)
	{
		for (auto pxplayer : *plist)
		{
			stResp.pxSender = pxplayer;
			Server::GetServer()->send_resp(&stResp);
		}
	}
}

int GameNPCRole::GetPeroid()
{
	return 10;
}

bool GameNPCRole::TimeOutProc()
{
	Response stResp;

	stResp.pxMsg = MakeTalkBroadcast("hello");
	auto plist = Server::GetServer()->GetRoleListByCharacter("GameRole");
	if (NULL != plist)
	{
		for (auto pxRole : *plist)
		{
			stResp.pxSender = pxRole;
			Server::GetServer()->send_resp(&stResp);
		}
	}
	return true;
}
