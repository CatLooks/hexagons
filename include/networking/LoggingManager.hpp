#pragma once

#include <eos_logging.h>
#include <eos_common.h>
#include <ctime>
#include <sstream>
#include <fstream>
#include <iostream>
#include <iomanip>

class LoggingManager
{
public:
    /** Constructor */
    LoggingManager();

    /** Destructor */
    ~LoggingManager();

    /** Registers the logging callback. */
    void RegisterLoggingCallback();

    /** Sets the log level. */
    void SetLogLevelVeryVerbose();

private:
    /** A variable that handles writing to the log file. */
    static std::ofstream LogFile;

    /**
     * Callback to print EOS log messages.
     *
     * @param InMessage - Log message from the EOS SDK.
     */
    static void EOS_CALL OnLogMessageReceived(const EOS_LogMessage* InMessage);
};