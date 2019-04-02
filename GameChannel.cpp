#include "GameChannel.h"
#include "GameRole.h"
#include "GameProtocol.h"

GameChannel::GameChannel():TcpListenChannel(8899)
{
}

bool GameChannel::TcpAfterConnection(int _iDataFd, sockaddr_in * pstClientAddr)
{
	/*创建gamerole对象对应的是客户端,绑定tcp通道*/
	GameRole *pxRole = new GameRole();
	/*创建协议对象绑定gamerole对象*/
	GameProtocol *pxProtocol = new GameProtocol();
	pxProtocol->pxBindRole = pxRole;
	/*创建tcp数据通道对象，绑定协议对象*/
	TcpDataChannel *pxChannel = new TcpDataChannel(_iDataFd, pxProtocol);
	pxRole->SetChannel(pxChannel);

	/*将channel和role添加到server中*/
	Server::GetServer()->install_channel(pxChannel);
	Server::GetServer()->add_role("GameRole", pxRole);


	return true;
}


GameChannel::~GameChannel()
{
}
