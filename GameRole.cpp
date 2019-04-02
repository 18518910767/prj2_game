#include "GameRole.h"
#include "GameMessage.h"
#include "msg.pb.h"

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
		/*����talk���͵���Ϣ��������OK*/
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
