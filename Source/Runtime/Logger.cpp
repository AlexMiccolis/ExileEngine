#include <Exile/Runtime/Logger.hpp>
#include <unordered_map>
#include <utility>
#include <cstdarg>
#include <mutex>
#include <ctime>

#ifdef WIN32
#define localtime_r(ti, tm) localtime_s(tm, ti)
#endif

namespace Exi::Runtime
{
    static std::unordered_map<std::string, Logger*> s_Loggers;
    static std::mutex s_LoggersMutex;

    Logger& Logger::GetLogger(const std::string& name)
    {
        std::unique_lock lock(s_LoggersMutex);
        if (!s_Loggers.contains(name))
            s_Loggers.emplace(name, new Logger(name));
        return *s_Loggers.at(name);
    }

    void Logger::Debug(const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        Log(LevelDebug, fmt, args);
        va_end(args);
    }

    void Logger::Info(const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        Log(LevelInfo, fmt, args);
        va_end(args);
    }

    void Logger::Warn(const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        Log(LevelWarning, fmt, args);
        va_end(args);
    }

    void Logger::Error(const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        Log(LevelError, fmt, args);
        va_end(args);
    }

    void Logger::Fatal(const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        Log(LevelFatal, fmt, args);
        va_end(args);
    }

    void Logger::Log(Level level, const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        Log(level, fmt, args);
        va_end(args);
    }

    void Logger::Log(Level level, const char* fmt, va_list args)
    {
        // Write line content into line buffer
        vsnprintf(m_LineBuffer.data(), m_LineBuffer.size(), fmt, args);

        const char* bracketColor  = ColorCodes[BrightWhite];
        const char* messageColor  = ColorCodes[BrightWhite];
        const char* tagColor      = ColorCodes[BrightWhite];
        switch (level)
        {
            case LevelDebug:
                messageColor = bracketColor = ColorCodes[Green];
                tagColor = ColorCodes[BrightGreen];
                break;
            case LevelWarning:
                messageColor = bracketColor = tagColor = ColorCodes[BrightYellow];
                break;
            case LevelError:
                messageColor = bracketColor = tagColor = ColorCodes[BrightRed];
                break;
            case LevelFatal:
                messageColor = bracketColor = tagColor = ColorCodes[Red];
                break;
            default:
                break;
        }

        printf("%s[%s%s%5s%s%s]%s ",
               bracketColor, ColorCodes[Reset],
               tagColor, m_Name.c_str(), ColorCodes[Reset],
               bracketColor, ColorCodes[Reset]);

        printf("%s[%s%s%s%s%s]%s %s%s%s\n",
               bracketColor, ColorCodes[Reset],
               tagColor, LevelPrefixes[level], ColorCodes[Reset],
               bracketColor, ColorCodes[Reset],
               messageColor, m_LineBuffer.data(), ColorCodes[Reset]);
        if (m_OutputFile)
            fprintf(m_OutputFile, "[%s] %s\n", LevelPrefixes[level], m_LineBuffer.data());
    }

    Logger::Logger(const std::string& name)
        : m_Name(name)
    {
        char timeBuf[128];
        const std::time_t time = std::time(nullptr);
        std::tm tm;

        localtime_r(&time, &tm);
        std::strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d_%I-%M-%S%p", &tm);
        m_OutputPath = "Exile_" + name + ".log";
        m_OutputFile = fopen(m_OutputPath.c_str(), "w");
    }

    Logger::~Logger()
    {
        if (m_OutputFile != nullptr)
            fclose(m_OutputFile);
    }

}
