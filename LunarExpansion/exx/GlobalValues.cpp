#include "GlobalValues.h"

void Globals::InitiateConsoleValues()
{
	stdouthandle = GetStdHandle(STD_OUTPUT_HANDLE);
	IntiatedHandle = true;

	CONSOLE_SCREEN_BUFFER_INFOEX info;

	GetConsoleScreenBufferInfoEx(stdouthandle, &info);

	//Mint Mango Green
	info.ColorTable[6] = RGB(15, 230, 122);

	//Orange
	info.ColorTable[7] = RGB(277, 143, 17);

	//Non Fatal Red
	info.ColorTable[8] = RGB(105, 24, 24);

	//Fatal Red
	info.ColorTable[9] = RGB(206, 49, 49);

	//Default White
	info.ColorTable[9] = RGB(255, 255, 255);

	SetConsoleScreenBufferInfoEx(stdouthandle, &info);
}

DWORD Globals::GetLogColor(unsigned char level)
{
	return level + ((unsigned char)6);
}

void Globals::LogMessage(std::string Head, std::string Message, LogLevel level)
{
	if (!IntiatedHandle) {
		InitiateConsoleValues();
	}

	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	GetConsoleScreenBufferInfo(stdouthandle, &consoleInfo);
	WORD originalAttributes = consoleInfo.wAttributes;

	std::cout << "[*] ";

	SetConsoleTextAttribute(stdouthandle, GetLogColor(level));

	std::cout << Head << ": " << Message << "\n";

	SetConsoleTextAttribute(stdouthandle, originalAttributes);
}

