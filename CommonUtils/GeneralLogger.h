#ifndef GENERALLOGGER_H_
#define GENERALLOGGER_H_

#include "Logging/LoggerMacros.h"

#include <string>

namespace CommonUtils
{

class GeneralLogger
{
public:
    void init(const std::string &configPath);
};

}

#endif