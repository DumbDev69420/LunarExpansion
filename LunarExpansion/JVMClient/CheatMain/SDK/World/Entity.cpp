#include "Entity.h"

namespace SDK 
{

	CEntity::CEntity(jobject EntityObject)
	{
		this->m_owningEntityObject = JavaExplorer::getEnv_S()->NewWeakGlobalRef(EntityObject);

		if (this->m_owningEntityObject) {
			if (!Offsets::Entity::IntializedEntityOffsets)
				Offsets::Entity::GetEntity_IDS(EntityObject);

			Entity::AddEntity(this);
		}
	}

	CEntity::~CEntity()
	{
		Entity::RemoveEntity(this);
	}

	Vector3 CEntity::GetWorldPosition()
	{
		using namespace SDK::Offsets::Entity;

		auto Env = JavaExplorer::getEnv_S();

		auto Vector_ = Env->GetObjectField(m_owningEntityObject, j_WorldPositionID);

		return Vector3(Vector_);
	}

	Vector3 CEntity::GetVelocity()
	{
		return Vector3();
	}

	void CEntity::SetVelocity(Vector3 Velocity)
	{
	}

	void CEntity::SetWorldPosition(Vector3 Position)
	{
	}

	void CEntity::SetRotation(Vector3 Rotation, bool StepTurn)
	{
	}

	bool CEntity::IsAlive() 
	{
		using namespace SDK::Offsets::Entity;

		auto Env = JavaExplorer::getEnv_S();

		return Env->GetBooleanField(m_owningEntityObject, j_IsLivingID);
	}

	bool CEntity::SetInteractionState(bool Locked)
	{
		auto Enviroment = JavaExplorer::getEnv_S();

		if (Locked) {
			auto strongRef = Enviroment->NewGlobalRef(this->m_owningEntityObject);

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
				Enviroment->DeleteGlobalRef(this->m_strongRef);

				this->m_strongRef = nullptr;
			}
		}
		

		return true;
	}

	void CEntity::FreeEntity()
	{
		if(m_UsingCleanupSelf)
		     this->FCleanupSelf(this->m_ObjectToCall);

		delete this;
	}

	namespace Entity
	{
		void AddEntity(CEntity* EntityToAdd)
		{
			if (!EntityToAdd->IsLocked())
			{
				if (!EntityToAdd->SetInteractionState(true))
				{
					return; // Failed to Lock Entity, Garbage Collector was faster
				}
			}

			ValidEntities.push_back(EntityToAdd);
		}

		void RemoveEntity(CEntity* EntityToRemove)
		{
			for (auto Entitybeg_ = ValidEntities.begin(); Entitybeg_ != ValidEntities.end(); )
			{
				if (EntityToRemove == *Entitybeg_)
				{
					auto Entity = *Entitybeg_;
					Entity->SetInteractionState(false);
					Entitybeg_ = ValidEntities.erase(Entitybeg_);
					break;
				}
				else
				{
					Entitybeg_++;
				}

			}
		}

		void EnableEntities()
		{
			for (size_t i = 0; i < ValidEntities.size(); i++)
			{
				if (!ValidEntities[i]->IsAlive())
				{
					ValidEntities[i]->SetInteractionState(false);

					auto entity_B = ValidEntities.begin();
					entity_B = ValidEntities.erase(ValidEntities.begin() + i);
					i--;
					continue;
				}
			}
		}

		void DisableEntities()
		{
			for (auto& EntityCurrent : ValidEntities)
			{
				EntityCurrent->SetInteractionState(false);
			}
		}

		void ReleaseEntities()
		{
			DisableEntities();

			std::vector<CEntity*> CopyList = ValidEntities;

			for (auto& EntityCurrent : CopyList)
			{
				//if (!EntityCurrent->m_SpecialEntity)
					delete EntityCurrent;
			}

			ValidEntities.clear();
		}
	}
}


