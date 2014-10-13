#include "Logger.h"

Logger Logger::instance;

Logger::Logger()
{
    file.open("log.txt");
}

Logger::~Logger()
{
    file.close();
}

Logger& Logger::getInstance()
{
    return instance;
}

void Logger::log(std::string toLog)
{
    file << toLog << std::endl;
}
