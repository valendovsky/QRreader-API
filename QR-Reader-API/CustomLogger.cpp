#include "CustomLogger.h"

#include <iostream>
#include <string>
#include <chrono>

#include <restbed>


// »нициализаци€ уровн€ логировани€ (по умолчанию DEBUG)
restbed::Logger::Level CustomLogger::m_logLevel = restbed::Logger::INFO;


// ¬озвращает текущую временную метку
std::string CustomLogger::_time::getTimePoint()
{
    return std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
}

// —охран€ет сообщение, если его уровень соответствует уровню логировани€
void CustomLogger::log(const restbed::Logger::Level level, const char* format, ...)
{
    // ≈сли уровень логировани€ выше уровн€ сообщени€, не сохран€ем лог
    if (level < m_logLevel)
    {
        return;
    }

    // ќпредел€ем префикс дл€ сообщени€ логировани€
    std::string logLevel;
    switch (level)
    {
    case restbed::Logger::FATAL:
        logLevel = "FATAL]: ";
        break;
#undef ERROR
    case restbed::Logger::ERROR:
        logLevel = "ERROR]: ";
        break;

    case restbed::Logger::WARNING:
        logLevel = "WARNING]: ";
        break;

    case restbed::Logger::SECURITY:
        logLevel = "SECURITY]: ";
        break;

    case restbed::Logger::INFO:
        logLevel = "INFO]: ";
        break;
    case restbed::Logger::DEBUG:
        logLevel = "DEBUG]: ";
        break;
    default:
        logLevel = "NOTICE]: ";
    }

    va_list arguments;
    va_start(arguments, format);

    // ‘ормируем префикс (врем€ и уровень лога)
    _time timer;
    std::cerr << "[" << timer.getTimePoint() << " " << logLevel;

    vfprintf(stderr, format, arguments);
    fprintf(stderr, "\n");

    va_end(arguments);
}

// —охран€ет сообщение, если передано верное условие
void CustomLogger::log_if(bool expression, const restbed::Logger::Level level, const char* format, ...)
{
    if (expression)
    {
        va_list arguments;
        va_start(arguments, format);
        log(level, format, arguments);
        va_end(arguments);
    }
}

// ”станавливает уровень логировани€
void CustomLogger::setLogLevel(const int logging)
{
    if (logging)
    {
        // ”ровень DEBUG, логирует все сообщени€ от сервера
        m_logLevel = restbed::Logger::INFO;
    }
    else
    {
        // ”ровень логировани€ RELEASE, логирует только сообщени€ выше уровн€ DEBUG
        m_logLevel = restbed::Logger::FATAL;
    }
}

// ¬озвращает название установленного уровн€ логировани€
std::string CustomLogger::getLogLevel()
{
    return (m_logLevel ? "RELEASE" : "DEBUG");
}
