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

	float GetDistance(Vector2F& other) {
		return std::sqrt(((other.x - this->x) * (other.x - this->x)) + ((other.y - this->y) * (other.y - this->y)));
	}
};


struct Vector3F {
	float x, y, z;

	Vector3F()
	{
		x = 0.0f, y = 0.0f, z = 0.0f;
	}

	float GetDistance(Vector3F& other) {
		return std::sqrt(((other.x - this->x) * (other.x - this->x)) + ((other.y - this->y) * (other.y - this->y)) + ((other.z - this->z) * (other.z - this->z)));
	}
};


//Java Types to C++


namespace SDK
{
	namespace Offsets
	{
		namespace Iterator
		{
			bool InitializedIteratorOffsets = false;


		}
	}

	template <typename T>
	struct Iterator
	{
	public:
		using Vector_T = std::vector<T>;

		Vector_T GetVector();
		void SetVector(Vector_T items);
	private:
		jobject m_OwningIteratorObject = nullptr;
		Vector_T m_Items;

		bool SetInteractionState(bool Locked);

		//Get Data from Iterator to m_Items
		void GetVectorData();
		//Set Data from Iterator
		void SetVectorData(Vector_T items)
	};

}