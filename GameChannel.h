#pragma once
#include <zinx/zinx.h>
class GameChannel :
	public TcpListenChannel
{
public:
	GameChannel();
	virtual bool TcpAfterConnection(int _iDataFd, struct sockaddr_in *pstClientAddr);
	virtual ~GameChannel();
};

