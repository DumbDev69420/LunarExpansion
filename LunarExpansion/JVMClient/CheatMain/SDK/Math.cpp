#include "Math.h"

namespace SDK
{
	bool Vector3::GetVecFromObj()
	{
		if (!m_Valid)
			return false;

		using namespace Offsets::Math::Vector3;

		auto Env = JavaExplorer::getEnv_S();

		try
		{
			this->Loc.x = Env->GetDoubleField(this->Vector3Obj, Vec3_xID);
			this->Loc.y = Env->GetDoubleField(this->Vector3Obj, Vec3_yID);
			this->Loc.z = Env->GetDoubleField(this->Vector3Obj, Vec3_zID);
		}
		catch (const std::exception&)
		{
			this->m_Valid = false;
			return false;
		}

		return true;
	}

	bool Vector3::UpdateVec(vec3 position)
	{
		if (!m_Valid)
			return false;

		using namespace Offsets::Math::Vector3;

		auto Env = JavaExplorer::getEnv_S();

		try
		{
			Env->SetDoubleField(this->Vector3Obj, Vec3_xID, this->Loc.x);
			Env->SetDoubleField(this->Vector3Obj, Vec3_yID, this->Loc.y);
			Env->SetDoubleField(this->Vector3Obj, Vec3_zID, this->Loc.z);
		}
		catch (const std::exception&)
		{
			this->m_Valid = false;
			return false;
		}

		return true;
	}

	Vector3::Vector3()
	{
		this->m_Valid = true;
		this->m_UseJavaObj = false;
	}

	Vector3::Vector3(jobject VectorObject)
	{
		if (!VectorObject)
		{
			this->m_UseJavaObj = false;
			this->m_Valid = false;
			return;
		}


		this->m_UseJavaObj = true;
		this->m_Valid = true;

		this->Vector3Obj = VectorObject;

		if (!Offsets::Math::Vector3::InitiatedFieldIDS) 
		{
			auto Env = JavaExplorer::getEnv_S();

			jclazz Clazz = Env->GetObjectClass(VectorObject);

			if (!Clazz) 
			{
				this->m_Valid = false;
				return;
			}
				

			Offsets::Math::Vector3::GetVector3_IDS(Clazz);
		}

		
	}

	Vector3::vec3 Vector3::GetVec()
	{
		vec3 Out = { 0.0, 0.0, 0.0 };

		if (m_UseJavaObj) 
		{
			auto Env = JavaExplorer::getEnv_S();

			auto strongRef = Env->NewLocalRef(this->Vector3Obj);

			if (strongRef) 
			{
				this->GetVecFromObj();
				Env->DeleteLocalRef(strongRef);
			}
			else
			{
				this->m_Valid = false;
				return Out;
			}
			
		}

		Out = this->Loc;

		return Out;
	}

	Vector3::vec3 Vector3::GetVecNormalized()
	{
		auto vec__ = GetVec();

		float normalizationfactor = vec__.GetNormalizationFactor();

		Vector3::vec3 out_ = { vec__.x / normalizationfactor, vec__.y / normalizationfactor, vec__.z / normalizationfactor };

		return vec__;
	}

	void Vector3::SetVec(vec3 vec)
	{
		if (m_UseJavaObj)
		{
			auto Env = JavaExplorer::getEnv_S();

			auto strongRef = Env->NewLocalRef(this->Vector3Obj);

			if (strongRef)
			{
				this->UpdateVec(vec);
				Env->DeleteLocalRef(strongRef);
			}
			else
			{
				this->m_Valid = false;
				return;
			}

		}
		
		this->Loc = vec;
	}

	void Vector3::SetVectorObject(jobject VectorObject)
	{
		m_UseJavaObj = true;
		m_Valid = true;

		this->Vector3Obj = VectorObject;
	}
}
