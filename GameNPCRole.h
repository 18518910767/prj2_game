#pragma once
#include "GameRole.h"
#include "TimerChannel.h"
class GameNPCRole :
	public GameRole,public TimerTask
{
public:
	GameNPCRole();
	virtual ~GameNPCRole();
	void NotifyNewPlayer(GameRole *pxRole);

	// ͨ�� TimerTask �̳�
	virtual int GetPeroid() override;
	virtual bool TimeOutProc() override;
};

