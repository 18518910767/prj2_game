#pragma once
#include "Achannel.h"
#include <vector>

class TimerTask
{
	
public:
	virtual int GetPeroid() = 0;
	int iCountLeast = 0;
	virtual bool TimeOutProc() = 0;
};

class TimerChannel :
	public Achannel
{
	std::vector<TimerTask *> m_tasklist;
public:
	TimerChannel();
	virtual ~TimerChannel();

	// Í¨¹ý Achannel ¼Ì³Ð
	virtual bool init() override;
	virtual bool readFd(uint32_t _event, RawData * pstData) override;
	virtual bool writeFd(const RawData * pstData) override;
	virtual void fini() override;
	bool Add_Task(TimerTask *_task);
	void Del_Task(TimerTask *_task);
};

