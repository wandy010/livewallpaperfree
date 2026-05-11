#pragma once

#include <string>
#include <sstream>
#include <mutex>
#include <fstream>

enum class LogLevel {
    DEBUG,
    INFO,
    WARN,
    ERR
};

class Logger {
public:
    static Logger& instance();
    void set_level(LogLevel level);
    void set_log_file(const std::string& path);

    static void debug(const std::string& message);
    static void info(const std::string& message);
    static void warn(const std::string& message);
    static void error(const std::string& message);

private:
    Logger() = default;
    ~Logger();
    void log_impl(LogLevel level, const std::string& message);
    std::string level_to_string(LogLevel level);
    std::string current_timestamp();

    LogLevel current_level_ = LogLevel::DEBUG;
    std::mutex mutex_;
    std::ofstream file_stream_;
};
