#include "TcpChannel.h"
#include "server.h"
#include <string>

TcpListenChannel::TcpListenChannel(unsigned short usPort):m_usPort(usPort)
{
}

TcpListenChannel::~TcpListenChannel()
{
}

bool TcpListenChannel::TcpAfterConnection(int _iDataFd, struct sockaddr_in *pstClientAddr)
{
    std::cout<<"client IP is "<<std::string(inet_ntoa(pstClientAddr->sin_addr))<<" Port is "<<ntohs(pstClientAddr->sin_port)<<std::endl;
    close(_iDataFd);

    return true;
}

bool TcpListenChannel::readFd(uint32_t _event, RawData * pstData)
{
	/*客户端连接后*/
	bool bRet = false;
	/*accept，得到数据套接字*/
	if (0 != (_event & EPOLLIN))
	{
		sockaddr_in stClientAddr;
		socklen_t iclientaddr_length = sizeof(stClientAddr);
		int idata_socket = accept(m_fd, (sockaddr *)(&stClientAddr), &iclientaddr_length);
		if (0 <= idata_socket)
		{
			/*调用TcpAfterConnection，实现连接到来之后的操作*/
			bRet = TcpAfterConnection(idata_socket, &stClientAddr);
		}
		else
		{
			perror("__FUNC__:accept:");
		}
	}
		
	return bRet;
}

bool TcpListenChannel::writeFd(const RawData * pstData)
{
	return false;
}

bool TcpListenChannel::init()
{
	bool bRet = false;
    /*创建socket*/
	int isocket = socket(AF_INET, SOCK_STREAM, 0);
	if (0 <= isocket)
	{
		/*绑定*/
		sockaddr_in stServerAddr;
		stServerAddr.sin_family = AF_INET;
		stServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		stServerAddr.sin_port = htons( m_usPort);

		/*为socket添加重复绑定属性*/
		int socketopt = 1;
		setsockopt(isocket, SOL_SOCKET, SO_REUSEADDR, &socketopt, sizeof(socketopt));

		if (0 == bind(isocket, (sockaddr *)(&stServerAddr), sizeof(stServerAddr)))
		{
			/*转换监听套接字*/
			if (0 == listen(isocket, 10))
			{
				bRet = true;
				m_fd = isocket;
			}
			else
			{
				perror("__FUNC__:listen:");
			}
		}
		else
		{
			perror("__FUNC__:bind:");
		}
	}
	else
	{
		perror("__FUNC__:socket:");
	}


	return bRet;
}

void TcpListenChannel::fini()
{
    if (0 <= m_fd)
    {
        close(m_fd);
        m_fd = -1;
    }
}

TcpDataChannel::TcpDataChannel(int _iDataFd, Aprotocol * _protocol):Achannel(_protocol)
{
    m_fd = _iDataFd;
    
    std::cout<<"++++++++++++++++++++++++++++++"<<std::endl;
    std::cout << "new tcp connection, fd = "<<m_fd<<std::endl;
    std::cout<<"++++++++++++++++++++++++++++++"<<std::endl;
}
TcpDataChannel::~TcpDataChannel()
{
}

bool TcpDataChannel::init()
{
    m_event = EPOLLIN|EPOLLHUP|EPOLLERR;
    return true;
}
void TcpDataChannel::fini()
{
    close(m_fd);
    delete m_pxProtocol;
    m_pxProtocol = NULL;
}

bool TcpDataChannel::readFd(uint32_t _event, RawData * pstData)
{
    bool bRet = false;

	/*若数据和断开连接的事件同时来，先读取数据，后处理断开事件*/
    if (0 != (_event & EPOLLIN))
    {
        bRet = TcpProcDataIn(pstData);
        if (false == bRet)
        {
            TcpProcHup();
        }
    }

    if (0 != (_event & (EPOLLHUP|EPOLLERR)))
    {
        bRet = false;
        TcpProcHup();
    }

    return bRet;
}

bool TcpDataChannel::writeFd(const RawData * pstData)
{
    bool bRet = false;
    if ((0 <= m_fd) && (pstData->iLength == send(m_fd, pstData->pucData, pstData->iLength, 0)))
    {
        bRet = true;
        std::cout<<"<----------------------------------------->"<<std::endl;
        std::cout<<"send to "<<m_fd<<":"<<Achannel::Convert2Printable(pstData)<<std::endl;
        std::cout<<"<----------------------------------------->"<<std::endl;
    }
    return bRet;
}

bool TcpDataChannel::TcpProcDataIn(RawData * pstData)
{
    bool bRet = false;
    
	/*收tcp数据，存到pstData中*/
	unsigned char aucBuff[256];
	ssize_t iRecvLen = 0;


	/*循环非阻塞recv，每次循环追加读取到的数据到pstData*/
	while (0 < (iRecvLen = recv(m_fd, aucBuff, sizeof(aucBuff), MSG_DONTWAIT)))
	{
		/*申请空间*/
		unsigned char *pucTemp = (unsigned char *)calloc(1UL, pstData->iLength + iRecvLen);
		memcpy(pucTemp, pstData->pucData, pstData->iLength);
		/*拷贝数据*/
		memcpy(pucTemp + pstData->iLength, aucBuff, iRecvLen);
		pstData->SetData(pucTemp, pstData->iLength + iRecvLen);
		free(pucTemp);
		bRet = true;
	}


    std::cout<<"<----------------------------------------->"<<std::endl;
    std::cout<<"recv from "<<m_fd<<":"<<Achannel::Convert2Printable(pstData)<<std::endl;
    std::cout<<"<----------------------------------------->"<<std::endl;

    return bRet;
}

void TcpDataChannel::TcpProcHup()
{
    std::cout<<"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"<<std::endl;
    std::cout<<m_fd<<" is hangup"<<std::endl;
    std::cout<<"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"<<std::endl;
    close(m_fd);
    m_fd = -1;
}
