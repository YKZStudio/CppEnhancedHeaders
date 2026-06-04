// Logger 单元测试（零依赖，基于 assert）
//
// 把日志输出到 std::ostringstream，断言内容；时间戳默认关闭以保证确定性。
#include <ceh/utils/logger.hpp>

#include <iostream>
#include <sstream>
#include <string>

using ceh::Logger;
using ceh::LogLevel;

static int g_checks = 0;

#define CHECK_TRUE(expr)                                               \
    do {                                                               \
        ++g_checks;                                                    \
        if (!(expr)) {                                                 \
            std::cerr << "FAIL [" << __LINE__ << "] " #expr "\n";      \
            std::exit(1);                                              \
        }                                                              \
    } while (0)

#define CHECK_EQ_STR(got, expected)                                    \
    do {                                                               \
        ++g_checks;                                                    \
        std::string g = (got);                                        \
        std::string e = (expected);                                   \
        if (g != e) {                                                  \
            std::cerr << "FAIL [" << __LINE__ << "] got [" << g        \
                      << "] expected [" << e << "]\n";                 \
            std::exit(1);                                              \
        }                                                              \
    } while (0)

int main() {
    // 默认级别 Info：debug/trace 被过滤，info 及以上输出
    {
        std::ostringstream os;
        Logger log(os);
        log.trace("t");
        log.debug("d");
        log.info("hello");
        log.warn("careful");
        log.error("boom");
        CHECK_EQ_STR(os.str(),
                     "[INFO] hello\n[WARN] careful\n[ERROR] boom\n");
    }

    // 调整级别为 Warn：info 也被过滤
    {
        std::ostringstream os;
        Logger log(os, LogLevel::Warn);
        log.info("ignored");
        log.warn("kept");
        CHECK_EQ_STR(os.str(), "[WARN] kept\n");
    }

    // 级别为 Trace：全部输出
    {
        std::ostringstream os;
        Logger log(os, LogLevel::Trace);
        log.trace("a");
        log.debug("b");
        CHECK_EQ_STR(os.str(), "[TRACE] a\n[DEBUG] b\n");
    }

    // 级别为 Off：什么都不输出
    {
        std::ostringstream os;
        Logger log(os, LogLevel::Off);
        log.error("nope");
        CHECK_EQ_STR(os.str(), "");
    }

    // 带名字前缀
    {
        std::ostringstream os;
        Logger log(os);
        log.set_name("net");
        log.info("connected");
        CHECK_EQ_STR(os.str(), "[INFO] (net) connected\n");
    }

    // level() / set_level / level_name
    {
        Logger log(std::cerr, LogLevel::Info);
        CHECK_TRUE(log.level() == LogLevel::Info);
        log.set_level(LogLevel::Error);
        CHECK_TRUE(log.level() == LogLevel::Error);
        CHECK_TRUE(std::string(Logger::level_name(LogLevel::Warn)) == "WARN");
    }

    // set_output 切换目标流
    {
        std::ostringstream a, b;
        Logger log(a);
        log.info("to-a");
        log.set_output(b);
        log.info("to-b");
        CHECK_EQ_STR(a.str(), "[INFO] to-a\n");
        CHECK_EQ_STR(b.str(), "[INFO] to-b\n");
    }

    // 开启时间戳后，行尾消息仍在，且包含级别标签（不校验具体时间）
    {
        std::ostringstream os;
        Logger log(os);
        log.enable_timestamp(true);
        log.info("ts");
        std::string out = os.str();
        CHECK_TRUE(out.find("[INFO] ts\n") != std::string::npos);
        CHECK_TRUE(out.size() > std::string("[INFO] ts\n").size());  // 多出时间戳
    }

    std::cout << "All Logger tests passed (" << g_checks << " checks).\n";
    return 0;
}
