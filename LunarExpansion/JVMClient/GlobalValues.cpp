#include "GlobalValues.h"

unsigned char Globals::RunCheat = true;
bool Globals::IntiatedHandle = false;
HANDLE Globals::stdouthandle = NULL;
FILE* Globals::ConsoleFile = nullptr;
std::thread Globals::mainThread;
std::vector<std::pair<std::string, bool>> Globals::Errors;

void Globals::InitiateConsoleValues()
{
	stdouthandle = GetStdHandle(STD_OUTPUT_HANDLE);
	IntiatedHandle = true;

	CONSOLE_SCREEN_BUFFER_INFOEX info = { sizeof(CONSOLE_SCREEN_BUFFER_INFOEX) };

	GetConsoleScreenBufferInfoEx(stdouthandle, &info);

	// Mint Mango Green
	info.ColorTable[6] = RGB(15, 230, 122);

	// Orange
	info.ColorTable[7] = RGB(255, 143, 17);

	// Non Fatal Red
	info.ColorTable[8] = RGB(105, 24, 24);

	// Fatal Red
	info.ColorTable[9] = RGB(206, 49, 49);

	//Default White
	info.ColorTable[10] = RGB(255, 255, 255);

	SetConsoleScreenBufferInfoEx(stdouthandle, &info);
}

WORD Globals::GetLogColor(unsigned char level)
{
	switch (level) {
	case LogLevel::Success: return 6; // Green
	case LogLevel::ErrorNonFatal: return 8; // Non Fatal Red
	case LogLevel::ErrorFatal: return 9; // Fatal Red
	default: return 10; // Default to white
	}
}

void Globals::LogMessage(std::string Head, std::string Message, LogLevel level)
{
	if (!IntiatedHandle) {
#ifndef DEBUG
		return;
#else
		InitiateConsoleValues();
#endif
	}

	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	GetConsoleScreenBufferInfo(stdouthandle, &consoleInfo);
	WORD originalAttributes = consoleInfo.wAttributes;

	std::cout << "[*] ";

	auto color = GetLogColor(level);

	SetConsoleTextAttribute(stdouthandle, color);

	std::cout << Head << ": " << Message << "\n";

	SetConsoleTextAttribute(stdouthandle, originalAttributes);
}

void Globals::SuccessLog(std::string Message)
{
	LogMessage("", Message, LogLevel::Success);
}

void Globals::Fail(std::string ErrorMsg, bool fatal)
{
	Errors.push_back(std::pair<std::string, bool>(ErrorMsg, fatal));
	RunCheat = (fatal ? CheatStatus::FailedFatal : CheatStatus::FailedNonFatal);
}

void Globals::EmptyErrorLogs()
{
	for (size_t i = 0; i < Errors.size(); i++)
	{
		LogMessage("Encountered Error", Errors[i].first, (Errors[i].second ? LogLevel::ErrorFatal : LogLevel::ErrorNonFatal));
	}
}