#pragma once

#include "../SDK.h"
#include <functional>

namespace SDK
{
	namespace Offsets 
	{
		namespace Entity 
		{
			inline bool IntializedEntityOffsets = false;

			inline jmethodID j_IsAliveID = nullptr;
			inline jfieldID j_VelocityID = nullptr;
			inline jfieldID j_removalReason = nullptr;
			inline jfieldID j_WorldPositionID = nullptr;
			inline jmethodID j_WorldRotationID = nullptr;
			inline jmethodID j_getForward = nullptr;


			inline void GetEntity_IDS(jobject Entity)
			{
				auto Env = JavaExplorer::getEnv_S();

				jclazz EntityClass = Env->GetObjectClass(Entity);

				j_IsAliveID = Env->GetMethodID(EntityClass, "isAlive", "()Z");
				j_getForward = Env->GetMethodID(EntityClass, "getForward", "()Lnet/minecraft/world/phys/Vec3;");

				j_removalReason = Env->GetFieldID(EntityClass, "removalReason", "Lnet/minecraft/world/entity/Entity$RemovalReason;");
				j_WorldPositionID = Env->GetFieldID(EntityClass, "position", "Lnet/minecraft/world/phys/Vec3;");
				j_VelocityID = Env->GetFieldID(EntityClass, "deltaMovement", "Lnet/minecraft/world/phys/Vec3;");

				IntializedEntityOffsets = true;

			}
		}
	}

	class CEntity
	{
	public:
		CEntity(jobject EntityObject);
		~CEntity();

		struct Vector3 GetWorldPosition();
		struct Vector3 GetVelocity();

		struct Vector3 GetForwardVector();

		void SetVelocity(struct Vector3 Velocity); // Sets Velocity of the Entity (Client Sided else Localplayer and riding Entity or if Player uses Elytra that)
		void SetWorldPosition(struct Vector3 Position); //Sets the Position of the Entity. (Client Sided else Localplayer Server Sided but Lagback if Distance it too long!)
		void SetRotation(struct Vector3 Rotation, bool StepTurn); //Multi Threaded! if StepTurn is true (Client Sided Or Boat & Elytra)

		//To Ensure Garbage Collector doesnt fuck us up
		bool SetInteractionState(bool Locked);

		//Check if Object is still Alive
		bool IsAlive();

		//Check if the Entity is even Valid
		bool IsValid();

		//Call for Cleanup
		void FreeEntity();

		// Use this to Reference a void Function that gets Called when cleaning up stuff. example for cleaning up resources from Class using an CEntity instance, to let it know the Entity cant be used anymore. 
		// Call with Nullptr to deactivate
		void SetCallbackCleanupHelper(std::function<void(void*)> Function, void* _this)
		{
			this->m_UseObjectForCleanup = false;
			this->m_ObjectToCall = nullptr;

			if (Function) {
				this->FCleanupSelf = Function;
				this->m_UsingCleanupSelf = true;

				this->m_UseObjectForCleanup = (_this != nullptr);
				this->m_ObjectToCall = this;
			}
			else
			{
				this->FCleanupSelf = nullptr;
				this->m_UsingCleanupSelf = false;
			}

		}

		constexpr bool IsLocked() const { return m_IsLocked; };

		//If Entity is Special it will be Handled by itself and not stored with the Other Entities
		bool m_SpecialEntity = false;
	private:
		//Entity has to be locked before Accessing any Variable
		bool m_IsLocked = false;

		bool m_UsingCleanupSelf = false;
		bool m_UseObjectForCleanup = false;

		//Gets called for when the Entity gets deleted from the List to Clean up resources from Child Classes using an Reference to this class
		std::function<void(void*)> FCleanupSelf;

		void* m_ObjectToCall = nullptr;

		//weak reference to owning Entity Object
		jweak m_owningEntityObject = nullptr;

		//strong reference used to lock Garbage collector from data racing
		jobject m_strongRef = nullptr;

	};

	namespace Entity
	{
		//Dont use Directly!, Call Function instead
		inline std::vector<CEntity*> ValidEntities;

		//Gets called by Constructor Automatically
		void AddEntity(CEntity* EntityToAdd);

		//Gets called by Destructor Automatically
		void RemoveEntity(CEntity* EntityToRemove);

		//Fetch and Populate list
		//void FetchEntities(CMinecraft* Mc);

		//Put this in Loop before iterating Entity List
		void EnableEntities();

		//Call this to release all Entities safely
		void ReleaseEntities();

		//Call to Disable safe guard on all Entities
		void DisableEntities();
	}
}
