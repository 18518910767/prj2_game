#pragma once
#include <zinx/zinx.h>
#include "GameRole.h"

class GameProtocol :
	public Aprotocol
{
	/*��һ�ε����ݻ�����*/
	RawData stBuff;
	int GetNum(unsigned char *pucStart);
	void SetNum(int iNum, unsigned char *pucStart);
public:
	GameProtocol();
	virtual ~GameProtocol();
	GameRole *pxBindRole = NULL;
	// ͨ�� Aprotocol �̳�
	virtual bool raw2request(const RawData * pstData, std::list<Request*>& _ReqList) override;
	virtual bool response2raw(const Response * pstResp, RawData * pstData) override;
};

