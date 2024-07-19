#pragma once

#include <jni.h>
#include <jvmti.h>
#include <format>
#include <unordered_map>

#include "../GlobalValues.h"


class JavaExplorer
{
public:
	JavaExplorer();
	~JavaExplorer();

	static constexpr JavaVM* getVM_S() { if (currentObject)return currentObject->m_jvm; else return nullptr; };
	static constexpr JNIEnv* getEnv_S() { if (currentObject)return currentObject->m_env; else return nullptr; };
	static constexpr jvmtiEnv* getJvmti_S() { if (currentObject)return currentObject->m_jvmti; else return nullptr; };

	static constexpr JavaExplorer* getInstance() { return currentObject; };

	constexpr JavaVM* getVM() { return m_jvm; };
	constexpr JNIEnv* getEnv() { return m_env; };
	constexpr jvmtiEnv* getJvmti() { return m_jvmti; };

	jclass FindClass(std::string Name);
	std::vector<std::string> GetClassFields(jclass class_);
private:
	static JavaExplorer* currentObject;

	JavaVM* m_jvm = nullptr;
	JNIEnv* m_env = nullptr;
	jvmtiEnv* m_jvmti = nullptr;

	void GetLoadedClasses();

	std::unordered_map<std::string, jclass> LoadedClasses;
};