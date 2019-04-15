#include "server.h"
#include <list>
using namespace std;

Server *Server::pxServer = NULL;

Server::Server()
{   
}

bool Server::init()
{
	/*����epollfd*/
	bool bRet = false;
	int iEpollFd = epoll_create(1);

	if (0 <= iEpollFd)
	{
		m_epollfd = iEpollFd;
		bRet = true;
	}
	else
	{
		perror("__FUNC__:epoll_create:");
	}

	return bRet;
}

void Server::fini()
{
    /*ժ����ǰmap�е�����channel*/
	for (auto pair : m_channel_map)
	{
		/*ж��channel����*/
		uninstall_channel(pair.second);
		/*ɾ����channel����*/
		delete pair.second;
	}
	/*�ص�epollfd*/
	close(m_epollfd);
	m_epollfd = -1;

	/*ժ�����е�role����*/
	for (auto role_node : m_role_list_map)
	{
		/*����ÿ���ڵ��list*/
		for (auto role : role_node.second)
		{
			del_role(role_node.first, role);
			delete role;
		}
	}
}

Server::~Server()
{
    fini();
}

bool Server::add_role(string szCharacter, Arole * pxRole)
{
	bool bRet = false;

	if (true == pxRole->init())
	{
		/*�ж��Ƿ��Ѿ�����szCharacter��Ӧ��list,�����򴴽�*/
		if (m_role_list_map.end() == m_role_list_map.find(szCharacter))
		{
			list<Arole *> rolelist;
			/*���pxrole��list*/
			rolelist.push_back(pxRole);
			m_role_list_map[szCharacter] = rolelist;
		}
		else
		{
			m_role_list_map[szCharacter].push_back(pxRole);
		}
		bRet = true;
	}

	return bRet;;
}

void Server::del_role(string szCharacter, Arole * pxRole)
{
	/*ɾ��list�е�Ԫ��*/
	auto itr = m_role_list_map.find(szCharacter);
	if (itr != m_role_list_map.end())
	{
		(*itr).second.remove(pxRole);
		/*�ж�list���ڵ�map�ڵ��Ƿ�û��listԪ�أ�ɾ��map�ڵ�*/
		if (0 == (*itr).second.size())
		{
			m_role_list_map.erase(itr);
		}
		pxRole->fini();
	}
	
}

bool Server::install_channel(Achannel * pxChannel)
{
	bool bRet = false;

	if (0 <= m_epollfd)
	{
		/*����channel�������еĳ�ʼ������*/
		if (true == pxChannel->init())
		{
			epoll_event stEvent;
			/*������ӵ�epoll��������*/
			stEvent.events = pxChannel->GetEvent();
			stEvent.data.ptr = pxChannel;

			/*���pxChannel�����е�fd��epoll��*/
			if (0 == epoll_ctl(m_epollfd, EPOLL_CTL_ADD, pxChannel->GetFd(), &stEvent))
			{
				/*���pxChannel����map*/
				m_channel_map[pxChannel->GetFd()] = pxChannel;
				bRet = true;
			}
			else
			{
				perror("__FUNC__:epoll_ctl:");
			}
		}
		else
		{
			cout << "channel init failed" << endl;
		}
	}


	return bRet;
}

void Server::uninstall_channel(Achannel * pxChannel)
{
	/*��map��ժ��channel����*/
	m_channel_map.erase(pxChannel->GetFd());
	/*��epoll����ժ��fd*/
	epoll_ctl(m_epollfd, EPOLL_CTL_DEL, pxChannel->GetFd(), NULL);
	/*ִ��channel�������е�ȥ��ʼ������*/
	pxChannel->fini();
}

bool Server::handle(Request * pstReq)
{
	/*�������������*/
	/*�����߳�ִ��proc_msg*/
	/*ͨ���̼߳�ͨ�ţ��������߳�ִ�н��*/
	return pstReq->pxProcessor->proc_msg(pstReq->pxMsg);
}

bool Server::send_resp(Response * pstResp)
{
	bool bRet = false;

	/*��ȡ�����ߺʹ����͵���Ϣ*/
	Arole *pxRole = pstResp->pxSender;
	Amessage *pxMsg = pstResp->pxMsg;
	/*��ȡ���ͨ����channel����*/
	Achannel *pxChannel = pxRole->GetChannel();
	if (NULL != pxChannel)
	{
		/*����channel����󶨵�protocol����response2rawת��ԭʼ����*/
		Aprotocol *pxProtocol = pxChannel->GetProtocol();
		RawData stRawData;
		if (NULL != pxProtocol && true == pxProtocol->response2raw(pstResp, &stRawData))
		{
			/*����writefd���*/
			bRet = pxChannel->writeFd(&stRawData);
		}
	}

	return bRet;
}

bool Server::run()
{
	bool bRet = false;

	if (0 <= m_epollfd)
	{
		epoll_event astEvents[128];
		/*��ȡready������fd*/
		while (true != m_need_exit) {
			int iepoll_ret = epoll_wait(m_epollfd, astEvents, 128, -1);
			if (0 > iepoll_ret)
			{
				/*���źŴ������Ϊ��ʧ��*/
				if (errno == EINTR)
				{
					continue;
				}
				else
				{
					break;
				}
			}
			/*������Щfd����ȡ���ݣ���������*/
			for (int i = 0; i < iepoll_ret; i++)
			{
				uint32_t events = astEvents[i].events;
				Achannel *pxChannel = static_cast<Achannel *> (astEvents[i].data.ptr);
				/*����channel�����readfd������ȡ����*/
				RawData stRawData;
				if (true == pxChannel->readFd(events, &stRawData))
				{
					/*����channel����󶨵�protocol�����raw2request�������ݴ���*/
					auto pxProtocol = pxChannel->GetProtocol();
					list<Request *> reqlist;
					if (NULL != pxProtocol && true == pxProtocol->raw2request(&stRawData, reqlist))
					{
						/*����handle��������request*/
						for (auto singlereq : reqlist)
						{
							(void)handle(singlereq);
						}
					}	
				}
				else
				{
					/*��Ϊchannel�����ˣ���Ҫժ����ɾ��*/
					uninstall_channel(pxChannel);
					delete pxChannel;
					break;
				}
			}
		}
		
	}

	return bRet;
}

void Server::ShutDownMainLoop()
{
    m_need_exit = true;
}

list<Arole*>* Server::GetRoleListByCharacter(std::string szCharacter)
{
	list<Arole*>* plist = NULL;
	auto itr = m_role_list_map.find(szCharacter);
	if (m_role_list_map.end() != itr)
	{
		plist =  &((*itr).second);
	}

	return plist;
}

