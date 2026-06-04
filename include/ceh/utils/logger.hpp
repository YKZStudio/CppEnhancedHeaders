// ceh/utils/logger.hpp
//
// Logger —— 轻量日志（header-only）。
//
// 把日志写到任意 std::ostream（默认 std::cerr），按级别过滤，可选时间戳，
// 写入受互斥量保护（多线程安全）。无外部依赖。
//
// 级别从低到高：Trace < Debug < Info < Warn < Error < Off。
// 设定级别后，低于该级别的消息被丢弃；Off 关闭全部输出。
//
// 输出格式（关时间戳、无名字时）：  "[INFO] message\n"
//        （开时间戳）            ："2026-06-04 12:00:00 [INFO] message\n"
//        （设名字 "net"）        ： "[INFO] (net) message\n"
//
// 用法：
//     ceh::Logger log;                 // 默认输出 std::cerr，级别 Info
//     log.info("server started");
//     log.set_level(ceh::LogLevel::Warn);
//     log.debug("ignored");            // 低于 Warn，被过滤

#ifndef CEH_UTILS_LOGGER_HPP
#define CEH_UTILS_LOGGER_HPP

#include <chrono>
#include <ctime>
#include <iostream>
#include <mutex>
#include <ostream>
#include <string>

namespace ceh {

enum class LogLevel { Trace, Debug, Info, Warn, Error, Off };

namespace detail {
inline std::tm localtime_safe(std::time_t t) {
    std::tm tm{};
#if defined(_WIN32)
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    return tm;
}
}  // namespace detail

class Logger {
public:
    explicit Logger(std::ostream& out = std::cerr, LogLevel level = LogLevel::Info)
        : out_(&out), level_(level) {}

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    void set_level(LogLevel level) { level_ = level; }
    LogLevel level() const { return level_; }

    void set_output(std::ostream& out) {
        std::lock_guard<std::mutex> lock(mtx_);
        out_ = &out;
    }

    void enable_timestamp(bool on) { timestamp_ = on; }
    void set_name(const std::string& name) { name_ = name; }

    static const char* level_name(LogLevel level) {
        switch (level) {
            case LogLevel::Trace: return "TRACE";
            case LogLevel::Debug: return "DEBUG";
            case LogLevel::Info:  return "INFO";
            case LogLevel::Warn:  return "WARN";
            case LogLevel::Error: return "ERROR";
            case LogLevel::Off:   return "OFF";
        }
        return "?";
    }

    void log(LogLevel level, const std::string& message) {
        // 过滤依赖 LogLevel 的声明顺序（Trace < Debug < ... < Error < Off）：
        // 低于当前阈值的消息直接丢弃；以 Off 为级别记录也一律丢弃。
        if (level == LogLevel::Off || level < level_) return;
        std::lock_guard<std::mutex> lock(mtx_);
        if (timestamp_) (*out_) << current_timestamp() << ' ';
        (*out_) << '[' << level_name(level) << ']';
        if (!name_.empty()) (*out_) << " (" << name_ << ')';
        (*out_) << ' ' << message << '\n';
    }

    void trace(const std::string& m) { log(LogLevel::Trace, m); }
    void debug(const std::string& m) { log(LogLevel::Debug, m); }
    void info(const std::string& m)  { log(LogLevel::Info, m); }
    void warn(const std::string& m)  { log(LogLevel::Warn, m); }
    void error(const std::string& m) { log(LogLevel::Error, m); }

private:
    static std::string current_timestamp() {
        std::time_t t = std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::now());
        std::tm tm = detail::localtime_safe(t);
        char buf[20];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
        return std::string(buf);
    }

    std::ostream* out_;
    LogLevel level_;
    bool timestamp_ = false;
    std::string name_;
    std::mutex mtx_;
};

}  // namespace ceh

#endif  // CEH_UTILS_LOGGER_HPP
