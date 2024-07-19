#pragma once

#include "Entity.h"

namespace SDK 
{
	class CCamera
	{
	public:
		CCamera(class CPlayer* PlayerInstance);
		~CCamera();

		Vector2F WorldPositionToScreen(Vector3 WorldPosition);

		float GetFov();
		void SetFOV(float NewFov);

	private:
		class CPlayer* m_LocalPlayer = nullptr;
	};


	class CPlayer
	{
	public:
		CPlayer();
		~CPlayer();

		//Checks if PlayerEntityObject is Valid
		bool IsValidPlayer();

		void SetPosition(Vector3 Position);
		void SetVelocity(Vector3 Velocity);
		void SetRotation(Vector3 Rotation);

		struct Vector3 GetPosition();

		void InitiatePlayer(jobject MinecrafInstance);
	private:
		bool m_IsValid = false;

		static void FreePlayer_Callback(void* _player);

		class CEntity* m_OwningEntity = nullptr;
		class CCamera* m_PlayerCam = nullptr;
	};
}