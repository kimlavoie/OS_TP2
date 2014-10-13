#include "Logger.h"

Logger Logger::instance;

Logger::Logger()
{
    //ctor
}

Logger& Logger::getInstance()
{
    return instance;
}

void Logger::log(std::string toLog)
{
    //TODO
}
