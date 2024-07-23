#pragma once

#include <Windows.h>
#include <functional>
#include <jni.h>
#include <jvmti.h>
#include <format>

#include "./SDK/SDK.h"

void OnMethodEntry_hook(jvmtiEnv* jvmti, JNIEnv* env, jthread thread, jmethodID method)
{
	std::cout << "Called!!!!!\n";
}

void MainThread(HMODULE Instance) 
{
	using namespace Globals;

	JavaExplorer* explorer = new JavaExplorer();

	//Not working for now, will fix later
	JavaHooker hookEngine_(explorer);

	hookEngine_.onMethodEntry += OnMethodEntry_hook;
	//////////////////////////////////////////////////

run:
	if (Globals::RunCheat == CheatStatus::runCheat) {
		if (GetAsyncKeyState(VK_F5))
		{
			Globals::RunCheat = CheatStatus::exitByDemand;
			goto run;
		}

		SDK::CMinecraft _CMinecraft;

		SDK::CPlayer Player;

		SDK::CLevel CurrentLevel;

		std::vector<SDK::CEntity> Entities;

		SuccessLog(std::format("Player Valid: {}", Player.IsValidPlayer()));

		while (RunCheat == CheatStatus::runCheat && !GetAsyncKeyState(VK_F5))
		{
			auto TickCurrent = GetTickCount64();

			static bool getentities = false;

			{
				static ULONGLONG UpdateEntityCleanup = 0;

				if (TickCurrent > UpdateEntityCleanup)
				{
					UpdateEntityCleanup = TickCurrent + 300;

					getentities = true;
					SDK::Entity::ReleaseEntities();
				}
			}

			{
				static ULONGLONG UpdateEntityList = 0;

				if (TickCurrent > UpdateEntityList || getentities)
				{
					getentities = false;

					std::vector<SDK::CEntity> CollectedEntities;

					UpdateEntityList = TickCurrent + 300;

#ifdef DEBUG
					SuccessLog(std::format("Entities Collected", Player.IsValidPlayer()));
#endif
				}
			}

			if (CurrentLevel != _CMinecraft.GetCurrentLevel())
			{
				CurrentLevel = _CMinecraft.GetCurrentLevel();
			}

			SDK::Entity::EnableEntities();

			if (Player.IsValidPlayer()) 
			{
				auto Location = Player.GetPosition().GetVec();

#ifdef DEBUG
				static ULONGLONG PrintPlayerPosTick = 0;

				if (TickCurrent > PrintPlayerPosTick)
				{
					PrintPlayerPosTick = TickCurrent + 500;
					SuccessLog(std::format("Player Position: x:{}, y:{}, z:{}", Location.x, Location.y, Location.z));
				}
#endif

				if (GetAsyncKeyState(VK_SPACE))
				{
					SDK::Vector3 Vec_ = Player.GetVelocity();

					auto VecDif = Vec_.GetVec();

					Vec_.SetVec({VecDif.x, 0.1f, VecDif.z});

					Player.SetVelocity(Vec_);
				}
			}
			else
			{
				static ULONGLONG UpdatePlayerTick = 0;

				if (TickCurrent > UpdatePlayerTick)
				{
					UpdatePlayerTick = TickCurrent + 300;

					Player = _CMinecraft.GetPlayer();
#ifdef DEBUG
					SuccessLog(std::format("Player Valid: {}", Player.IsValidPlayer()));
#endif
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