#pragma once

//Used to identify the device type - required to create a Device ID
#ifdef _WIN32
constexpr const char* ID = "Windows_Device";
#else
constexpr const char* ID = "Unix_Device";
#endif

#include "eos_sdk.h"

class HWID
{
public:
    /** Constructor: Initializes the state */
    HWID();

    /** Starts the asynchronous login process */
    void Login(EOS_HPlatform* PlatformHandle);

    /** Checks if the login process has successfully completed */
    bool IsLoggedIn() const;

    /** Gets the user's ID. Returns nullptr if not logged in. */
    EOS_ProductUserId GetLocalUserId() const;

    static void EOS_CALL OnLoginComplete(const EOS_Connect_LoginCallbackInfo* Data);

    ~HWID() {
		delete m_ConnectHandle;
    }

private:
    // --- Member Variables ---
    // These store the state for a specific instance of the class.
    EOS_ProductUserId m_LocalUserId = nullptr;
    EOS_HConnect* m_ConnectHandle = nullptr;
    bool m_bIsLoggedIn;
};