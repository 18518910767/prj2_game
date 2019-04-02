#include "GameChannel.h"
#include "GameRole.h"
#include "GameProtocol.h"

GameChannel::GameChannel():TcpListenChannel(8899)
{
}

bool GameChannel::TcpAfterConnection(int _iDataFd, sockaddr_in * pstClientAddr)
{
	/*����gamerole�����Ӧ���ǿͻ���,��tcpͨ��*/
	GameRole *pxRole = new GameRole();
	/*����Э������gamerole����*/
	GameProtocol *pxProtocol = new GameProtocol();
	pxProtocol->pxBindRole = pxRole;
	/*����tcp����ͨ�����󣬰�Э�����*/
	TcpDataChannel *pxChannel = new TcpDataChannel(_iDataFd, pxProtocol);
	pxRole->SetChannel(pxChannel);

	/*��channel��role��ӵ�server��*/
	Server::GetServer()->install_channel(pxChannel);
	Server::GetServer()->add_role("GameRole", pxRole);


	return true;
}


GameChannel::~GameChannel()
{
}
