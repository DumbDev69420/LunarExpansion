#pragma once

#include "SDK.h"

namespace SDK
{
	namespace Offsets
	{
		namespace Minecraft
		{
			inline bool InitiatedFieldIDS = false;

			
		}

		namespace Level 
		{
			inline bool InitiatedFieldIDS = false;

			inline jfieldID j_isClientSide = nullptr;

			inline jmethodID j_getEntities = nullptr;
		}

		namespace LevelEntityGetter
		{
			inline bool InitiatedFieldIDS = false;

			inline jfieldID getAll = nullptr;

			inline void GetLevelEntityGetter_IDS(jobject LevelEntityGetter)
			{

			}
		}
	}

	class CMinecraft 
	{
	private:
		jobject Instance = nullptr;
	public:
		CMinecraft();

		jobject GetInstance(bool Force);
		class CPlayer GetPlayer();
		class CLevel GetCurrentLevel();

	private:
		jclazz GetClass();

		class CClientLevel* level = nullptr;
	};

	class CLevel
	{
	protected:
		jobject* m_LevelReference = nullptr;
		std::vector<class CEntity> m_Entities_InLCPRenderDistance;

		bool SetInteractionState(bool Locked);
	private:
		bool m_IsLocked = false;

		int m_CurrentDimension = 0;
		std::string m_LevelName = "";

	public:
		bool operator!=(CLevel& Level);
		bool operator==(CLevel& Level);

	private:
		jobject LevelObj = nullptr;
		//Local Ref
		jobject m_strongRef = nullptr;
	};

	class CClientLevel : CLevel
	{

	};
}

