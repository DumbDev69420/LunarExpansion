#include "Minecraft.h"

namespace SDK
{
	CMinecraft::CMinecraft()
	{
		auto JVEXP = JavaExplorer::getInstance();
	}

	jobject CMinecraft::GetInstance(bool Force = false)
	{
		if (this->Instance && !Force)
			return this->Instance;

		jclazz MinecraftClass = GetClass();

		if (!MinecraftClass)
			return nullptr;

		static jfieldID ID = nullptr;
		
		if(!ID)
		ID = JavaExplorer::getEnv_S()->GetStaticFieldID(MinecraftClass, "instance", "Lnet/minecraft/client/Minecraft;");

		jobject MCObject = JavaExplorer::getEnv_S()->GetStaticObjectField(MinecraftClass, ID);

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

	CLevel CMinecraft::GetCurrentLevel()
	{

		return CLevel();
	}

	jclazz CMinecraft::GetClass()
	{
		return JavaExplorer::getInstance()->FindClass("net.minecraft.client.Minecraft");
	}

	bool CLevel::SetInteractionState(bool Locked)
	{
		auto Enviroment = JavaExplorer::getEnv_S();

		if (Locked) {
			auto strongRef = Enviroment->NewLocalRef(this->LevelObj);

			if (strongRef)
			{
				this->m_strongRef = strongRef;
				this->m_IsLocked = true;
			}
			else
			{
				delete this;
				return false;
			}
		}
		else
		{
			if (this->m_IsLocked)
			{
				this->m_IsLocked = false;
				Enviroment->DeleteLocalRef(this->m_strongRef);

				this->m_strongRef = nullptr;
			}
		}


		return true;
	}

	bool CLevel::operator!=(CLevel& Level)
	{
		return (this->m_LevelReference != Level.m_LevelReference);
	}

	bool CLevel::operator==(CLevel& Level)
	{
		return (this->m_LevelReference == Level.m_LevelReference);
	}
}