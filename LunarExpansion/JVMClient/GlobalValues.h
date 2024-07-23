#pragma once

#include <Windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <thread>


//#define DEBUG


namespace Globals {
	extern bool IntiatedHandle;
	extern HANDLE stdouthandle;

	void InitiateConsoleValues();
	WORD GetLogColor(unsigned char level);

	enum CheatStatus : unsigned char
	{
		exitByDemand = 0,
		runCheat = 1,
		FailedFatal = 2,
		FailedNonFatal = 3,
	};

	//Cheat Status
	extern unsigned char RunCheat; // 0 = false (Exit by demand), 1 = true, 2 = Failed (Something went wrong), 3 = Failed but not Fatal (Print Errors but exit not needed)

	//Main Stuff
	extern FILE* ConsoleFile;
	extern std::thread mainThread;


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
	extern std::vector<std::pair<std::string, bool>> Errors;

	void SuccessLog(std::string Message);
	void Fail(std::string ErrorMsg, bool fatal);

	void EmptyErrorLogs();
}

