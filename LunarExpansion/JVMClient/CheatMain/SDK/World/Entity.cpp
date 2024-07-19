#include "Entity.h"

namespace SDK 
{

	CEntity::CEntity(jobject EntityObject)
	{
		this->m_owningEntityObject = JavaExplorer::getEnv_S()->NewWeakGlobalRef(EntityObject);

		if (this->m_owningEntityObject) {
			Entity::AddEntity(this);
		}
	}

	CEntity::~CEntity()
	{
		Entity::RemoveEntity(this);
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
		     this->FCleanupSelf();

		delete this;
	}

	void CEntity::SetCallbackCleanupHelper(Eventhandler Function)
	{
		if (Function) {
			this->FCleanupSelf = Function;
			this->m_UsingCleanupSelf = true;
		}
		else
		{
			this->FCleanupSelf = nullptr;
			this->m_UsingCleanupSelf = false;
		}
		
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
	}
}


