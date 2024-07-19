#pragma once

#include <Windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <thread>


namespace Globals {
	bool IntiatedHandle = false;
	HANDLE stdouthandle = NULL;

	void InitiateConsoleValues();
	DWORD GetLogColor(unsigned char level);

	enum CheatStatus : unsigned char
	{
		exitByDemand = 0,
		runCheat = 1,
		FailedFatal = 2,
		FailedNonFatal = 3,
	};

	//Cheat Status
	unsigned char RunCheat = true; // 0 = false (Exit by demand), 1 = true, 2 = Failed (Something went wrong), 3 = Failed but not Fatal (Print Errors but exit not needed)

	//Main Stuff
	FILE* ConsoleFile = nullptr;
	std::thread mainThread;


	//Logging Stuff
	enum LogLevel : unsigned char
	{
		Success = 0,
		ErrorNonFatal = 1,
		ErrorFatal = 2,
		Warning = 3,
		Default = 4
	};

	void LogMessage(std::string Head, std::string Message, LogLevel level);


	//Error Stuff
	std::vector<std::pair<std::string, bool>> Errors;

	void SuccessLog(std::string Message) 
	{
		LogMessage("", Message, LogLevel::Success);
	}

	void Fail(std::string ErrorMsg, bool fatal) 
	{
		Errors.push_back(std::pair<std::string, bool>(ErrorMsg, fatal));
		RunCheat = (fatal ? CheatStatus::FailedFatal : CheatStatus::FailedNonFatal);
	}

	void EmptyErrorLogs() 
	{
		for (size_t i = 0; i < Errors.size(); i++)
		{
			LogMessage("Encountered Error", Errors[i].first, (Errors[i].second ? LogLevel::ErrorFatal : LogLevel::ErrorNonFatal));
		}
	}

}

