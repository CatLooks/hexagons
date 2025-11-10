#include "eos/LoggingManager.hpp"

std::ofstream LoggingManager::LogFile;

LoggingManager::LoggingManager()
{
    // Open the log file.
    LogFile.open("ExampleLogFile.log");
}

LoggingManager::~LoggingManager()
{
    // Close the log file.
    LogFile.close();
}

void EOS_CALL LoggingManager::OnLogMessageReceived(const EOS_LogMessage* InMessage)
{
    // Get the current date and time.
    std::time_t CurrentDateTime = std::time(nullptr);
    std::tm* LocalDateTime = std::localtime(&CurrentDateTime);

    // Use std::ostringstream to create a complete log that consists of the current date and 
    // time for the log, log category, log level, and log message.
    std::ostringstream CompleteLog;
    CompleteLog << std::put_time(LocalDateTime, "%Y.%m.%d-%T") << " - " << InMessage->Category
        << " - " << static_cast<int>(InMessage->Level) << " - " << InMessage->Message;

    // Print the complete log string to the console.
    std::cout << CompleteLog.str() << std::endl;

    // Save the complete log string to the log file.
    LogFile << CompleteLog.str() << std::endl;
}

void LoggingManager::RegisterLoggingCallbackExample()
{
    // Call the EOS SDK to set the passed in function as the logging callback.
    EOS_EResult SetLogCallbackResult = EOS_Logging_SetCallback(OnLogMessageReceived);

    // Check if the EOS SDK function call returned any errors.
    if (SetLogCallbackResult != EOS_EResult::EOS_Success)
    {
        std::cerr << "Error setting up logging callback! Error code: " <<
            static_cast<int>(SetLogCallbackResult) << " - " <<
            EOS_EResult_ToString(SetLogCallbackResult) << std::endl;
    }
}

void LoggingManager::SetLogLevelVeryVerboseExample()
{
    // Call the EOS SDK to set the log level for all log categories to the highest level.
    EOS_EResult SetLogLevelResult = EOS_Logging_SetLogLevel(EOS_ELogCategory::EOS_LC_ALL_CATEGORIES,
        EOS_ELogLevel::EOS_LOG_VeryVerbose);

    // Check if the EOS SDK function call returned any errors.
    if (SetLogLevelResult != EOS_EResult::EOS_Success)
    {
        std::cerr << "Error setting log level! Error code:" <<
            static_cast<int>(SetLogLevelResult) << " - " <<
            EOS_EResult_ToString(SetLogLevelResult) << std::endl;
    }
}