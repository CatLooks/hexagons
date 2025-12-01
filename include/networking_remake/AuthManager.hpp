#pragma once

//Used to identify the device type - required to create a Device ID
#ifdef _WIN32
constexpr const char* ID = "Windows_Device";
#else
constexpr const char* ID = "Unix_Device";
#endif

#include <iostream>
#include "eos_sdk.h"
#include "eos_auth.h"
#include "eos_init.h"
#include "networking_remake/Delegate.hpp"

class AuthManager {

public:
    //anouncment for successful login list
    Delegate<EOS_ProductUserId> OnLoginSuccess;
    /** Constructor: Initializes the state */

    AuthManager(EOS_HPlatform handle);

    /** Starts the asynchronous login process */
    void CreateDeviceId();
    void HardwareIDLogin(const EOS_Connect_CreateDeviceIdCallbackInfo* Data);
    void DevAuthToolLogin();
	void AccountPortalPersistentAuthLogin();
	
	EOS_Auth_IdToken* GetIdToken() const;
    void EpicAccountLogin();

    void Logout();

    /** Checks if the login process has successfully completed */
    bool IsLoggedIn() const;
	void SetEpicAccountId(EOS_EpicAccountId id);

    /** Gets the user's ID. Returns nullptr if not logged in. */
    EOS_ProductUserId GetLocalUserId() const;

    ~AuthManager();

private:
    // --- Member Variables ---
    // These store the state for a specific instance of the class.
	EOS_HPlatform m_PlatformHandle = nullptr;
    EOS_HAuth m_AuthHandle = nullptr;
    EOS_HConnect m_ConnectHandle = nullptr;

    EOS_EpicAccountId m_EpicId = nullptr;
    EOS_ProductUserId m_LocalUserId = nullptr;
    
    bool m_bIsLoggedIn;

    static void EOS_CALL OnCreateDeviceIdComplete(const EOS_Connect_CreateDeviceIdCallbackInfo* Data);
	static void EOS_CALL OnHardwareIDLoginComplete(const EOS_Connect_LoginCallbackInfo* Data);
	static void EOS_CALL OnDevAuthLoginComplete(const EOS_Auth_LoginCallbackInfo* Data);
    static void EOS_CALL OnAuthLogInWithPersistentAuthComplete(const EOS_Auth_LoginCallbackInfo* Data);
    static void EOS_CALL OnAuthLogInWithAccountPortalComplete(const EOS_Auth_LoginCallbackInfo* Data);
    static void EOS_CALL OnEpicLoginComplete(const EOS_Connect_LoginCallbackInfo* Data);
    static void EOS_CALL OnCreateUserComplete(const EOS_Connect_CreateUserCallbackInfo* Data);
	static void EOS_CALL OnLogoutComplete(const EOS_Auth_LogoutCallbackInfo* Data);
	static void EOS_CALL OnDeletePersistentAuthComplete(const EOS_Auth_DeletePersistentAuthCallbackInfo* Data);

    void HandleCreateDeviceId(const EOS_Connect_CreateDeviceIdCallbackInfo* Data);
    void HandleHardwareIDLogin(const EOS_Connect_LoginCallbackInfo* Data);
    void HandleDevAuthLoginComplete(const EOS_Auth_LoginCallbackInfo* Data);
    void HandlePersistentAuthLogin(const EOS_Auth_LoginCallbackInfo* Data);
    void HandleAccountPortalLogin(const EOS_Auth_LoginCallbackInfo* Data);
	void HandleEpicLoginComplete(const EOS_Connect_LoginCallbackInfo* Data);
	void HandleCreateUserComplete(const EOS_Connect_CreateUserCallbackInfo* Data);
	void HandleLogoutComplete(const EOS_Auth_LogoutCallbackInfo* Data);
	void HandleDeletePersistentAuthComplete(const EOS_Auth_DeletePersistentAuthCallbackInfo* Data);
    //void HandleHWIDLoginComplete(const EOS_Connect_LoginCallbackInfo* Data);

    void LogInWithAccountPortal();
};