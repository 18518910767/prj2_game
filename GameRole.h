#pragma once
#include <zinx/zinx.h>
#include <string>



class GameRole :
	public IdMsgRole
{
public:
	GameRole();
	/*定义玩家姓名和ID作为成员变量*/
	std::string szName;
	int pid = 0;

	float x;
	float y;//高
	float z;//宽
	float v;
	virtual ~GameRole();
	virtual bool init() override;
	virtual void fini() override;
};

