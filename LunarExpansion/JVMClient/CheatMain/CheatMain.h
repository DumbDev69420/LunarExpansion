#pragma once

#include <Windows.h>
#include <functional>
#include <jni.h>
#include <jvmti.h>
#include <format>

#include "./SDK/SDK.h"

void MainThread(HMODULE Instance) 
{
	using namespace Globals;

	JavaExplorer* explorer = new JavaExplorer();

run:
	if (Globals::RunCheat == CheatStatus::runCheat) {
		if (GetAsyncKeyState(VK_F5))
		{
			Globals::RunCheat = CheatStatus::exitByDemand;
			goto run;
		}


		SDK::CMinecraft _CMinecraft;

		SDK::CPlayer Player = _CMinecraft.GetPlayer();

		SuccessLog(std::format("Player Valid: {}", Player.IsValidPlayer()));

		while (RunCheat == CheatStatus::runCheat && !GetAsyncKeyState(VK_F5))
		{
			SDK::Entity::EnableEntities();

			if (Player.IsValidPlayer()) 
			{
				auto Location = Player.GetPosition().GetVec();

				SuccessLog(std::format("Player Position: x:{}, y:{}, z:{}", Location.x, Location.y, Location.z));
			}
			else
			{
				static ULONGLONG UpdatePlayerTick = 0;

				if (GetTickCount64() > UpdatePlayerTick) 
				{
					UpdatePlayerTick = GetTickCount64() + 300;

					Player = _CMinecraft.GetPlayer();
					SuccessLog(std::format("Player Valid: {}", Player.IsValidPlayer()));
				}
			}
		}

	}
	
	switch (RunCheat) {
	   case CheatStatus::FailedNonFatal:
#ifdef DEBUG
	   	    EmptyErrorLogs();
#endif
			RunCheat = CheatStatus::runCheat;
			goto run;
			break;

	   case CheatStatus::FailedFatal:
#ifdef DEBUG
	      	EmptyErrorLogs();
#endif
			break;
	}

	SDK::Entity::ReleaseEntities();

	delete explorer;

	FreeLibrary(Instance);
}