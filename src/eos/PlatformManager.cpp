#include "eos/PlatformManager.hpp"
#include "eos/Config.hpp"
#include <eos_init.h>
#include <eos_common.h>
#include <eos_sdk.h>
#include <string.h>
#include <iostream>

// PlatformManager.cpp

void PlatformManager::InitializeEOSSdk(EOSSdkConfig* Config)
{
    // Assign the required initialization options.
    EOS_InitializeOptions EOSSdkOptions = {};
    EOSSdkOptions.ApiVersion = EOS_INITIALIZE_API_LATEST;
    EOSSdkOptions.AllocateMemoryFunction = nullptr;
    EOSSdkOptions.ReallocateMemoryFunction = nullptr;
    EOSSdkOptions.ReleaseMemoryFunction = nullptr;
    EOSSdkOptions.ProductName = Config->GameName;
    EOSSdkOptions.ProductVersion = Config->GameVersion;
    EOSSdkOptions.Reserved = nullptr;
    EOSSdkOptions.SystemInitializeOptions = nullptr;
    EOSSdkOptions.OverrideThreadAffinity = nullptr;

    // Initialize the EOS SDK with the required options.
    EOS_EResult InitEOSSdkResult = EOS_Initialize(&EOSSdkOptions);

    // Print the result of the EOS SDK initialization.
    if (InitEOSSdkResult == EOS_EResult::EOS_Success)
    {
        std::cout << "[PlatformManager::InitializeEOSSdk] EOS initialization succeeded!"
            << std::endl;
    }
    else
    {
        std::cerr << "[PlatformManager::InitializeEOSSdk] EOS initialization failed with result: "
            << EOS_EResult_ToString(InitEOSSdkResult) << std::endl;
        return;
    }
}

void PlatformManager::CreatePlatformInstance(EOSSdkConfig* Config)
{
    // Assign the required options to create your platform instance using the EOS SDK credentials you configured for your game in EOSSdkConfig.h.
    double TimeoutSetting = 5;
    EOS_Platform_Options PlatformOptions = {};
    PlatformOptions.ApiVersion = EOS_PLATFORM_OPTIONS_API_LATEST;
    PlatformOptions.bIsServer = EOS_FALSE;
    PlatformOptions.EncryptionKey = Config->EncryptionKey;
    PlatformOptions.OverrideCountryCode = nullptr;
    PlatformOptions.OverrideLocaleCode = nullptr;
    PlatformOptions.ProductId = Config->ProductId;
    PlatformOptions.SandboxId = Config->SandboxId;
    PlatformOptions.DeploymentId = Config->DeploymentId;
    PlatformOptions.ClientCredentials.ClientId = Config->ClientCredentialsId;
    PlatformOptions.ClientCredentials.ClientSecret = Config->ClientCredentialsSecret;
    // This guide provides information on how to set up a simple command-line project that demonstrates the implementation of EOS services without graphics. The EOS Overlay requires graphics. The DisableOverlay flag disables the EOS Overlay to prevent additional errors and warnings in the EOS SDK logs. If you choose to implement the EOS Overlay later, you must remove this flag.
    PlatformOptions.Flags = EOS_PF_DISABLE_OVERLAY;
    PlatformOptions.TaskNetworkTimeoutSeconds = &TimeoutSetting;

    // Create your platform instance with the required options.
    PlatformHandle = new EOS_HPlatform(EOS_Platform_Create(&PlatformOptions));

    // Print the result of your platform instance creation.
    if (PlatformHandle)
    {
        std::cout << "[PlatformManager::CreatePlatformInstance] Platform instance created successfully!"
            << std::endl;
    }
    else
    {
        std::cerr << "[PlatformManager::CreatePlatformInstance] Platform instance creation failed."
            << std::endl;
        return;
    }
}