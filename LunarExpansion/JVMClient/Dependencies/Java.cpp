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

jclass JavaExplorer::FindClass(std::string Name) {

	if (this->LoadedClasses.contains(Name))
		return this->LoadedClasses.at(Name);

	Globals::Fail(std::string("Failed to Find Class: " + Name), false);

	return nullptr;
}
