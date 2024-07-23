#pragma once

#include <jni.h>
#include <jvmti.h>
#include <format>

#include <unordered_map>

#include "../Event.h"
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

	JavaExplorer* BindClassFunction_ToDestructor(std::function<void(void*)> Function, void* instance);
	void UnbindClassFunction(std::function<void(void*)> Function);
private:
	static JavaExplorer* currentObject;

	std::vector<void*> BindedInstances;
	EventCallback<void*> OnDestroy;

	JavaVM* m_jvm = nullptr;
	JNIEnv* m_env = nullptr;
	jvmtiEnv* m_jvmti = nullptr;

	//Gets freed with class
	jclass* ClassArray = nullptr;

	void GetLoadedClasses();

	std::unordered_map<std::string, jclass> LoadedClasses;
};

class jclazz //Handles Local Reference by deleting it after going out of bounds
{
private:
	jclass m_jclass = nullptr;

public:
	jclazz();
	jclazz(jclass classPtr);
	~jclazz();

	jclass GetClass();

	void operator=(jclass& j);
	operator bool();
	operator jclass() { return m_jclass; };
};

class JavaHooker
{
public:
	enum CallbackType : DWORD
	{
		vmInit = 1 << 0, vmDeath = 1 << 1, threadStart = 1 << 2, threadEnd = 1 << 3, methodEntry = 1 << 4, methodExit = 1 << 5, compiled_method_load = 1 << 6, All = vmInit | vmDeath | threadStart | threadEnd
		| methodEntry | methodExit | compiled_method_load,
	};

private:
	bool m_AddedCapabilities = false;
	bool m_failed = false;

	JavaExplorer* m_explorer = nullptr;
	jvmtiEnv* m_jvmti = nullptr;

	jvmtiEventCallbacks m_ecallbacks;

	static void OnCleanupJavaExplorer(void* instance);

	//Called at start and End once
	void SetupJvmtiCallback();
	void CleanupJvmtiCallback();

protected:
	virtual void EnableEventNotification(CallbackType ct);
	virtual void DisableEventNotification(CallbackType ct);


	static void JNICALL vmInitCallback(jvmtiEnv* jvmti, JNIEnv* env, jthread thread);
	static void JNICALL vmDeathCallback(jvmtiEnv* jvmti, JNIEnv* env);
	static void JNICALL threadStartCallback(jvmtiEnv* jvmti, JNIEnv* env, jthread thread);
	static void JNICALL threadEndCallback(jvmtiEnv* jvmti, JNIEnv* env, jthread thread);
	static void JNICALL methodEntryCallback(jvmtiEnv* jvmti, JNIEnv* env, jthread thread, jmethodID method);
	static void JNICALL methodExitCallback(jvmtiEnv* jvmti, JNIEnv* env, jthread thread, jmethodID method, jboolean wasPoppedByException, jvalue returnValue);
	static void JNICALL compiledMethodLoadCallback(jvmtiEnv* jvmti, jmethodID method, jint code_size, const void* code_addr, jint map_length, const jvmtiAddrLocationMap* map, const void* compile_info);

public:
	//Creates Callbacks and Enables Compability
	JavaHooker(JavaExplorer* explorerInstance);

	//Destroy Callbacks and Disables Compability
	~JavaHooker();

	//Activate and Deactivate EventSystem
	bool Activate(bool value);

	//Add your Functions on them!
	inline static EventCallback<jvmtiEnv*, JNIEnv*> onVmInit;
	inline static EventCallback<jvmtiEnv*, JNIEnv*> onVmDeath;
	inline static EventCallback<jvmtiEnv*, JNIEnv*, jthread> onThreadStart;
	inline static EventCallback<jvmtiEnv*, JNIEnv*, jthread> onThreadEnd;
	inline static EventCallback<jvmtiEnv*, JNIEnv*, jthread, jmethodID> onMethodEntry;
	inline static EventCallback<jvmtiEnv*, JNIEnv*, jthread, jmethodID, jboolean, jvalue> onMethodExit;
	inline static EventCallback<jvmtiEnv*, jmethodID, jint, const void*, jint, const jvmtiAddrLocationMap*, const void*> onCompiledMethodLoad;
};