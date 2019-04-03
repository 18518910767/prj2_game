#include "GameRole.h"
#include "GameMessage.h"
#include "msg.pb.h"
#include "AOI_wolrd.h"
#include <algorithm>
#include <random>
#include "RandomeName.h"

using namespace std;

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
	/*会在收到聊天内容后被调用*/
	virtual bool ProcMsg(IdMsgRole * _pxRole, IdMessage * _pxMsg) override
	{
		auto pxInMsg = dynamic_cast<GameMessage *>(_pxMsg);
		auto talkContent = dynamic_cast<pb::Talk *> (pxInMsg->pxGameMsg);
		auto pxGameRole = dynamic_cast<GameRole *>(_pxRole);

		Response stResp;
		stResp.pxMsg = pxGameRole->MakeTalkBroadcast(talkContent->content());
		/*获取所有玩家*/
		auto plist = Server::GetServer()->GetRoleListByCharacter("GameRole");
		for (auto player : *plist)
		{
			/*发送聊天信息*/
			stResp.pxSender = dynamic_cast<GameRole *>(player);
			Server::GetServer()->send_resp(&stResp);
		}

		return true;
	}
};

class id3Proc :public IIdMsgProc{
public:


	// 通过 IIdMsgProc 继承
	/*会在收到玩家移动的消息后被调用*/
	virtual bool ProcMsg(IdMsgRole * _pxRole, IdMessage * _pxMsg) override
	{
		/*先从消息中读取新坐标*/
		auto pxInMsg = dynamic_cast<GameMessage *>(_pxMsg);
		auto pxPos = dynamic_cast<pb::Position *>(pxInMsg->pxGameMsg);
		int newX = (int)pxPos->x();
		int newY = (int)pxPos->z();
		auto pxGameRole = dynamic_cast<GameRole *>(_pxRole);
		/*判断是否切换网格*/
		if (true == AOI_world::GetWorld()->GridChanged(pxGameRole, newX, newY))
		{
			/*获取之前的邻居*/
			auto oldsurlist = AOI_world::GetWorld()->GetSurPlayers(pxGameRole);
			AOI_world::GetWorld()->DelPlayer(pxGameRole);
			/*获取现在的邻居*/
			pxGameRole->x = pxPos->x();
			pxGameRole->y = pxPos->y();
			pxGameRole->z = pxPos->z();
			pxGameRole->v = pxPos->v();
			AOI_world::GetWorld()->AddPlayer(pxGameRole);
			auto newsurlist = AOI_world::GetWorld()->GetSurPlayers(pxGameRole);
			/*处理视野丢失和视野出现*/
			pxGameRole->ViewAppear(newsurlist, oldsurlist);
			pxGameRole->ViewLost(newsurlist, oldsurlist);
		}

		/*向周围玩家发送新位置*/
		/*更新坐标*/
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
	/*定义该玩家坐标和名称*/
	x = 100 + e() % 20;
	y = 0;
	z = 100 + e() % 20;
	v = 0;
	szName = r.GetName();
	/*定义玩家id*/
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
	
	/*将自己添加到游戏世界*/
	AOI_world::GetWorld()->AddPlayer(this);

	/*发送id和昵称给客户端*/
	Response stResp1;
	stResp1.pxMsg = MakeLogonSyncPid();
	stResp1.pxSender = this;
	Server::GetServer()->send_resp(&stResp1);
	/*发送周围玩家给客户端*/
	Response stResp2;
	stResp2.pxMsg = MakeSurPlays();
	stResp2.pxSender = this;
	Server::GetServer()->send_resp(&stResp2);
	/*发送自己的位置给周围玩家的客户端*/
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
	/*向周围玩家发送下线消息*/
	Response stResp;
	stResp.pxMsg = MakeLogoffSyncPid();
	for (auto pxplayer : AOI_world::GetWorld()->GetSurPlayers(this))
	{
		stResp.pxSender = dynamic_cast<GameRole *> (pxplayer);
		Server::GetServer()->send_resp(&stResp);
	}
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

void GameRole::ViewLost(std::list<AOI_Player*>& newsurlist, std::list<AOI_Player*>& oldsurlist)
{
	Response stResp2Other;
	stResp2Other.pxMsg = MakeLogoffSyncPid();

	/*在新周围玩家中查找旧周围玩家，找不到就是旧邻居*/
	for (auto oldplayer : oldsurlist)
	{
		if (newsurlist.end() == find(newsurlist.begin(), newsurlist.end(), oldplayer))
		{
			/*向旧邻居发送logoffsyncid*/
			stResp2Other.pxSender = dynamic_cast<GameRole *> (oldplayer);
			Server::GetServer()->send_resp(&stResp2Other);
			/*向自己发送邻居logoffsyncid消息*/
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
			/*向新邻居发送broadcastlogon消息*/
			stResp2Other.pxSender = dynamic_cast<GameRole *> (newplayer);
			Server::GetServer()->send_resp(&stResp2Other);
			/*向自己发送新邻居的broadcastlogon消息*/
			Response stResp2self;
			stResp2self.pxMsg = dynamic_cast<GameRole *> (newplayer)->MakeInitPosBroadcast();
			stResp2self.pxSender = this;
			Server::GetServer()->send_resp(&stResp2self);
		}
	}
}
