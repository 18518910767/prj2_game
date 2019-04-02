#include "GameProtocol.h"
#include <string>
#include "GameMessage.h"

using namespace std;

int GameProtocol::GetNum(unsigned char * pucStart)
{
	int iRet = 0;
	iRet += pucStart[0];
	iRet += pucStart[1] * 0xff;
	iRet += pucStart[2] * 0xffff;
	iRet += pucStart[3] * 0xffffff;
	return iRet;
}

void GameProtocol::SetNum(int iNum, unsigned char * pucStart)
{
	pucStart[0] = iNum % 0xff;
	pucStart[1] = (iNum >> 8) % 0xff;
	pucStart[2] = (iNum >> 16) % 0xff;
	pucStart[3] = (iNum >> 24) % 0xff;
}

GameProtocol::GameProtocol()
{
}


GameProtocol::~GameProtocol()
{
	if (NULL != pxBindRole)
	{
		Server::GetServer()->del_role("GameRole", pxBindRole);
		delete pxBindRole;
		pxBindRole = NULL;
	}
}

bool GameProtocol::raw2request(const RawData * pstData, std::list<Request*>& _ReqList)
{
	int iPos = 0;
	bool bRet = false;

	/*1. 追加新数据到原缓冲区*/
	stBuff.AppendData(pstData);
	/*2. 若报文规定的长度大于缓冲区长度，退出*/
	int content_length;
	while ((stBuff.iLength - iPos >= 8) && (content_length = GetNum(stBuff.pucData + iPos)) + 8 <= stBuff.iLength - iPos)
	{
		/*3. 读取缓冲区中的报文并移动游标，重复步骤2*/
		int msg_id = GetNum(stBuff.pucData + iPos + 4);
		Request *pstReq = new Request();
		auto pxMsg = new GameMessage(msg_id);
		pxMsg->parse(stBuff.pucData + iPos + 8, content_length);
		pstReq->pxMsg = pxMsg;
		pstReq->pxProcessor = pxBindRole;
		_ReqList.push_back(pstReq);
		iPos += content_length + 8;
		bRet = true;
	}

	/*更新缓冲区,将当期游标的后续数据设置到缓冲区*/
	stBuff.SetData(stBuff.pucData + iPos, stBuff.iLength - iPos);

	return bRet;
}

bool GameProtocol::response2raw(const Response * pstResp, RawData * pstData)
{
	/*获取消息内容长度*/
	auto pxMsg = dynamic_cast<GameMessage *> (pstResp->pxMsg);
	int iLength = pxMsg->GetLength() + 8;
	/*组装待输出数据*/
	unsigned char *pucBuff = (unsigned char *)calloc(1UL, iLength);
	SetNum(iLength - 8, pucBuff);
	SetNum(pxMsg->Id, pucBuff + 4);
	pxMsg->serialize(pucBuff + 8, iLength - 8);

	/*设置输出参数*/
	pstData->SetData(pucBuff, iLength);
	free(pucBuff);

	return true;
}
