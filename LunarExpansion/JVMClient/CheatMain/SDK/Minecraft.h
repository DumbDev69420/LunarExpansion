#pragma once

#include "SDK.h"

namespace SDK
{
	namespace Offsets
	{
		namespace Minecraft
		{

		}
	}

	class CMinecraft 
	{
	private:
		jobject Instance = nullptr;
	public:
		CMinecraft();
		jobject GetInstance();
		class CPlayer GetPlayer();

	private:
		jclass GetClass();

		class CClientLevel* level = nullptr;
	};

	class CLevel
	{
	private:
		jobject* m_LevelReference = nullptr;

		std::string LevelName = "";
		int Dimension = 0;
	};

	class CClientLevel 
	{

	};
}

