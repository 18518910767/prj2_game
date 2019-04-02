#pragma once
#include <zinx/zinx.h>
#include <google/protobuf/message.h>

#define GAME_MSG_LOGON_SYNCPID 1
#define GAME_MSG_TALK_CONTENT 2
#define GAME_MSG_NEW_POSTION 3
#define GAME_MSG_BROADCAST 200
#define GAME_MSG_LOGOFF_SYNCPID 201
#define GAME_MSG_SUR_PLAYER 202

class GameMessage :
	public IdMessage
{
public:
	GameMessage(int _id);
	/*pxGameMsg存消息内容*/
	google::protobuf::Message *pxGameMsg = NULL;
	/*提供序列化和解析的函数*/
	/*消息----》数据*/
	bool serialize(unsigned char *pucBuff, int iMaxLength);
	/*数据----》消息*/
	bool parse(unsigned char *pucBuff, int iMaxLength);
	int GetLength();
	virtual ~GameMessage();
};

