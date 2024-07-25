#pragma once

#include <math.h>
#include <iostream>

#include "../../Dependencies/Java.h"

//Normal c++ types for different Calculations
struct Vector2F {
	float x, y;

	Vector2F()
	{
		x = 0.0f, y = 0.0f;
	}

	float GetDistance(Vector2F& other);
};


struct Vector3F {
	float x, y, z;

	Vector3F()
	{
		x = 0.0f, y = 0.0f, z = 0.0f;
	}

	float GetDistance(Vector3F& other);
};


//Java Types to C++


namespace SDK
{
	namespace Offsets
	{
		namespace Iterator
		{
			inline bool InitializedIteratorOffsets = false;


		}
	}


	template <typename T>
	struct Iterator
	{
	public:
		using Vector_T = std::vector<T>;

		Iterator() 
		{
			
		}

		Iterator(jobject ItObj)
		{
			this->m_isJavaObject = true;


		}

		~Iterator()
		{
			//if()
		}
		
		Vector_T GetVector()
		{
			this->m_Items = Vector_T;

			if(this->m_isJavaObject)
				GetVectorData();
			
			return this->m_Items;
		}

		void SetVector(Vector_T items) 
		{
			this->m_Items = items;

			if (this->m_isJavaObject)
				SetVectorData();
		}

		void SetIteratorObject(jobject ItObj)
		{
			this->m_isLocked = false;

			this->m_OwningIteratorObject = ItObj;

			this->m_isJavaObject = (ItObj != nullptr);
		}

		constexpr bool IsLocked() const { return m_isLocked; };
		constexpr bool IsValid() const { return m_isValid; };
		constexpr bool IsJavaObj() const { return m_isJavaObject; };

	private:
		bool m_isLocked = false;
		bool m_isValid = false;

		bool m_isJavaObject = false;

		jobject m_OwningIteratorObject = nullptr;
		jobject m_strongref = nullptr;

		Vector_T m_Items;

		//Lock for GarbageCollector
		bool SetInteractionState(bool Locked)
		{
			auto Enviroment = JavaExplorer::getEnv_S();

			if (Locked) {
				auto strongRef = Enviroment->NewLocalRef(this->m_OwningIteratorObject);

				if (strongRef)
				{
					this->m_strongref = strongRef;
					this->m_isLocked = true;
				}
				else
				{
					delete this;
					return false;
				}
			}
			else
			{
				if (this->m_isLocked)
				{
					this->m_isLocked = false;
					Enviroment->DeleteLocalRef(this->m_strongref);

					this->m_strongref = nullptr;
				}
			}


			return true;
		}

		//Get Data from Java Iterator to m_Items
		void GetVectorData()
		{
			if (!m_isValid || SetInteractionState(true))
			{
				m_isValid = false;
				return;
			}


			SetInteractionState(false);
		}

		//Set Data from m_Items to Java Iterator
		void SetVectorData()
		{
			if (!m_isValid || SetInteractionState(true))
			{
				m_isValid = false;
				return;
			}



			SetInteractionState(false);
		}
	};

}