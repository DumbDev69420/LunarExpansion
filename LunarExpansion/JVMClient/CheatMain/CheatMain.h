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


		CMinecraft _CMinecraft;

		jobject Minecraft_ = _CMinecraft.GetInstance();

		while (RunCheat == CheatStatus::runCheat && !GetAsyncKeyState(VK_F5))
		{

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

	delete explorer;

	FreeLibrary(Instance);
}