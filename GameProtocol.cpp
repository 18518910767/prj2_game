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

	/*1. ׷�������ݵ�ԭ������*/
	stBuff.AppendData(pstData);
	/*2. �����Ĺ涨�ĳ��ȴ��ڻ��������ȣ��˳�*/
	int content_length;
	while ((stBuff.iLength - iPos >= 8) && (content_length = GetNum(stBuff.pucData + iPos)) + 8 <= stBuff.iLength - iPos)
	{
		/*3. ��ȡ�������еı��Ĳ��ƶ��α꣬�ظ�����2*/
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

	/*���»�����,�������α�ĺ����������õ�������*/
	stBuff.SetData(stBuff.pucData + iPos, stBuff.iLength - iPos);

	return bRet;
}

bool GameProtocol::response2raw(const Response * pstResp, RawData * pstData)
{
	/*��ȡ��Ϣ���ݳ���*/
	auto pxMsg = dynamic_cast<GameMessage *> (pstResp->pxMsg);
	int iLength = pxMsg->GetLength() + 8;
	/*��װ���������*/
	unsigned char *pucBuff = (unsigned char *)calloc(1UL, iLength);
	SetNum(iLength - 8, pucBuff);
	SetNum(pxMsg->Id, pucBuff + 4);
	pxMsg->serialize(pucBuff + 8, iLength - 8);

	/*�����������*/
	pstData->SetData(pucBuff, iLength);
	free(pucBuff);

	return true;
}
