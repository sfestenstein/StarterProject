#include "GeneralLogger.h"

namespace CommonUtils
{

void GeneralLogger::init(const std::string &configPath)
{
    Logging::InitFromFile(configPath);
}

}