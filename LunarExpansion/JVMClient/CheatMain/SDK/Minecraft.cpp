#include "Minecraft.h"

namespace SDK
{
	CMinecraft::CMinecraft()
	{
		auto JVEXP = JavaExplorer::getInstance();
	}

	jobject CMinecraft::GetInstance()
	{
		auto MinecraftClass = GetClass();

		if (!MinecraftClass)
			return nullptr;

		static jfieldID ID = nullptr;
		
		ID = JavaExplorer::getEnv_S()->GetStaticFieldID(MinecraftClass, "instance", "Lnet/minecraft/client/Minecraft;");

		jobject MCObject = JavaExplorer::getInstance()->getEnv()->GetStaticObjectField(MinecraftClass, ID);

		if (MCObject)
		{
			//Globals::SuccessLog(std::format("Found Static Object of Minecraft!, Address:0x{:x}", (uintptr_t)MCObject));
			this->Instance = MCObject;
		}
		else
		{
			//Globals::Fail("Failed to get Minecraft Instance!", false);
			this->Instance = nullptr;
		}

		return MCObject;
	}

	CPlayer CMinecraft::GetPlayer()
	{
		CPlayer player;

		player.InitiatePlayer(this->GetInstance());

		return player;
	}

	jclass CMinecraft::GetClass()
	{
		return JavaExplorer::getInstance()->FindClass("net.minecraft.client.Minecraft");
	}

}