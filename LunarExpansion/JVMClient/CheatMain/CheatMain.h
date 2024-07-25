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
#ifdef DEBUG
				auto Location = Player.GetPosition().GetVec();

				static ULONGLONG PrintPlayerPosTick = 0;

				if (TickCurrent > PrintPlayerPosTick)
				{
					PrintPlayerPosTick = TickCurrent + 500;
					SuccessLog(std::format("Player Position: x:{}, y:{}, z:{}", Location.x, Location.y, Location.z));
				}
#endif
				static bool Flytest = false;

				if (GetAsyncKeyState('H')& 1)
					Flytest = !Flytest;

				if (Flytest)
				{
					const float Speed = 15.0f;

					const float FallDown_V = -0.7f;
					const ULONGLONG TickTillFallDown = 500;

					static ULONGLONG TickUpdateFallDown = 0;
					static ULONGLONG TickUpdateFallDown_Tilldone = 0; // atleast 20ms for Server

					SDK::Vector3 Vec_ = Player.GetForwardVector();
					SDK::Vector3 Vec_CCVelocity = Player.GetVelocity();

					auto VecDif = Vec_.GetVecNormalized();
					auto VecDif__CCVelocity = Vec_CCVelocity.GetVec();

					if (TickCurrent >= TickUpdateFallDown)
					{
						if (TickCurrent >= TickUpdateFallDown_Tilldone)
						{
							TickUpdateFallDown = TickCurrent + TickTillFallDown;
							TickUpdateFallDown_Tilldone = TickUpdateFallDown + 20;
						}
						else
						{
							Vec_.SetVec({ VecDif__CCVelocity.x, FallDown_V, VecDif__CCVelocity.z });
						}
					}

					float highSet = 1.0f;

					if (GetAsyncKeyState(VK_SPACE))
					{
						Vec_.SetVec({ VecDif.x * Speed, highSet * Speed, VecDif.z * Speed });
					}
					

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