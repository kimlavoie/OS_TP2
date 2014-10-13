#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>


class Logger
{
    public:
        Logger();
        ~Logger();
        static Logger& getInstance();
        void log(std::string toLog);
    protected:
    private:
        static Logger instance;
        std::ofstream file;

};

#endif // LOGGER_H
