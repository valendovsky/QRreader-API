#include "CustomLogger.h"

#include <iostream>
#include <string>
#include <chrono>

#include <restbed>


// ������������� ������ ����������� (�� ��������� DEBUG)
restbed::Logger::Level CustomLogger::m_logLevel = restbed::Logger::INFO;


// ���������� ������� ��������� �����
std::string CustomLogger::_time::getTimePoint()
{
    return std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
}

// ��������� ���������, ���� ��� ������� ������������� ������ �����������
void CustomLogger::log(const restbed::Logger::Level level, const char* format, ...)
{
    // ���� ������� ����������� ���� ������ ���������, �� ��������� ���
    if (level < m_logLevel)
    {
        return;
    }

    // ���������� ������� ��� ��������� �����������
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

    // ��������� ������� (����� � ������� ����)
    _time timer;
    std::cerr << "[" << timer.getTimePoint() << " " << logLevel;

    vfprintf(stderr, format, arguments);
    fprintf(stderr, "\n");

    va_end(arguments);
}

// ��������� ���������, ���� �������� ������ �������
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

// ������������� ������� �����������
void CustomLogger::setLogLevel(const int logging)
{
    if (logging)
    {
        // ������� DEBUG, �������� ��� ��������� �� �������
        m_logLevel = restbed::Logger::INFO;
    }
    else
    {
        // ������� ����������� RELEASE, �������� ������ ��������� ���� ������ DEBUG
        m_logLevel = restbed::Logger::FATAL;
    }
}

// ���������� �������� �������������� ������ �����������
std::string CustomLogger::getLogLevel()
{
    return (m_logLevel ? "RELEASE" : "DEBUG");
}
