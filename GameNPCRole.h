#pragma once
#include "GameRole.h"
#include "TimerChannel.h"
class GameNPCRole :
	public virtual GameRole,public virtual TimerTask
{
public:
	GameNPCRole();
	virtual ~GameNPCRole();
	void NotifyNewPlayer(GameRole *pxRole);

	// Í¨¹ý TimerTask ¼Ì³Ð
	virtual int GetPeroid() override;
	virtual bool TimeOutProc() override;
};

