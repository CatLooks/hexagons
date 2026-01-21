#include "networking/HWID.hpp"
#include <iostream>
#include <eos_connect.h>
#include <cstdlib>
#include <cstdio>
#include <networking/AuthManager.hpp>

HWID::HWID()
    : m_LocalUserId(nullptr), m_bIsLoggedIn(false){}

struct CreateDeviceIdContext
{
    HWID* Manager = nullptr;
    EOS_HConnect ConnectHandle = nullptr;
};

static void EOS_CALL OnCreateDeviceIdComplete(const EOS_Connect_CreateDeviceIdCallbackInfo* Data);

bool HWID::IsLoggedIn() const
{
    return m_bIsLoggedIn;
}

EOS_ProductUserId HWID::GetLocalUserId() const
{
    return m_LocalUserId;
}

void HWID::Login(EOS_HPlatform PlatformHandle)
{
    EOS_HConnect ConnectHandle = EOS_Platform_GetConnectInterface(PlatformHandle);

    // Step 1: Ensure a Device ID exists for this machine
    EOS_Connect_CreateDeviceIdOptions createOpts = {};
    createOpts.ApiVersion = EOS_CONNECT_CREATEDEVICEID_API_LATEST;
    CreateDeviceIdContext* Ctx = new CreateDeviceIdContext();
    Ctx->Manager = this;
	Ctx->ConnectHandle = ConnectHandle;

	//Device ID created in reference to the platform
    createOpts.DeviceModel = ID;
    std::cout << "[HWID] Creating/Ensuring Device ID..." << std::endl;
    EOS_Connect_CreateDeviceId(ConnectHandle, &createOpts, Ctx, &OnCreateDeviceIdComplete);
}

// The callback for Device ID creation. On success (or already exists), it proceeds to login.
static void EOS_CALL OnCreateDeviceIdComplete(const EOS_Connect_CreateDeviceIdCallbackInfo* Data)
{
    CreateDeviceIdContext* Ctx = static_cast<CreateDeviceIdContext*>(Data->ClientData);
    if (!Ctx || !Ctx->Manager)
    {
        std::cerr << "[HWID] Critical error: CreateDeviceId ClientData invalid." << std::endl;
        delete Ctx;
        return;
    }

    HWID* Manager = Ctx->Manager;
    EOS_HConnect ConnectHandle = Ctx->ConnectHandle;

    if (Data->ResultCode == EOS_EResult::EOS_Success || Data->ResultCode == EOS_EResult::EOS_DuplicateNotAllowed)
    {
        // Step 2: Login using Device ID
        EOS_Connect_Credentials credentials = {};
        credentials.ApiVersion = EOS_CONNECT_CREDENTIALS_API_LATEST;
        credentials.Type = EOS_EExternalCredentialType::EOS_ECT_DEVICEID_ACCESS_TOKEN;
        credentials.Token = nullptr;

        // DisplayName is required on first-time DeviceID user creation; safe to always provide
        EOS_Connect_UserLoginInfo userInfo = {};
        userInfo.ApiVersion = EOS_CONNECT_USERLOGININFO_API_LATEST;

        // Generate a simple display name for testing; persist if you want a stable name
        char displayName[64];
        std::snprintf(displayName, sizeof(displayName), "TestUser_%u", (unsigned)std::rand());
        userInfo.DisplayName = displayName;

        EOS_Connect_LoginOptions loginOpts = {};
        loginOpts.ApiVersion = EOS_CONNECT_LOGIN_API_LATEST;
        loginOpts.Credentials = &credentials;
        loginOpts.UserLoginInfo = &userInfo;

        std::cout << "[HWID] Starting Connect login with Device ID..." << std::endl;
        EOS_Connect_Login(ConnectHandle, &loginOpts, Manager, &HWID::OnLoginComplete);
    }
    else
    {
        std::cerr << "[HWID] CreateDeviceId failed: " << EOS_EResult_ToString(Data->ResultCode) << std::endl;
    }

    delete Ctx;
}

// The callback signature must match the function we called.
void EOS_CALL HWID::OnLoginComplete(const EOS_Connect_LoginCallbackInfo* Data)
{
    HWID* Manager = static_cast<HWID*>(Data->ClientData);
    if (Manager) {
        Manager->HandleLoginComplete(Data);
    }
}

void HWID::HandleLoginComplete(const EOS_Connect_LoginCallbackInfo* Data)
{
    if (Data->ResultCode == EOS_EResult::EOS_Success)
    {
        m_LocalUserId = Data->LocalUserId;
        m_bIsLoggedIn = true;
        std::cout << "[HWID] Callback: Connect login successful." << std::endl;

        // Announce to the world that we have logged in successfully!
        // Any code that is "listening" will now be executed.
        OnLoginSuccess.invoke(m_LocalUserId);
    }
    else
    {
        m_bIsLoggedIn = false;
        std::cerr << "[HWID] Callback: Connect login failed: " << EOS_EResult_ToString(Data->ResultCode) << std::endl;
        // OnLoginFailure delegate possible here
    }
}