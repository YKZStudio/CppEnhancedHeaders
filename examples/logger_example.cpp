// Logger 使用示例
#include <ceh/utils/logger.hpp>

#include <iostream>

int main() {
    using ceh::Logger;
    using ceh::LogLevel;

    Logger log(std::cout);          // 输出到 stdout，便于查看
    log.set_name("demo");

    log.info("程序启动");
    log.debug("这条 debug 默认级别(Info)下被过滤，不会显示");

    log.set_level(LogLevel::Trace); // 放开到最低级别
    log.debug("现在 debug 能显示了");

    log.enable_timestamp(true);
    log.warn("磁盘空间不足");
    log.error("无法写入文件");

    return 0;
}
