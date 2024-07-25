#pragma once

#include "SDK.h"

namespace SDK 
{
	namespace Offsets 
	{
		namespace Math 
		{
			namespace Vector3 
			{
				inline bool InitiatedFieldIDS = false;

				inline jfieldID Vec3_xID = nullptr;
				inline jfieldID Vec3_yID = nullptr;
				inline jfieldID Vec3_zID = nullptr;

				inline void GetVector3_IDS(jclass Vector3Class) //net/miencraft/util/math. Clazz Name 1.21 - 1.12
				{
					auto env = JavaExplorer::getEnv_S();

					Vec3_xID = env->GetFieldID(Vector3Class, "x", "D");
					Vec3_yID = env->GetFieldID(Vector3Class, "y", "D");
					Vec3_zID = env->GetFieldID(Vector3Class, "z", "D");

					InitiatedFieldIDS = true;
				}
			}
		}
		
	}

	struct Vector3
	{
		struct vec3
		{
			double x, y, z;

			double GetNormalizationFactor()
			{
				return std::sqrt((x * x) + (y * y) + (z * z));
			}

			vec3 operator+(vec3 other)
			{
				return vec3{ x + other.x, y + other.y, z + other.z };
			}

			vec3 operator*(vec3 other)
			{
				return vec3{ x * other.x, y * other.y, z * other.z };
			}

			vec3 operator/(vec3 other)
			{
				return vec3{ x / other.x, y / other.y, z / other.z };
			}

			vec3 operator-(vec3 other)
			{
				return vec3{ x - other.x, y - other.y, z - other.z };
			}
		};

	private:
		bool m_UseJavaObj = false;
		bool m_Valid = true;
		jobject Vector3Obj = nullptr;

		vec3 Loc;

		//Get Current Java Objects Vector3
		bool GetVecFromObj();

		//Set Current Java Objects Vector3
		bool UpdateVec(vec3 position);
	public:
		Vector3();
		Vector3(jobject VectorObject);

		vec3 GetVec();
		vec3 GetVecNormalized();
		void SetVec(vec3 vec);

		void SetMode(bool UseInternalJavaObj) { m_UseJavaObj = UseInternalJavaObj; };
		void SetVectorObject(jobject VectorObject);

		void operator=(Vector3& other) 
		{
			if (!m_Valid) return;

			SetVec(other.GetVec());
		}

		void operator+=(Vector3& other) 
		{
			if (!m_Valid) return;

			vec3 other_ = other.GetVec();
			vec3 self_ = this->GetVec();

			SetVec(self_ + other_);
		}

		void operator-=(Vector3& other)
		{
			if (!m_Valid) return;

			vec3 other_ = other.GetVec();
			vec3 self_ = this->GetVec();

			SetVec(self_ - other_);
		}
	};
}