#include "networking_remake/AuthManager.hpp"

AuthManager::AuthManager(EOS_HPlatform handle): m_PlatformHandle(handle), m_LocalUserId(nullptr), m_bIsLoggedIn(false) {
	m_AuthHandle = EOS_Platform_GetAuthInterface(m_PlatformHandle);
	m_ConnectHandle = EOS_Platform_GetConnectInterface(m_PlatformHandle);
}

void AuthManager::SetEpicAccountId(EOS_EpicAccountId id) {
    m_EpicId = id;
}

EOS_ProductUserId AuthManager::GetLocalUserId() const {
    return m_LocalUserId;
}

void AuthManager::CreateDeviceId() {
    // Step 1: Ensure a Device ID exists for this machine
    EOS_Connect_CreateDeviceIdOptions createOpts = {};
    createOpts.ApiVersion = EOS_CONNECT_CREATEDEVICEID_API_LATEST;

    //Device ID created in reference to the platform
    createOpts.DeviceModel = ID;
    std::cout << "[AuthManager] Creating/Ensuring Device ID..." << std::endl;
    EOS_Connect_CreateDeviceId(m_ConnectHandle, &createOpts, this, &OnCreateDeviceIdComplete);
}

void AuthManager::OnCreateDeviceIdComplete(const EOS_Connect_CreateDeviceIdCallbackInfo* Data) {
    if (auto Manager = static_cast<AuthManager*>(Data->ClientData)) {
        Manager->HardwareIDLogin(Data);
    }
}

void AuthManager::HardwareIDLogin(const EOS_Connect_CreateDeviceIdCallbackInfo* Data) {
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

        std::cout << "[AuthManager] Starting Connect login with Device ID..." << std::endl;
        EOS_Connect_Login(m_ConnectHandle, &loginOpts, this, OnHardwareIDLoginComplete);
    }
    else
    {
        std::cerr << "[AuthManager] CreateDeviceId failed: " << EOS_EResult_ToString(Data->ResultCode) << std::endl;
    }
}

void EOS_CALL AuthManager::OnHardwareIDLoginComplete(const EOS_Connect_LoginCallbackInfo* Data) {
    if (auto Manager = static_cast<AuthManager*>(Data->ClientData)) {
        Manager->HandleHardwareIDLogin(Data);
    }
}

void AuthManager::HandleHardwareIDLogin(const EOS_Connect_LoginCallbackInfo* Data) {

    if (Data->ResultCode == EOS_EResult::EOS_Success)
    {
        m_LocalUserId = Data->LocalUserId;
        m_bIsLoggedIn = true;
        std::cout << "[AuthManager] Callback: Connect login successful." << std::endl;

        // Announce to the world that we have logged in successfully!
        // Any code that is "listening" will now be executed.
        OnLoginSuccess.Broadcast(m_LocalUserId);
    }
    else
    {
        m_bIsLoggedIn = false;
        std::cerr << "[AuthManager] Callback: Connect login failed: " << EOS_EResult_ToString(Data->ResultCode) << std::endl;
        // OnLoginFailure delegate possible here
    }
}

void AuthManager::DevAuthToolLogin() {
    // Set the credentials for the Dev Auth Tool. 
    EOS_Auth_Credentials Credentials = {};
    Credentials.ApiVersion = EOS_AUTH_CREDENTIALS_API_LATEST;
    Credentials.Type = EOS_ELoginCredentialType::EOS_LCT_Developer;
    Credentials.Token = "test";
    Credentials.Id = "localhost:26900";

    // Set the options for logging in.
    EOS_Auth_LoginOptions LoginOptions = {};
    LoginOptions.ApiVersion = EOS_AUTH_LOGIN_API_LATEST;
    LoginOptions.ScopeFlags = EOS_EAuthScopeFlags::EOS_AS_BasicProfile | EOS_EAuthScopeFlags::EOS_AS_Presence;
    LoginOptions.Credentials = &Credentials;

    // Log into the player's Epic Games account using the Auth Interface.
    EOS_Auth_Login(m_AuthHandle, &LoginOptions, this, OnDevAuthLoginComplete);
}

void EOS_CALL AuthManager::OnDevAuthLoginComplete(const EOS_Auth_LoginCallbackInfo* Data) {
    if (auto Manager = static_cast<AuthManager*>(Data->ClientData)) {
        Manager->HandleDevAuthLoginComplete(Data);
    }
}

void AuthManager::HandleDevAuthLoginComplete(const EOS_Auth_LoginCallbackInfo* Data) {
    if (Data->ResultCode == EOS_EResult::EOS_Success)
    {
        std::cout << "[AuthManager] Logged in with the Dev Auth Tool successfully!" << std::endl;

        // Retrieve the AuthManager pointer from the client data passed into the callback function.
        AuthManager* Manager = static_cast<AuthManager*>(Data->ClientData);

        // Retrieve the Epic Account ID.
		Manager->SetEpicAccountId(Data->LocalUserId);
        Manager->EpicAccountLogin();
    }
    else if (EOS_EResult_IsOperationComplete(Data->ResultCode) == EOS_FALSE)
    {
        // If the code gets here, the operation is retrying and is not yet complete.
        return;
    }
    else
    {
        // If the code gets here, there was an error.
        // Handle this however you need for your game.
        std::cerr << "[AuthManager] Failed to log in with the Dev Auth Tool. Error code: " <<
            static_cast<int>(Data->ResultCode) << " - " << EOS_EResult_ToString(Data->ResultCode) << std::endl;
    }
}

void AuthManager::AccountPortalPersistentAuthLogin() {
    // Set the credentials for persistent authorization.
    EOS_Auth_Credentials Credentials = {};
    Credentials.ApiVersion = EOS_AUTH_CREDENTIALS_API_LATEST;
    Credentials.Type = EOS_ELoginCredentialType::EOS_LCT_PersistentAuth;
    Credentials.Token = nullptr;
    Credentials.Id = nullptr;

    // Set the options for logging in.
    EOS_Auth_LoginOptions LoginOptions = {};
    LoginOptions.ApiVersion = EOS_AUTH_LOGIN_API_LATEST;
    LoginOptions.ScopeFlags = EOS_EAuthScopeFlags::EOS_AS_BasicProfile | EOS_EAuthScopeFlags::EOS_AS_Presence;
    LoginOptions.Credentials = &Credentials;

    // Log into an Epic Games account using the Auth Interface.
    EOS_Auth_Login(m_AuthHandle, &LoginOptions, this, OnAuthLogInWithPersistentAuthComplete);
}

void EOS_CALL AuthManager::OnAuthLogInWithPersistentAuthComplete(const EOS_Auth_LoginCallbackInfo* Data) {
    if (auto Manager = static_cast<AuthManager*>(Data->ClientData)) {
        Manager->HandlePersistentAuthLogin(Data);
    }
}

void AuthManager::HandlePersistentAuthLogin(const EOS_Auth_LoginCallbackInfo* Data) {
    if (Data->ResultCode == EOS_EResult::EOS_Success)
    {
        std::cout << "Logged in with persistent authorization successfully!" << std::endl;

        // Retrieve the AuthManager pointer from the client data passed into the callback function.
        AuthManager* Manager = static_cast<AuthManager*>(Data->ClientData);

        // Retrieve the Epic Account ID. 
		Manager->SetEpicAccountId(Data->LocalUserId);
		Manager->EpicAccountLogin();
    }
    else if (EOS_EResult_IsOperationComplete(Data->ResultCode) == EOS_FALSE)
    {
        // If the code gets here, the operation is retrying and is not yet complete.
        return;
    }
    else
    {
        // If the code gets here, there was an error;
        // attempt logging in using the account portal instead.
        std::cerr << "Failed to log in with persistent authorization. Attempting log in through Account Portal. Error code: "
            << static_cast<int>(Data->ResultCode) << " - " << EOS_EResult_ToString(Data->ResultCode) << std::endl;

        // Retrieve the AuthManager pointer from the client data passed into the callback function.
        AuthManager* Self = static_cast<AuthManager*>(Data->ClientData);

        // If logging in with persistent authorization didn't work, try logging in through the Account Portal instead.
        Self->LogInWithAccountPortal();
    }
}

void AuthManager::LogInWithAccountPortal() {
    //  Set the credentials for the Account Portal.
    EOS_Auth_Credentials Credentials = {};
    Credentials.ApiVersion = EOS_AUTH_CREDENTIALS_API_LATEST;
    Credentials.Type = EOS_ELoginCredentialType::EOS_LCT_AccountPortal;
    Credentials.Token = nullptr;
    Credentials.Id = nullptr;

    // Set the options for logging in.
    EOS_Auth_LoginOptions LoginOptions = {};
    LoginOptions.ApiVersion = EOS_AUTH_LOGIN_API_LATEST;
    LoginOptions.ScopeFlags = EOS_EAuthScopeFlags::EOS_AS_BasicProfile | EOS_EAuthScopeFlags::EOS_AS_Presence;
    LoginOptions.Credentials = &Credentials;

    // Log into an Epic Games account using the Auth Interface.
    EOS_Auth_Login(m_AuthHandle, &LoginOptions, this, OnAuthLogInWithAccountPortalComplete);
}

void EOS_CALL AuthManager::OnAuthLogInWithAccountPortalComplete(const EOS_Auth_LoginCallbackInfo* Data) {
    if (auto Manager = static_cast<AuthManager*>(Data->ClientData)) {
        Manager->HandleAccountPortalLogin(Data);
    }
}

void AuthManager::HandleAccountPortalLogin(const EOS_Auth_LoginCallbackInfo* Data) {
    if (Data->ResultCode == EOS_EResult::EOS_Success)
    {
        std::cout << "Logged in through the Account Portal successfully!" << std::endl;

        // Retrieve the AuthManager pointer from the client data passed into the callback function.
        AuthManager* Manager = static_cast<AuthManager*>(Data->ClientData);

        // Retrieve the Epic Account ID.
		Manager->SetEpicAccountId(Data->LocalUserId);
		Manager->EpicAccountLogin();
    }
    else if (EOS_EResult_IsOperationComplete(Data->ResultCode) == EOS_FALSE)
    {
        // If the code gets here, the operation is retrying and is not yet complete.
        return;
    }
    else
    {
        // If the code gets here, there was an error.
        // Handle this however you need for your game.
        std::cerr << "Failed to log in through the Account Portal. Error code: " << static_cast<int>(Data->ResultCode) <<
            " - " << EOS_EResult_ToString(Data->ResultCode) << std::endl;
    }
}

EOS_Auth_IdToken* AuthManager::GetIdToken() const {
    // Create a pointer to store the ID Token.
    EOS_Auth_IdToken* IdToken = nullptr;

    // Set the options for copying the ID Token.
    EOS_Auth_CopyIdTokenOptions CopyTokenOptions = {};
    CopyTokenOptions.ApiVersion = EOS_AUTH_COPYIDTOKEN_API_LATEST;
    CopyTokenOptions.AccountId = m_EpicId;

    // Call the EOS SDK to copy the ID Token to the ID Token pointer.
    EOS_EResult Result = EOS_Auth_CopyIdToken(m_AuthHandle, &CopyTokenOptions, &IdToken);
    if (Result == EOS_EResult::EOS_Success)
    {
        std::cout << "Successfully copied ID Token!" << std::endl;
        return IdToken;
    }
    else
    {
        std::cerr << "Failed to copy ID Token! Error code: "
            << static_cast<int>(Result) << " - " << EOS_EResult_ToString(Result) << std::endl;
        return nullptr;
    }
}

void AuthManager::EpicAccountLogin() {
    {
        // Get the ID Token from the Auth Interface.
        EOS_Auth_IdToken* IdToken = GetIdToken();

        // Set the credentials using a valid ID Token.
        EOS_Connect_Credentials Credentials;
        Credentials.ApiVersion = EOS_CONNECT_CREDENTIALS_API_LATEST;
        Credentials.Token = IdToken->JsonWebToken;
        Credentials.Type = EOS_EExternalCredentialType::EOS_ECT_EPIC_ID_TOKEN;

        // Set the options for logging in.
        EOS_Connect_LoginOptions LoginOptions = {};
        LoginOptions.ApiVersion = EOS_CONNECT_LOGIN_API_LATEST;
        LoginOptions.Credentials = &Credentials;
        LoginOptions.UserLoginInfo = nullptr;

        // Log in using the Connect Interface to access EOS Game Services.
        EOS_Connect_Login(m_ConnectHandle, &LoginOptions, this, OnEpicLoginComplete);

        // Release the memory for the ID Token when done with it.
        EOS_Auth_IdToken_Release(IdToken);
    }
};

void EOS_CALL AuthManager::OnEpicLoginComplete(const EOS_Connect_LoginCallbackInfo* Data)
{
    if (auto Manager = static_cast<AuthManager*>(Data->ClientData)) {
        Manager->HandleEpicLoginComplete(Data);
    }
}
void AuthManager::HandleEpicLoginComplete(const EOS_Connect_LoginCallbackInfo* Data) {
    if (Data->ResultCode == EOS_EResult::EOS_Success)
    {
        std::cout << "Logged in with ID token successfully!" << std::endl;

        // Retrieve the ConnectManager pointer from the client data passed into the callback function.
        AuthManager* Manager = static_cast<AuthManager*>(Data->ClientData);

        // Retrieve the logged-in local player's Product User ID.
		Manager->m_LocalUserId = Data->LocalUserId;
        Manager->m_bIsLoggedIn = true;
        // Announce to the world that we have logged in successfully!
        // Any code that is "listening" will now be executed.
		Manager->OnLoginSuccess.Broadcast(Manager->m_LocalUserId);
    }
    else if (Data->ResultCode == EOS_EResult::EOS_InvalidUser)
    {
        // If the code gets here, the local player has no existing Product User ID, so you need to create a new one.
        // If your game is cross-platform, add alternate identity provider login flow here.
        std::cout << "Creating Product User ID for new local player login." << std::endl;

        // Set the options for creating a new Product User ID for the local player.
        EOS_Connect_CreateUserOptions CreateUserOptions;
        CreateUserOptions.ApiVersion = EOS_CONNECT_CREATEUSER_API_LATEST;
        CreateUserOptions.ContinuanceToken = Data->ContinuanceToken;

        // Retrieve the ConnectManager pointer from the client data passed into the callback function.
        AuthManager* Manager = static_cast<AuthManager*>(Data->ClientData);

        // Create a new Product User ID for the local player.
        EOS_Connect_CreateUser(Manager->m_ConnectHandle, &CreateUserOptions, Manager, OnCreateUserComplete);
    }
    else if (EOS_EResult_IsOperationComplete(Data->ResultCode) == EOS_FALSE)
    {
        // If the code gets here, the operation is retrying and is not yet complete.
        return;
    }
    else
    {
        // If the code gets here, there was an error.
        // Handle this however you need for your game.
        std::cerr << "Failed to log in with ID token. Error code: "
            << static_cast<int>(Data->ResultCode) << " - " << EOS_EResult_ToString(Data->ResultCode) << std::endl;
    }
}

void EOS_CALL AuthManager::OnCreateUserComplete(const EOS_Connect_CreateUserCallbackInfo* Data) {
    if (auto Manager = static_cast<AuthManager*>(Data->ClientData)) {
        Manager->HandleCreateUserComplete(Data);
    }
}

void AuthManager::HandleCreateUserComplete(const EOS_Connect_CreateUserCallbackInfo* Data) {
    if (Data->ResultCode == EOS_EResult::EOS_Success)
    {
        std::cout << "New Product User ID created for the local player!" << std::endl;

        // Retrieve the ConnectManager pointer from the client data passed into the callback function.
        AuthManager* Manager = static_cast<AuthManager*>(Data->ClientData);

        // Retrieve the local player's newly created Product User ID.
        Manager->m_LocalUserId = Data->LocalUserId;
        Manager->m_bIsLoggedIn = true;
        // Announce to the world that we have logged in successfully!
        // Any code that is "listening" will now be executed.
        Manager->OnLoginSuccess.Broadcast(Manager->m_LocalUserId);
    }
    else if (EOS_EResult_IsOperationComplete(Data->ResultCode) == EOS_FALSE)
    {
        // If the code gets here, the operation is retrying and is not yet complete.
        return;
    }
    else
    {
        // If the code gets here, there was an error.
        // Handle this however you need for your game.
        std::cerr << "Failed to create Product User ID for the local player. Error code: "
            << static_cast<int>(Data->ResultCode) << " - " << EOS_EResult_ToString(Data->ResultCode) << std::endl;
    }
}

void AuthManager::Logout() {
    EOS_Auth_LogoutOptions LogoutOptions = {};
    LogoutOptions.ApiVersion = EOS_AUTH_LOGOUT_API_LATEST;
    LogoutOptions.LocalUserId = m_EpicId;
    EOS_Auth_Logout(m_AuthHandle, &LogoutOptions, this, OnLogoutComplete);
}

void EOS_CALL AuthManager::OnLogoutComplete(const EOS_Auth_LogoutCallbackInfo* Data) {
    if (auto Manager = static_cast<AuthManager*>(Data->ClientData)) {
        Manager->HandleLogoutComplete(Data);
    }
}

void AuthManager::HandleLogoutComplete(const EOS_Auth_LogoutCallbackInfo* Data) {
    if (Data->ResultCode == EOS_EResult::EOS_Success)
    {
        std::cout << "[AuthManager] Logged out successfully!" << std::endl;
        EOS_Auth_DeletePersistentAuthOptions options = {};
        options.ApiVersion = EOS_AUTH_DELETEPERSISTENTAUTH_API_LATEST;
        options.RefreshToken = NULL;

        EOS_Auth_DeletePersistentAuth(m_AuthHandle, &options, this, OnDeletePersistentAuthComplete);
    }
    else
    {
        std::cerr << "[AuthManager] Logout failed: " << EOS_EResult_ToString(Data->ResultCode) << std::endl;
    }
}

void EOS_CALL AuthManager::OnDeletePersistentAuthComplete(const EOS_Auth_DeletePersistentAuthCallbackInfo* Data) {
    if (auto Manager = static_cast<AuthManager*>(Data->ClientData)) {
        Manager->HandleDeletePersistentAuthComplete(Data);
    }
}

void AuthManager::HandleDeletePersistentAuthComplete(const EOS_Auth_DeletePersistentAuthCallbackInfo* Data) {
    if (Data->ResultCode == EOS_EResult::EOS_Success)
    {
        std::cout << "[AuthManager] Deleted persistent auth successfully!" << std::endl;
    }
    else
    {
        std::cerr << "[AuthManager] Delete persistent auth failed: " << EOS_EResult_ToString(Data->ResultCode) << std::endl;
    }
}


AuthManager::~AuthManager() {};