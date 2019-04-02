#include "GameMessage.h"
#include <iostream>
#include "msg.pb.h"

using namespace std;

GameMessage::GameMessage(int _id):IdMessage(_id)
{
	/*根据id不同构造不同的pxGameMsg子类对象*/
	switch (_id)
	{
	case GAME_MSG_LOGON_SYNCPID:
		pxGameMsg = new pb::SyncPid();
		break;
	case GAME_MSG_TALK_CONTENT:
		pxGameMsg = new pb::Talk();
		break;
	case GAME_MSG_NEW_POSTION:
		pxGameMsg = new pb::Position();
		break;
	case GAME_MSG_BROADCAST:
		pxGameMsg = new pb::BroadCast();
		break;
	case GAME_MSG_LOGOFF_SYNCPID:
		pxGameMsg = new pb::SyncPid();
		break;
	case GAME_MSG_SUR_PLAYER:
		pxGameMsg = new pb::SyncPlayers();
		break;

	default:
		break;
	}
}


bool GameMessage::serialize(unsigned char * pucBuff, int iMaxLength)
{
	/*序列化之前打印调试信息*/
	cout << pxGameMsg->Utf8DebugString() << endl;
	return pxGameMsg->SerializeToArray(pucBuff, iMaxLength);
}

bool GameMessage::parse(unsigned char * pucBuff, int iMaxLength)
{
	pxGameMsg->ParseFromArray(pucBuff, iMaxLength);
	cout << pxGameMsg->Utf8DebugString() << endl;
	return true;
}

int GameMessage::GetLength()
{
	return pxGameMsg->ByteSizeLong();
}

GameMessage::~GameMessage()
{
}
