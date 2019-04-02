#include "GameRole.h"
#include "GameMessage.h"
#include "msg.pb.h"
#include "AOI_wolrd.h"

/*定义id为0的处理类*/
class id0Proc :public IIdMsgProc {
public:


	// 通过 IIdMsgProc 继承
	virtual bool ProcMsg(IdMsgRole * _pxRole, IdMessage * _pxMsg) override
	{
		Response stResp;
		stResp.pxMsg = new GameMessage(10);
		stResp.pxSender = _pxRole;

		return Server::GetServer()->send_resp(&stResp);
	}

};

class id2proc :public IIdMsgProc {
public:


	// 通过 IIdMsgProc 继承
	virtual bool ProcMsg(IdMsgRole * _pxRole, IdMessage * _pxMsg) override
	{
		Response stResp;
		auto pxGameRole = dynamic_cast<GameRole *>(_pxRole);
		stResp.pxMsg = pxGameRole->MakeTalkBroadcast("呵呵呵");
		stResp.pxSender = _pxRole;

		return Server::GetServer()->send_resp(&stResp);
	}

};

GameRole::GameRole()
{
}


GameRole::~GameRole()
{
}

bool GameRole::init()
{
	register_id_func(0, new id0Proc());
	register_id_func(1, new id0Proc());
	register_id_func(GAME_MSG_TALK_CONTENT, new id2proc());
	
	/*将自己添加到游戏世界*/
	AOI_world::GetWorld()->AddPlayer(this);

	return true;
}

void GameRole::fini()
{
	/*把自己摘出游戏世界*/
	AOI_world::GetWorld()->DelPlayer(this);
}

int GameRole::GetX()
{
	return (int)x;
}

int GameRole::GetY()
{
	return (int)z;
}

GameMessage * GameRole::MakeLogonSyncPid()
{
	GameMessage *pxRet = new GameMessage(GAME_MSG_LOGON_SYNCPID);
	auto pxSyncpid = dynamic_cast<pb::SyncPid *> (pxRet->pxGameMsg);

	pxSyncpid->set_pid(pid);
	pxSyncpid->set_username(szName);

	return pxRet;
}

GameMessage * GameRole::MakeTalkBroadcast(std::string _talkContent)
{
	GameMessage *pxRet = new GameMessage(GAME_MSG_BROADCAST);
	auto pxTalkBroadcast = dynamic_cast<pb::BroadCast *>(pxRet->pxGameMsg);

	pxTalkBroadcast->set_pid(pid);
	pxTalkBroadcast->set_username(szName);
	pxTalkBroadcast->set_tp(1);
	pxTalkBroadcast->set_content(_talkContent);

	return pxRet;
}

GameMessage * GameRole::MakeInitPosBroadcast()
{
	GameMessage *pxRet = new GameMessage(GAME_MSG_BROADCAST);
	auto pxInitPosBroadcast = dynamic_cast<pb::BroadCast *>(pxRet->pxGameMsg);

	pxInitPosBroadcast->set_pid(pid);
	pxInitPosBroadcast->set_username(szName);
	pxInitPosBroadcast->set_tp(2);

	/*获取子消息指针并挂到父消息里*/
	auto pxP = pxInitPosBroadcast->mutable_p();
	/*添加数据到子消息*/
	pxP->set_x(x);
	pxP->set_y(y);
	pxP->set_z(z);
	pxP->set_v(v);

	return pxRet;
}

GameMessage * GameRole::MakeNewPosBroadcast()
{
	GameMessage *pxRet = new GameMessage(GAME_MSG_BROADCAST);
	auto pxNewPosBroadcast = dynamic_cast<pb::BroadCast *>(pxRet->pxGameMsg);

	pxNewPosBroadcast->set_pid(pid);
	pxNewPosBroadcast->set_username(szName);
	pxNewPosBroadcast->set_tp(4);

	/*获取子消息指针并挂到父消息里*/
	auto pxP = pxNewPosBroadcast->mutable_p();
	/*添加数据到子消息*/
	pxP->set_x(x);
	pxP->set_y(y);
	pxP->set_z(z);
	pxP->set_v(v);

	return pxRet;
}

GameMessage * GameRole::MakeLogoffSyncPid()
{
	GameMessage *pxRet = new GameMessage(GAME_MSG_LOGOFF_SYNCPID);
	auto pxSyncpid = dynamic_cast<pb::SyncPid *> (pxRet->pxGameMsg);

	pxSyncpid->set_pid(pid);
	pxSyncpid->set_username(szName);

	return pxRet;
}

GameMessage * GameRole::MakeSurPlays()
{
	GameMessage *pxRet = new GameMessage(GAME_MSG_SUR_PLAYER);
	auto pxSurplayer = dynamic_cast<pb::SyncPlayers *>(pxRet->pxGameMsg);

	/*获取自己周围玩家*/
	auto surlist = AOI_world::GetWorld()->GetSurPlayers(this);
	/*遍历list组织子消息添加到pxSurplayer里*/
	for (auto player : surlist)
	{
		auto pxSinglePlayer = pxSurplayer->add_ps();
		auto pxRole = dynamic_cast<GameRole *>(player);
		pxSinglePlayer->set_pid(pxRole->pid);
		pxSinglePlayer->set_username(pxRole->szName);
		auto subPos = pxSinglePlayer->mutable_p();
		subPos->set_x(pxRole->x);
		subPos->set_y(pxRole->y);
		subPos->set_z(pxRole->z);
		subPos->set_v(pxRole->v);
	}

	return pxRet;
}
