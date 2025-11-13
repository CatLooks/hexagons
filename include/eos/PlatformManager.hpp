#pragma once

#include <eos_init.h>
#include "eos/Config.hpp"

class PlatformManager
{
    public:
        /** Constructor */
        PlatformManager() {}

        /** Deconstructor */
        ~PlatformManager() {};

        /** Getter for PlatformHandle. You use this function to access the PlatformHandle outside of the PlatformManager class. */
        EOS_HPlatform* GetPlatformHandle() const { return PlatformHandle; } 

        /** 
         * Initialize the EOS SDK.
         * 
         * @param Config - SDK configuration data from the Developer 
 * Portal.
         */
        void InitializeEOSSdk(EOSSdkConfig* Config);

        /**
         * Create the platform instance using configuration data.
         * 
         * @param Config - SDK configuration data from the Developer 
 * Portal.
         */
        void CreatePlatformInstance(EOSSdkConfig* Config);

    private:
        /** Handle to Platform Interface. */
        EOS_HPlatform* PlatformHandle = nullptr;
};