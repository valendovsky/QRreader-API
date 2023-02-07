#ifndef CUSTOMLOGGER_H
#define CUSTOMLOGGER_H

#include <iostream>
#include <string>
#include <memory>

#include <restbed>


// Логирует события сервера
class CustomLogger : public restbed::Logger
{
private:
    static restbed::Logger::Level m_logLevel;

public:
    // Вложенный класс управления временем
    class _time
    {
    public:
        _time()
        {

        }

        std::string getTimePoint();
    };

public:
    void stop(void) { return; }
    void start(const std::shared_ptr<const restbed::Settings>&) { return; }

    void log(const restbed::Logger::Level level, const char* format, ...);
    void log_if(bool expression, const restbed::Logger::Level level, const char* format, ...);

    void setLogLevel(const int logging);
    std::string getLogLevel();

};

#endif // !CUSTOMLOGGER_H
