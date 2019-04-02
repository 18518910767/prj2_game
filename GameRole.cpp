#include "GameRole.h"
#include "GameMessage.h"
#include "msg.pb.h"

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
		/*返回talk类型的消息，内容是OK*/
		auto pxMsg = new GameMessage(GAME_MSG_TALK_CONTENT);
		auto pxGameMsg = dynamic_cast<pb::Talk *>(pxMsg->pxGameMsg);
		pxGameMsg->set_content("OK");

		Response stResp;
		stResp.pxMsg = pxMsg;
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
	return true;
}

void GameRole::fini()
{
}
