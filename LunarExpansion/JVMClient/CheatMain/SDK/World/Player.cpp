#include "Player.h"

namespace SDK 
{
	CPlayer::CPlayer()
	{
		this->m_PlayerCam = new CCamera(this);
	}

	CPlayer::~CPlayer()
	{
		delete this->m_PlayerCam;
	}

	bool CPlayer::IsValidPlayer()
	{
		bool PlayerValid = false;
		
		if (this->m_IsValid && this->m_OwningEntity->IsAlive())
			PlayerValid = true;
		else
			this->m_IsValid = false;

		return PlayerValid;
	}

	void CPlayer::FreePlayer_Callback(void* _player) 
	{
		CPlayer* player_ = (CPlayer*)_player;

		player_->m_IsValid = false;
		player_->m_OwningEntity = nullptr;
	}

	Vector3 CPlayer::GetPosition()
	{
		if (!m_IsValid)
			return Vector3();

		return this->m_OwningEntity->GetWorldPosition();
	}

	void CPlayer::InitiatePlayer(jobject MinecrafInstance)
	{
		if (!MinecrafInstance)
			return;

		auto Env = JavaExplorer::getEnv_S();

		auto MCClass = Env->GetObjectClass(MinecrafInstance);
		
		static jfieldID FieldPlayer = nullptr;

		FieldPlayer = Env->GetFieldID(MCClass, "player", "Lnet/minecraft/client/player/LocalPlayer;");

		if(FieldPlayer)
		{
			auto LocalPlayerEntity = Env->GetObjectField(MinecrafInstance, FieldPlayer);

			if (LocalPlayerEntity) 
			{
				this->m_OwningEntity = new CEntity(LocalPlayerEntity);

				this->m_OwningEntity->SetCallbackCleanupHelper(FreePlayer_Callback, this);

				this->m_IsValid = true;
			}
			else
			{
				this->m_OwningEntity = nullptr;
				this->m_IsValid = false;
			}
		}
		else
		{
			this->m_OwningEntity = nullptr;
			this->m_IsValid = false;
		}

		Env->DeleteLocalRef(MCClass);
	}

	CCamera::CCamera(CPlayer* PlayerInstance)
	{
	}

	CCamera::~CCamera()
	{
	}

}
