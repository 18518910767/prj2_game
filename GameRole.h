#pragma once
#include <zinx/zinx.h>
#include <string>



class GameRole :
	public IdMsgRole
{
public:
	GameRole();
	/*�������������ID��Ϊ��Ա����*/
	std::string szName;
	int pid = 0;

	float x;
	float y;//��
	float z;//��
	float v;
	virtual ~GameRole();
	virtual bool init() override;
	virtual void fini() override;
};

