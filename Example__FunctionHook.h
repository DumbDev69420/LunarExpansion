#include <jni.h>
#include <jvmti.h>
#include <iostream>

// Callback definitions using EventCallback
EventCallback<> onVmInit;
EventCallback<> onVmDeath;
EventCallback<jthread> onThreadStart;
EventCallback<jthread> onThreadEnd;
EventCallback<jthread, jmethodID> onMethodEntry;
EventCallback<jthread, jmethodID, jboolean> onMethodExit;
EventCallback<jmethodID, jint, const void*, jint, const jvmtiAddrLocationMap*, const void*> onCompiledMethodLoad;

// JVMTI callbacks
void JNICALL vmInitCallback(jvmtiEnv* jvmti, JNIEnv* env, jthread thread) {
    onVmInit();
}

void JNICALL vmDeathCallback(jvmtiEnv* jvmti, JNIEnv* env) {
    onVmDeath();
}

void JNICALL threadStartCallback(jvmtiEnv* jvmti, JNIEnv* env, jthread thread) {
    onThreadStart(thread);
}

void JNICALL threadEndCallback(jvmtiEnv* jvmti, JNIEnv* env, jthread thread) {
    onThreadEnd(thread);
}

void JNICALL methodEntryCallback(jvmtiEnv* jvmti, JNIEnv* env, jthread thread, jmethodID method) {
    onMethodEntry(thread, method);
}

void JNICALL methodExitCallback(jvmtiEnv* jvmti, JNIEnv* env, jthread thread, jmethodID method, jboolean wasPoppedByException, jvalue returnValue) {
    onMethodExit(thread, method, wasPoppedByException);
}

void JNICALL compiledMethodLoadCallback(jvmtiEnv* jvmti, jmethodID method, jint code_size, const void* code_addr, jint map_length, const jvmtiAddrLocationMap* map, const void* compile_info) {
    onCompiledMethodLoad(method, code_size, code_addr, map_length, map, compile_info);
}

// Setup JVMTI callbacks
void setupJvmtiCallbacks(jvmtiEnv* jvmti) {
    jvmtiEventCallbacks callbacks;
    memset(&callbacks, 0, sizeof(callbacks));
    callbacks.VMInit = &vmInitCallback;
    callbacks.VMDeath = &vmDeathCallback;
    callbacks.ThreadStart = &threadStartCallback;
    callbacks.ThreadEnd = &threadEndCallback;
    callbacks.MethodEntry = &methodEntryCallback;
    callbacks.MethodExit = &methodExitCallback;
    callbacks.CompiledMethodLoad = &compiledMethodLoadCallback;

    jvmtiError error = jvmti->SetEventCallbacks(&callbacks, sizeof(callbacks));
    if (error != JVMTI_ERROR_NONE) {
        std::cerr << "Failed to set event callbacks: " << error << std::endl;
        exit(1);
    }
}

// Enable JVMTI event notifications
void enableEventNotifications(jvmtiEnv* jvmti) {
    jvmtiCapabilities capabilities;
    memset(&capabilities, 0, sizeof(capabilities));
    capabilities.can_generate_all_class_hook_events = 1;
    capabilities.can_generate_method_entry_events = 1;
    capabilities.can_generate_method_exit_events = 1;
    capabilities.can_generate_compiled_method_load_events = 1;

    jvmtiError error = jvmti->AddCapabilities(&capabilities);
    if (error != JVMTI_ERROR_NONE) {
        std::cerr << "Failed to add capabilities: " << error << std::endl;
        exit(1);
    }

    error = jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_VM_INIT, nullptr);
    if (error != JVMTI_ERROR_NONE) {
        std::cerr << "Failed to enable VM_INIT event: " << error << std::endl;
        exit(1);
    }

    error = jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_VM_DEATH, nullptr);
    if (error != JVMTI_ERROR_NONE) {
        std::cerr << "Failed to enable VM_DEATH event: " << error << std::endl;
        exit(1);
    }

    error = jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_THREAD_START, nullptr);
    if (error != JVMTI_ERROR_NONE) {
        std::cerr << "Failed to enable THREAD_START event: " << error << std::endl;
        exit(1);
    }

    error = jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_THREAD_END, nullptr);
    if (error != JVMTI_ERROR_NONE) {
        std::cerr << "Failed to enable THREAD_END event: " << error << std::endl;
        exit(1);
    }

    error = jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_METHOD_ENTRY, nullptr);
    if (error != JVMTI_ERROR_NONE) {
        std::cerr << "Failed to enable METHOD_ENTRY event: " << error << std::endl;
        exit(1);
    }

    error = jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_METHOD_EXIT, nullptr);
    if (error != JVMTI_ERROR_NONE) {
        std::cerr << "Failed to enable METHOD_EXIT event: " << error << std::endl;
        exit(1);
    }

    error = jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_COMPILED_METHOD_LOAD, nullptr);
    if (error != JVMTI_ERROR_NONE) {
        std::cerr << "Failed to enable COMPILED_METHOD_LOAD event: " << error << std::endl;
        exit(1);
    }
}

// Disable JVMTI event notifications
void disableEventNotifications(jvmtiEnv* jvmti) {
    jvmtiError error = jvmti->SetEventNotificationMode(JVMTI_DISABLE, JVMTI_EVENT_VM_INIT, nullptr);
    if (error != JVMTI_ERROR_NONE) {
        std::cerr << "Failed to disable VM_INIT event: " << error << std::endl;
    }

    error = jvmti->SetEventNotificationMode(JVMTI_DISABLE, JVMTI_EVENT_VM_DEATH, nullptr);
    if (error != JVMTI_ERROR_NONE) {
        std::cerr << "Failed to disable VM_DEATH event: " << error << std::endl;
    }

    error = jvmti->SetEventNotificationMode(JVMTI_DISABLE, JVMTI_EVENT_THREAD_START, nullptr);
    if (error != JVMTI_ERROR_NONE) {
        std::cerr << "Failed to disable THREAD_START event: " << error << std::endl;
    }

    error = jvmti->SetEventNotificationMode(JVMTI_DISABLE, JVMTI_EVENT_THREAD_END, nullptr);
    if (error != JVMTI_ERROR_NONE) {
        std::cerr << "Failed to disable THREAD_END event: " << error << std::endl;
    }

    error = jvmti->SetEventNotificationMode(JVMTI_DISABLE, JVMTI_EVENT_METHOD_ENTRY, nullptr);
    if (error != JVMTI_ERROR_NONE) {
        std::cerr << "Failed to disable METHOD_ENTRY event: " << error << std::endl;
    }

    error = jvmti->SetEventNotificationMode(JVMTI_DISABLE, JVMTI_EVENT_METHOD_EXIT, nullptr);
    if (error != JVMTI_ERROR_NONE) {
        std::cerr << "Failed to disable METHOD_EXIT event: " << error << std::endl;
    }

    error = jvmti->SetEventNotificationMode(JVMTI_DISABLE, JVMTI_EVENT_COMPILED_METHOD_LOAD, nullptr);
    if (error != JVMTI_ERROR_NONE) {
        std::cerr << "Failed to disable COMPILED_METHOD_LOAD event: " << error << std::endl;
    }
}

int main() {
    // Example usage
    onVmInit += []() {
        std::cout << "VM Init Event Triggered" << std::endl;
    };

    onVmDeath += []() {
        std::cout << "VM Death Event Triggered" << std::endl;
    };

    onThreadStart += [](jthread thread) {
        std::cout << "Thread Start Event Triggered" << std::endl;
    };

    onThreadEnd += [](jthread thread) {
        std::cout << "Thread End Event Triggered" << std::endl;
    };

    onMethodEntry += [](jthread thread, jmethodID method) {
        std::cout << "Method Entry Event Triggered" << std::endl;
    };

    onMethodExit += [](jthread thread, jmethodID method, jboolean wasPoppedByException) {
        std::cout << "Method Exit Event Triggered" << std::endl;
    };

    onCompiledMethodLoad += [](jmethodID method, jint code_size, const void* code_addr, jint map_length, const jvmtiAddrLocationMap* map, const void* compile_info) {
        std::cout << "Compiled Method Load Event Triggered" << std::endl;
    };

    // JVMTI initialization code
    JavaVM* jvm;
    JNIEnv* env;
    JavaVMInitArgs vm_args;
    JavaVMOption options[1];

    options[0].optionString = const_cast<char*>("-agentlib:jdwp=transport=dt_socket,server=y,suspend=n,address=*:5005");
    vm_args.version = JNI_VERSION_1_8;
    vm_args.nOptions = 1;
    vm_args.options = options;
    vm_args.ignoreUnrecognized = JNI_FALSE;

    jint res = JNI_CreateJavaVM(&jvm, (void**)&env, &vm_args);
    if (res != JNI_OK) {
        std::cerr << "Failed to create JVM: " << res << std::endl;
        return 1;
    }

    jvmtiEnv* jvmti;
    res = jvm->GetEnv((void**)&jvmti, JVMTI_VERSION_1_0);
    if (res != JNI_OK || jvmti == nullptr) {
        std::cerr << "Failed to get JVMTI environment: " << res << std::endl;
        return 1;
    }

    setupJvmtiCallbacks(jvmti);
    enableEventNotifications(jvmti);

    // Disable notifications as needed (for example, after some time or condition)
    disableEventNotifications(jvmti);

    // Main loop to keep the JVM alive
    jvm->DestroyJavaVM();
    return 0;
}
