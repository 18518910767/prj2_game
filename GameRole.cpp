#include "GameRole.h"
#include "GameMessage.h"
#include "msg.pb.h"
#include "AOI_wolrd.h"
#include <algorithm>
#include <random>
#include "RandomeName.h"

using namespace std;

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
	/*�����յ��������ݺ󱻵���*/
	virtual bool ProcMsg(IdMsgRole * _pxRole, IdMessage * _pxMsg) override
	{
		auto pxInMsg = dynamic_cast<GameMessage *>(_pxMsg);
		auto talkContent = dynamic_cast<pb::Talk *> (pxInMsg->pxGameMsg);
		auto pxGameRole = dynamic_cast<GameRole *>(_pxRole);

		Response stResp;
		stResp.pxMsg = pxGameRole->MakeTalkBroadcast(talkContent->content());
		/*��ȡ�������*/
		auto plist = Server::GetServer()->GetRoleListByCharacter("GameRole");
		for (auto player : *plist)
		{
			/*����������Ϣ*/
			stResp.pxSender = dynamic_cast<GameRole *>(player);
			Server::GetServer()->send_resp(&stResp);
		}

		return true;
	}
};

class id3Proc :public IIdMsgProc{
public:


	// ͨ�� IIdMsgProc �̳�
	/*�����յ�����ƶ�����Ϣ�󱻵���*/
	virtual bool ProcMsg(IdMsgRole * _pxRole, IdMessage * _pxMsg) override
	{
		/*�ȴ���Ϣ�ж�ȡ������*/
		auto pxInMsg = dynamic_cast<GameMessage *>(_pxMsg);
		auto pxPos = dynamic_cast<pb::Position *>(pxInMsg->pxGameMsg);
		int newX = (int)pxPos->x();
		int newY = (int)pxPos->z();
		auto pxGameRole = dynamic_cast<GameRole *>(_pxRole);
		/*�ж��Ƿ��л�����*/
		if (true == AOI_world::GetWorld()->GridChanged(pxGameRole, newX, newY))
		{
			/*��ȡ֮ǰ���ھ�*/
			auto oldsurlist = AOI_world::GetWorld()->GetSurPlayers(pxGameRole);
			AOI_world::GetWorld()->DelPlayer(pxGameRole);
			/*��ȡ���ڵ��ھ�*/
			pxGameRole->x = pxPos->x();
			pxGameRole->y = pxPos->y();
			pxGameRole->z = pxPos->z();
			pxGameRole->v = pxPos->v();
			AOI_world::GetWorld()->AddPlayer(pxGameRole);
			auto newsurlist = AOI_world::GetWorld()->GetSurPlayers(pxGameRole);
			/*������Ұ��ʧ����Ұ����*/
			pxGameRole->ViewAppear(newsurlist, oldsurlist);
			pxGameRole->ViewLost(newsurlist, oldsurlist);
		}

		/*����Χ��ҷ�����λ��*/
		/*��������*/
		pxGameRole->x = pxPos->x();
		pxGameRole->y = pxPos->y();
		pxGameRole->z = pxPos->z();
		pxGameRole->v = pxPos->v();

		Response stResp;
		stResp.pxMsg = pxGameRole->MakeNewPosBroadcast();
		for (auto surplayer : AOI_world::GetWorld()->GetSurPlayers(pxGameRole))
		{
			stResp.pxSender = dynamic_cast<GameRole *>(surplayer);
			Server::GetServer()->send_resp(&stResp);
		}

		return true;;
	}

};

int g_iPidStart = 0;
default_random_engine e(time(NULL));
RandomeName r;

GameRole::GameRole()
{
	/*�����������������*/
	x = 100 + e() % 20;
	y = 0;
	z = 100 + e() % 20;
	v = 0;
	szName = r.GetName();
	/*�������id*/
	pid = g_iPidStart++;
}


GameRole::~GameRole()
{
	r.ReleaseName(szName);
}

bool GameRole::init()
{
	register_id_func(GAME_MSG_TALK_CONTENT, new id2proc());
	register_id_func(GAME_MSG_NEW_POSTION, new id3Proc());
	
	/*���Լ���ӵ���Ϸ����*/
	AOI_world::GetWorld()->AddPlayer(this);

	/*����id���ǳƸ��ͻ���*/
	Response stResp1;
	stResp1.pxMsg = MakeLogonSyncPid();
	stResp1.pxSender = this;
	Server::GetServer()->send_resp(&stResp1);
	/*������Χ��Ҹ��ͻ���*/
	Response stResp2;
	stResp2.pxMsg = MakeSurPlays();
	stResp2.pxSender = this;
	Server::GetServer()->send_resp(&stResp2);
	/*�����Լ���λ�ø���Χ��ҵĿͻ���*/
	Response stResp3;
	stResp3.pxMsg = MakeInitPosBroadcast();
	for (auto pxplayer : AOI_world::GetWorld()->GetSurPlayers(this))
	{
		stResp3.pxSender = dynamic_cast<GameRole *> (pxplayer);
		Server::GetServer()->send_resp(&stResp3);
	}

	return true;
}

void GameRole::fini()
{
	/*����Χ��ҷ���������Ϣ*/
	Response stResp;
	stResp.pxMsg = MakeLogoffSyncPid();
	for (auto pxplayer : AOI_world::GetWorld()->GetSurPlayers(this))
	{
		stResp.pxSender = dynamic_cast<GameRole *> (pxplayer);
		Server::GetServer()->send_resp(&stResp);
	}
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

void GameRole::ViewLost(std::list<AOI_Player*>& newsurlist, std::list<AOI_Player*>& oldsurlist)
{
	Response stResp2Other;
	stResp2Other.pxMsg = MakeLogoffSyncPid();

	/*������Χ����в��Ҿ���Χ��ң��Ҳ������Ǿ��ھ�*/
	for (auto oldplayer : oldsurlist)
	{
		if (newsurlist.end() == find(newsurlist.begin(), newsurlist.end(), oldplayer))
		{
			/*����ھӷ���logoffsyncid*/
			stResp2Other.pxSender = dynamic_cast<GameRole *> (oldplayer);
			Server::GetServer()->send_resp(&stResp2Other);
			/*���Լ������ھ�logoffsyncid��Ϣ*/
			Response stResp2self;
			stResp2self.pxMsg = dynamic_cast<GameRole *> (oldplayer)->MakeLogoffSyncPid();
			stResp2self.pxSender = this;
			Server::GetServer()->send_resp(&stResp2self);
		}
	}
}

void GameRole::ViewAppear(std::list<AOI_Player*>& newsurlist, std::list<AOI_Player*>& oldsurlist)
{
	Response stResp2Other;
	stResp2Other.pxMsg = MakeInitPosBroadcast();

	for (auto newplayer : newsurlist)
	{
		if (oldsurlist.end() == find(oldsurlist.begin(), oldsurlist.end(), newplayer))
		{
			/*�����ھӷ���broadcastlogon��Ϣ*/
			stResp2Other.pxSender = dynamic_cast<GameRole *> (newplayer);
			Server::GetServer()->send_resp(&stResp2Other);
			/*���Լ��������ھӵ�broadcastlogon��Ϣ*/
			Response stResp2self;
			stResp2self.pxMsg = dynamic_cast<GameRole *> (newplayer)->MakeInitPosBroadcast();
			stResp2self.pxSender = this;
			Server::GetServer()->send_resp(&stResp2self);
		}
	}
}
