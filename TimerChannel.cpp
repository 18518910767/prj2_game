#include "TimerChannel.h"
#include <sys/timerfd.h>


TimerChannel::TimerChannel()
{
}


TimerChannel::~TimerChannel()
{
}

bool TimerChannel::init()
{
	bool bRet = false;
	int iTimerFd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC);
	if (0 <= iTimerFd)
	{
		itimerspec peroid;
		peroid.it_value.tv_nsec = 0;
		peroid.it_value.tv_sec = 1;
		peroid.it_interval.tv_nsec = 0;
		peroid.it_interval.tv_sec = 1;
		if (0 == timerfd_settime(iTimerFd, 0, &peroid, NULL))
		{
			m_fd = iTimerFd;
			bRet = true;
		}
		else
		{
			close(iTimerFd);
		}
	}
	return bRet;
}

bool TimerChannel::readFd(uint32_t _event, RawData * pstData)
{
	uint64_t ulCount = 0;

	read(m_fd, &ulCount, sizeof(ulCount));
	for (auto singleTask : m_tasklist)
	{
		if (singleTask->iCountLeast <= 0)
		{
			if (false == singleTask->TimeOutProc())
			{
				Del_Task(singleTask);
			}
			singleTask->iCountLeast = singleTask->GetPeroid();
		}
		singleTask->iCountLeast--;
	}

	return true;
}

bool TimerChannel::writeFd(const RawData * pstData)
{
	return false;
}

void TimerChannel::fini()
{
}
TimerChannel *pxTimerChannel = NULL;
bool TimerChannel::Add_Task(TimerTask * _task)
{
	bool bRet = false;

	_task->iCountLeast =  _task->GetPeroid();
	m_tasklist.push_back(_task);

	return bRet;
}

void TimerChannel::Del_Task(TimerTask * _task)
{
	for (auto itr = m_tasklist.begin(); itr != m_tasklist.end(); itr++)
	{
		if ((*itr) == _task)
		{
			m_tasklist.erase(itr);
			break;
		}
	}
}
