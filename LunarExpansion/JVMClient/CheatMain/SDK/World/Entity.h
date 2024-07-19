#pragma once

#include "../SDK.h"
#include <functional>

namespace SDK
{
	namespace Offsets 
	{
		namespace Entity 
		{
			jfieldID j_IsLivingID = nullptr;
			jmethodID j_VelocityID = nullptr;
			jmethodID j_WorldPositionID = nullptr;
			jmethodID j_WorldRotationID = nullptr;


			void GetEntity_IDS(jclass EntityClass)
			{
				auto Env = JavaExplorer::getEnv_S();

				j_IsLivingID = Env->GetFieldID(EntityClass, "isAlive", "Z");
				j_VelocityID = Env->GetMethodID(EntityClass, "getMotion", "()Lcvi;");
			}
		}
	}

	class CEntity
	{
	private:
		using Eventhandler = std::function<void()>();
	public:
		CEntity(jobject EntityObject);
		~CEntity();

		Vector3 GetWorldPosition();
		Vector3 GetVelocity();

		void SetVelocity(Vector3 Velocity); // Sets Velocity of the Entity (Client Sided else Localplayer and riding Entity or if Player uses Elytra that)
		void SetWorldPosition(Vector3 Position); //Sets the Position of the Entity. (Client Sided else Localplayer Server Sided but Lagback if Distance it too long!)
		void SetRotation(Vector3 Rotation, bool StepTurn); //Multi Threaded! if StepTurn is true (Client Sided Or Boat & Elytra)

		//To Ensure Garbage Collector doesnt fuck us up
		bool SetInteractionState(bool Locked);

		//Check if Object is still Alive
		bool IsAlive();

		//Call for Cleanup
		void FreeEntity();

		// Use this to Reference a void Function that gets Called when cleaning up stuff. example for cleaning up resources from Class using an CEntity instance, to let it know the Entity cant be used anymore. 
		// Call with Nullptr to deactivate
		void SetCallbackCleanupHelper(Eventhandler Function);

		constexpr bool IsLocked() const { return m_IsLocked; };

		//If Entity is Special it will be Handled by itself and not stored with the Other Entities
		bool m_SpecialEntity = false;
	private:
		//Entity has to be locked before Accessing any Variable
		bool m_IsLocked = false;

		bool m_UsingCleanupSelf = false;

		//Gets called for when the Entity gets deleted from the List to Clean up resources from Child Classes using an Reference to this class
		std::function<void()> FCleanupSelf;

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
		void FetchEntities(CMinecraft* Mc);

		//Put this in Loop before iterating Entity List
		void EnableEntities();

		//Call this after Done using for Fast Clean Up
		void DisableEntities();
	}
}
