#include "Java.h"


JavaExplorer* JavaExplorer::currentObject = nullptr;

JavaExplorer::JavaExplorer()
{

	jint result = JNI_GetCreatedJavaVMs(&m_jvm, 1, nullptr);

	if (result == JNI_ERR) {
		Globals::Fail("Failed to Get JavaVM", true);
	}
	else
	{
		Globals::SuccessLog(std::format("Successfully gotten JavaVM!, Address: 0x{:x}", (uintptr_t)m_jvm));

		result = m_jvm->AttachCurrentThread((void**)&m_env, nullptr);

		if (result == JNI_ERR) {
			Globals::Fail("Failed to Attach to JVM Thread", true);
		}
		else
		{
			Globals::SuccessLog(std::format("Successfully Attached to jvm Thread!, Address: 0x{:x}", (uintptr_t)m_env));

			if (m_jvm->GetEnv((void**)&m_jvmti, JVMTI_VERSION_1_2) == JNI_OK) {
					Globals::SuccessLog(std::format("Successfully gotten JVMTI!, Address: 0x{:x}", (uintptr_t)m_jvmti));

					currentObject = this;
					GetLoadedClasses();
			}
			else
				Globals::Fail("Failed to Get JVMTI", true);
			
		}


	}
}

JavaExplorer::~JavaExplorer()
{
	for (size_t i = 0; i < this->OnDestroy.getFunctionCount(); i++)
	{
		this->OnDestroy[i](this->BindedInstances[i]);
	}

	currentObject = nullptr;

	m_jvmti->Deallocate((unsigned char*)this->ClassArray);
	m_jvm->DetachCurrentThread();

	m_jvmti = nullptr;
	m_jvm = nullptr;
	m_env = nullptr;
}

void JavaExplorer::GetLoadedClasses()
{
	std::unordered_map<std::string, jclass> listOut;

	jclass lang = m_env->FindClass("java/lang/Class");
	jmethodID getName = m_env->GetMethodID(lang, "getName", "()Ljava/lang/String;");

	jclass* classes = nullptr;
	jint amount = 0;

	m_jvmti->GetLoadedClasses(&amount, &classes);

	jboolean isCopy;

	ClassArray = classes;

	for (size_t i = 0; i < amount; i++)
	{
		jstring name = (jstring)m_env->CallObjectMethod(classes[i], getName);
		const char* className = m_env->GetStringUTFChars(name, &isCopy);

		m_env->ReleaseStringUTFChars(name, className);

		listOut.emplace(std::make_pair((std::string)className, classes[i]));
	}

	m_env->DeleteLocalRef(lang);

	this->LoadedClasses = listOut;
}

std::vector<std::string> JavaExplorer::GetClassFields(jclass class_) {
	if (!class_) 
	{
		Globals::Fail("Couldn't Get Class Fields, Class was nullptr!", false);
		return std::vector<std::string>();
	}
		

	jclass lang = m_env->FindClass("java/lang/Class");

	jmethodID getDeclaredFields = m_env->GetMethodID(lang, "getDeclaredFields", "()[Ljava/lang/reflect/Field;");
	jmethodID getName = m_env->GetMethodID(lang, "getName", "()Ljava/lang/String;");

	jobjectArray fieldsArray = (jobjectArray)m_env->CallObjectMethod(class_, getDeclaredFields);

	jsize numberofFields = m_env->GetArrayLength(fieldsArray);

	std::vector<std::string> Fields(numberofFields);

	for (jsize i = 0; i < numberofFields; i++)
	{
		jobject fieldObject = m_env->GetObjectArrayElement(fieldsArray, i);

		jclass fieldClass = m_env->GetObjectClass(fieldObject);

		// Get the getName method ID
		jmethodID getNameMethod = m_env->GetMethodID(fieldClass, "getName", "()Ljava/lang/String;");
		if (getNameMethod == NULL) {
			break;
		}

		// Call the getName method
		jstring fieldName = (jstring)m_env->CallObjectMethod(fieldObject, getNameMethod);

		// Convert the Java string to a C string
		const char* fieldNameCStr = m_env->GetStringUTFChars(fieldName, NULL);

		// Release the Java string
		m_env->ReleaseStringUTFChars(fieldName, fieldNameCStr);

		Fields[i] = std::string(fieldNameCStr);

		// Delete the local reference to the field object
		m_env->DeleteLocalRef(fieldObject);
	}

	return Fields;

}

JavaExplorer* JavaExplorer::BindClassFunction_ToDestructor(std::function<void(void*)> Function, void* instance)
{
	this->OnDestroy += Function;
	this->BindedInstances.push_back(instance);

	return this;
}

//When using Class dont forget to delete Local References!
jclass JavaExplorer::FindClass(std::string Name) {

	if (this->LoadedClasses.contains(Name))
		return this->LoadedClasses.at(Name);

	Globals::Fail(std::string("Failed to Find Class: " + Name), false);

	return nullptr;
}

jclazz::jclazz()
{

}

jclazz::jclazz(jclass classPtr)
{
	this->m_jclass = classPtr;
}

jclazz::~jclazz()
{
	auto Env_ = JavaExplorer::getEnv_S();

	if (m_jclass)
	{
		if (Env_)
		{
			Env_->DeleteLocalRef(this->m_jclass);
		}
		else
		{
			throw std::exception("Failed to delete Reference to class because Env was nullptr");
		}
	}
}

jclass jclazz::GetClass()
{
	return this->m_jclass;
}


void jclazz::operator=(jclass& j)
{
	auto Env_ = JavaExplorer::getEnv_S();

	if (Env_ && m_jclass)
	{
		Env_->DeleteLocalRef(this->m_jclass);
	}

	this->m_jclass = j;
}

jclazz::operator bool()
{
	return (this->m_jclass != nullptr);
}

void JavaHooker::OnCleanupJavaExplorer(void* instance)
{
	JavaHooker* hook = (JavaHooker*)instance;

	//Deactivate everything thats relevant
	hook->Activate(false);
}

void JavaHooker::SetupJvmtiCallback()
{
	this->m_failed = false;

	m_ecallbacks.VMInit = &vmInitCallback;
	m_ecallbacks.VMDeath = &vmDeathCallback;
	m_ecallbacks.ThreadStart = &threadStartCallback;
	m_ecallbacks.ThreadEnd = &threadEndCallback;
	m_ecallbacks.MethodEntry = &methodEntryCallback;
	m_ecallbacks.MethodExit = &methodExitCallback;
	m_ecallbacks.CompiledMethodLoad = &compiledMethodLoadCallback;

	jvmtiError error = m_jvmti->SetEventCallbacks(&m_ecallbacks, sizeof(m_ecallbacks));

	if (error != JVMTI_ERROR_NONE) {
		Globals::Fail(std::string("Failed to set event callbacks: " + std::to_string(error)), false);
		this->m_failed = true;
	}
}

void JavaHooker::CleanupJvmtiCallback()
{
	m_jvmti->SetEventCallbacks(nullptr, 0);
}

void JavaHooker::EnableEventNotification(CallbackType ct)
{
	jvmtiError error = JVMTI_ERROR_NONE;

	if (!this->m_AddedCapabilities)
	{
		jvmtiCapabilities capabilities;
		memset(&capabilities, 0, sizeof(capabilities));
		capabilities.can_generate_all_class_hook_events = 1;
		capabilities.can_generate_method_entry_events = 1;
		capabilities.can_generate_method_exit_events = 1;
		capabilities.can_generate_compiled_method_load_events = 1;

		error = m_jvmti->AddCapabilities(&capabilities);
		if (error != JVMTI_ERROR_NONE) {
			Globals::Fail("Failed to add capabilities: " + std::to_string(error), false);
			return;
		}
		else
		{
			this->m_AddedCapabilities = true;
		}
	}


	if (ct & vmInit)
	{
		error = m_jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_VM_INIT, nullptr);
		if (error != JVMTI_ERROR_NONE) {
			Globals::Fail("Failed to enable VM_INIT event: " + std::to_string(error), false);
		}
	}

	if (ct & vmDeath)
	{
		error = m_jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_VM_DEATH, nullptr);
		if (error != JVMTI_ERROR_NONE) {
			Globals::Fail("Failed to enable VM_DEATH event: " + std::to_string(error), false);
		}
	}

	if (ct & threadStart)
	{
		error = m_jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_THREAD_START, nullptr);
		if (error != JVMTI_ERROR_NONE) {
			Globals::Fail("Failed to enable THREAD_START event: " + std::to_string(error), false);
		}
	}

	if (ct & threadEnd)
	{
		error = m_jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_THREAD_END, nullptr);
		if (error != JVMTI_ERROR_NONE) {
			Globals::Fail("Failed to enable THREAD_END event: " + std::to_string(error), false);
		}
	}

	if (ct & methodEntry)
	{
		error = m_jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_METHOD_ENTRY, nullptr);
		if (error != JVMTI_ERROR_NONE) {
			Globals::Fail("Failed to enable METHOD_ENTRY event: " + std::to_string(error), false);
		}
	}
	
	if (ct & methodExit)
	{
		error = m_jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_METHOD_EXIT, nullptr);
		if (error != JVMTI_ERROR_NONE) {
			Globals::Fail("Failed to enable METHOD_EXIT event: " + std::to_string(error), false);
		}
	}

	if (ct & compiled_method_load)
	{
		error = m_jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_COMPILED_METHOD_LOAD, nullptr);
		if (error != JVMTI_ERROR_NONE) {
			Globals::Fail("Failed to enable COMPILED_METHOD_LOAD event: " + std::to_string(error), false);
		}
	}


}

void JavaHooker::DisableEventNotification(CallbackType ct)
{
	jvmtiError error = JVMTI_ERROR_NONE;

	if (ct & vmInit)
	{
		error = m_jvmti->SetEventNotificationMode(JVMTI_DISABLE, JVMTI_EVENT_VM_INIT, nullptr);
		if (error != JVMTI_ERROR_NONE) {
			Globals::Fail("Failed to disable VM_INIT event: " + std::to_string(error), false);
		}
	}

	if (ct & vmDeath)
	{
		error = m_jvmti->SetEventNotificationMode(JVMTI_DISABLE, JVMTI_EVENT_VM_DEATH, nullptr);
		if (error != JVMTI_ERROR_NONE) {
			Globals::Fail("Failed to disable VM_DEATH event: " + std::to_string(error), false);
		}
	}

	if (ct & threadStart)
	{
		error = m_jvmti->SetEventNotificationMode(JVMTI_DISABLE, JVMTI_EVENT_THREAD_START, nullptr);
		if (error != JVMTI_ERROR_NONE) {
			Globals::Fail("Failed to disable THREAD_START event: " + std::to_string(error), false);
		}
	}

	if (ct & threadEnd)
	{
		error = m_jvmti->SetEventNotificationMode(JVMTI_DISABLE, JVMTI_EVENT_THREAD_END, nullptr);
		if (error != JVMTI_ERROR_NONE) {
			Globals::Fail("Failed to disable THREAD_END event: " + std::to_string(error), false);
		}
	}

	if (ct & methodEntry)
	{
		error = m_jvmti->SetEventNotificationMode(JVMTI_DISABLE, JVMTI_EVENT_METHOD_ENTRY, nullptr);
		if (error != JVMTI_ERROR_NONE) {
			Globals::Fail("Failed to disable METHOD_ENTRY event: " + std::to_string(error), false);
		}
	}

	if (ct & methodExit)
	{
		error = m_jvmti->SetEventNotificationMode(JVMTI_DISABLE, JVMTI_EVENT_METHOD_EXIT, nullptr);
		if (error != JVMTI_ERROR_NONE) {
			Globals::Fail("Failed to disable METHOD_EXIT event: " + std::to_string(error), false);
		}
	}

	if (ct & compiled_method_load)
	{
		error = m_jvmti->SetEventNotificationMode(JVMTI_DISABLE, JVMTI_EVENT_COMPILED_METHOD_LOAD, nullptr);
		if (error != JVMTI_ERROR_NONE) {
			Globals::Fail("Failed to disable COMPILED_METHOD_LOAD event: " + std::to_string(error), false);
		}
	}
}

JavaHooker::JavaHooker(JavaExplorer* explorerInstance)
{

	memset(&m_ecallbacks, 0, sizeof(m_ecallbacks));
	explorerInstance->BindClassFunction_ToDestructor(JavaHooker::OnCleanupJavaExplorer, this);

	this->m_jvmti = explorerInstance->getJvmti();

	SetupJvmtiCallback();

	Activate(true);
}

JavaHooker::~JavaHooker()
{
	Activate(false);

	CleanupJvmtiCallback();
}

bool JavaHooker::Activate(bool value)
{

	if (value)
	{
		this->EnableEventNotification(All);
	}
	else
	{
		this->DisableEventNotification(All);
	}

	return false;
}

// Called when vm gets intiated after creation
void JNICALL JavaHooker::vmInitCallback(jvmtiEnv* jvmti, JNIEnv* env, jthread thread)
{
	onVmInit(jvmti, env);
}

// Called when Vm exits and cleans up
void JNICALL JavaHooker::vmDeathCallback(jvmtiEnv* jvmti, JNIEnv* env)
{
	onVmDeath(jvmti, env);
}

// Called when Thread Starts
void JNICALL JavaHooker::threadStartCallback(jvmtiEnv* jvmti, JNIEnv* env, jthread thread)
{
	onThreadStart(jvmti, env, thread);
}

// Called when Thread Ends
void JNICALL JavaHooker::threadEndCallback(jvmtiEnv* jvmti, JNIEnv* env, jthread thread)
{
	onThreadEnd(jvmti, env, thread);
}

// Called when Method gets Called
void JNICALL JavaHooker::methodEntryCallback(jvmtiEnv* jvmti, JNIEnv* env, jthread thread, jmethodID method)
{
	onMethodEntry(jvmti, env, thread, method);
}

// Called when Method finishes and exits
void JNICALL JavaHooker::methodExitCallback(jvmtiEnv* jvmti, JNIEnv* env, jthread thread, jmethodID method, jboolean wasPoppedByException, jvalue returnValue)
{
	onMethodExit(jvmti, env, thread, method, wasPoppedByException, returnValue);
}

// Called after Method gets Jit Compiled, can be used for Hooking Functions if too late use methodEntry for "hooking"
void JNICALL JavaHooker::compiledMethodLoadCallback(jvmtiEnv* jvmti, jmethodID method, jint code_size, const void* code_addr, jint map_length, const jvmtiAddrLocationMap* map, const void* compile_info)
{
	onCompiledMethodLoad(jvmti, method, code_size, code_addr, map_length, map, compile_info);
}
