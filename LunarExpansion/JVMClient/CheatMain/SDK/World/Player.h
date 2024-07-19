#pragma once

#include "Entity.h"

namespace SDK 
{
	class CCamera
	{
	public:
		CCamera(CPlayer* PlayerInstance);
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
		//Checks if PlayerEntityObject is Valid
		bool IsValidPlayer();

		void SetPosition(Vector3 Position);
		void SetVelocity(Vector3 Velocity);
		void SetRotation(Vector3 Rotation);

		void InitiatePlayer(jobject MinecrafInstance);
	private:
		bool m_IsValid = false;

		class CEntity* m_OwningEntity = nullptr;
		class CCamera* m_PlayerCam = nullptr;
	};
}