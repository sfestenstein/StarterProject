#include <iostream>
#include <signal.h>
#include <thread>
#include <string>

#include "Logging/Logger.h"
#include "Logging/LoggerMacros.h"

void sigHandler(int signalNumber)
{
    GPCRIT("Signal Caught! {}", signalNumber);
    CommonUtils::Logging::Flush();
    exit(1);
}


int main(int argc, char **argv)
{
    // Catch seg faults for logging purposes, then
    // initialize the logger if a config file is present.
    signal(SIGSEGV, sigHandler);

    GPINFO("Logger is available before init");

    std::string configPath;
    for (int index = 1; index < argc; ++index)
    {
        std::string argument(argv[index]);
        if (argument == "-c")
        {
            if (index + 1 >= argc)
            {
                std::cerr << "Missing path after -c" << std::endl;
                return 1;
            }

            configPath = argv[++index];
        }
        else
        {
            std::cerr << "Unknown argument: " << argument << std::endl;
            std::cerr << "Usage: StarterProjectMain [-c path/to/config.yaml]" << std::endl;
            return 1;
        }
    }

    if (!configPath.empty())
    {
        std::string errorMessage;
        GPINFO("using config path: {}", configPath);
        if (!CommonUtils::Logging::InitFromFile(configPath, &errorMessage))
        {
            std::cerr << "Failed to load logging config: " << errorMessage << std::endl;
            GPINFO("falling back to default logger config");
        }
        else
        {
            GPINFO("successfully loaded logging config");
        }
    }
    else
    {
        GPINFO("empty config path, using startup fallback logger");
    }

    // Now let's actually do something with our program!
    GPINFO("Hello Starter Project!");

    return 0;
}