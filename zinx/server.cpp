#include "server.h"
#include <list>
using namespace std;

Server *Server::pxServer = NULL;

Server::Server()
{   
}

bool Server::init()
{
	/*创建epollfd*/
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
    /*摘掉当前map中的所有channel*/
	for (auto pair : m_channel_map)
	{
		/*卸载channel对象*/
		uninstall_channel(pair.second);
		/*删除掉channel对象*/
		delete pair.second;
	}
	/*关掉epollfd*/
	close(m_epollfd);
	m_epollfd = -1;

	/*摘掉所有的role对象*/
	for (auto role_node : m_role_list_map)
	{
		/*遍历每个节点的list*/
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
		/*判断是否已经存在szCharacter对应的list,若无则创建*/
		if (m_role_list_map.end() == m_role_list_map.find(szCharacter))
		{
			list<Arole *> rolelist;
			/*添加pxrole到list*/
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
	/*删掉list中的元素*/
	auto itr = m_role_list_map.find(szCharacter);
	if (itr != m_role_list_map.end())
	{
		(*itr).second.remove(pxRole);
		/*判断list所在的map节点是否没有list元素，删掉map节点*/
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
		/*这行channel对象特有的初始化操作*/
		if (true == pxChannel->init())
		{
			epoll_event stEvent;
			/*定义添加到epoll树的数据*/
			stEvent.events = pxChannel->GetEvent();
			stEvent.data.ptr = pxChannel;

			/*添加pxChannel对象中的fd到epoll树*/
			if (0 == epoll_ctl(m_epollfd, EPOLL_CTL_ADD, pxChannel->GetFd(), &stEvent))
			{
				/*添加pxChannel对象到map*/
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
	/*从map中摘掉channel对象*/
	m_channel_map.erase(pxChannel->GetFd());
	/*从epoll树中摘掉fd*/
	epoll_ctl(m_epollfd, EPOLL_CTL_DEL, pxChannel->GetFd(), NULL);
	/*执行channel对象特有的去初始化操作*/
	pxChannel->fini();
}

bool Server::handle(Request * pstReq)
{
	/*拷贝传入的数据*/
	/*创建线程执行proc_msg*/
	/*通过线程间通信，告诉主线程执行结果*/
	return pstReq->pxProcessor->proc_msg(pstReq->pxMsg);
}

bool Server::send_resp(Response * pstResp)
{
	bool bRet = false;

	/*提取发送者和待发送的消息*/
	Arole *pxRole = pstResp->pxSender;
	Amessage *pxMsg = pstResp->pxMsg;
	/*获取输出通道（channel对象）*/
	Achannel *pxChannel = pxRole->GetChannel();
	if (NULL != pxChannel)
	{
		/*调用channel对象绑定的protocol对象response2raw转换原始数据*/
		Aprotocol *pxProtocol = pxChannel->GetProtocol();
		RawData stRawData;
		if (NULL != pxProtocol && true == pxProtocol->response2raw(pstResp, &stRawData))
		{
			/*调用writefd输出*/
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
		/*获取ready的所有fd*/
		while (true != m_need_exit) {
			int iepoll_ret = epoll_wait(m_epollfd, astEvents, 128, -1);
			if (0 > iepoll_ret)
			{
				/*被信号打断则认为不失败*/
				if (errno == EINTR)
				{
					continue;
				}
				else
				{
					break;
				}
			}
			/*遍历这些fd，读取数据，处理数据*/
			for (int i = 0; i < iepoll_ret; i++)
			{
				uint32_t events = astEvents[i].events;
				Achannel *pxChannel = static_cast<Achannel *> (astEvents[i].data.ptr);
				/*调用channel对象的readfd函数读取数据*/
				RawData stRawData;
				if (true == pxChannel->readFd(events, &stRawData))
				{
					/*调用channel对象绑定的protocol对象的raw2request进行数据处理*/
					auto pxProtocol = pxChannel->GetProtocol();
					list<Request *> reqlist;
					if (NULL != pxProtocol && true == pxProtocol->raw2request(&stRawData, reqlist))
					{
						/*调用handle函数处理request*/
						for (auto singlereq : reqlist)
						{
							(void)handle(singlereq);
						}
					}	
				}
				else
				{
					/*认为channel出错了，需要摘除并删掉*/
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

