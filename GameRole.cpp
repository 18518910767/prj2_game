#include "GameRole.h"
#include "GameMessage.h"
#include "msg.pb.h"
#include "AOI_wolrd.h"

/*����idΪ0�Ĵ�����*/
class id0Proc :public IIdMsgProc {
public:


	// ͨ�� IIdMsgProc �̳�
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


	// ͨ�� IIdMsgProc �̳�
	virtual bool ProcMsg(IdMsgRole * _pxRole, IdMessage * _pxMsg) override
	{
		Response stResp;
		auto pxGameRole = dynamic_cast<GameRole *>(_pxRole);
		stResp.pxMsg = pxGameRole->MakeTalkBroadcast("�ǺǺ�");
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
	
	/*���Լ���ӵ���Ϸ����*/
	AOI_world::GetWorld()->AddPlayer(this);

	return true;
}

void GameRole::fini()
{
	/*���Լ�ժ����Ϸ����*/
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

	/*��ȡ����Ϣָ�벢�ҵ�����Ϣ��*/
	auto pxP = pxInitPosBroadcast->mutable_p();
	/*������ݵ�����Ϣ*/
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

	/*��ȡ����Ϣָ�벢�ҵ�����Ϣ��*/
	auto pxP = pxNewPosBroadcast->mutable_p();
	/*������ݵ�����Ϣ*/
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

	/*��ȡ�Լ���Χ���*/
	auto surlist = AOI_world::GetWorld()->GetSurPlayers(this);
	/*����list��֯����Ϣ��ӵ�pxSurplayer��*/
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
