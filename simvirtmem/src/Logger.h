#ifndef LOGGER_H
#define LOGGER_H

#include <string>


class Logger
{
    public:
        Logger();
        static Logger& getInstance();
        void log(std::string toLog);
    protected:
    private:
        static Logger instance;
};

#endif // LOGGER_H
