#pragma once

#include "SDK.h"

namespace SDK
{
	class CMinecraft {
	private:
		jobject Instance = nullptr;
	public:
        CMinecraft();
        jobject GetInstance();
        class CPlayer GetPlayer();
        
private:
	jclass GetClass();
};
}

