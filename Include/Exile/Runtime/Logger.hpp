#pragma once

#include <Exile/Runtime/API.hpp>
#include <string>
#include <array>

namespace Exi::Runtime
{

    class RUNTIME_API Logger
    {
    public:
        enum Level
        {
            LevelDebug,
            LevelInfo,
            LevelWarning,
            LevelError,
            LevelFatal
        };

        enum AnsiColor
        {
            Reset   = 0,
            Black   = 1,
            Red     = 2,
            Green   = 3,
            Yellow  = 4,
            Blue    = 5,
            Magenta = 6,
            Cyan    = 7,
            White   = 8,

            Gray          = 9,
            BrightRed     = 10,
            BrightGreen   = 11,
            BrightYellow  = 12,
            BrightBlue    = 13,
            BrightMagenta = 14,
            BrightCyan    = 15,
            BrightWhite   = 16,
        };

        static constexpr const char* LevelPrefixes[] =
        {
            "DEBUG",
            "INFO ",
            "WARN ",
            "ERROR",
            "FATAL"
        };

        static constexpr const char* ColorCodes[] =
        {
            "\u001b[0m",    // Reset
            "\u001b[30m",   // Black
            "\u001b[31m",   // Red
            "\u001b[32m",   // Green
            "\u001b[33m",   // Yellow
            "\u001b[34m",   // Blue
            "\u001b[35m",   // Magenta
            "\u001b[36m",   // Cyan
            "\u001b[37m",   // White
            "\u001b[30;1m", // Gray
            "\u001b[31;1m", // Bright Red
            "\u001b[32;1m", // Bright Green
            "\u001b[33;1m", // Bright Yellow
            "\u001b[34;1m", // Bright Blue
            "\u001b[35;1m", // Bright Magenta
            "\u001b[36;1m", // Bright Cyan
            "\u001b[37;1m"  // Bright White
        };

        static Logger& GetLogger(const std::string& name);

        void Log(Level level, const char* fmt, ...);
    private:
        Logger(const std::string& name = "");
        ~Logger();


        std::string m_Name;
        std::string m_OutputPath;
        FILE* m_OutputFile;
        std::array<char, 2048> m_LineBuffer;
    };
}
