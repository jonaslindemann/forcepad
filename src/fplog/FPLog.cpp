#include "FPLog.h"
#include <spdlog/sinks/stdout_color_sinks.h>

namespace FPLog {

void init(std::vector<spdlog::sink_ptr> extra_sinks)
{
    std::vector<spdlog::sink_ptr> sinks;

    auto console = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console->set_level(spdlog::level::debug);
    sinks.push_back(console);

    for (auto& s : extra_sinks)
        sinks.push_back(s);

    auto logger = std::make_shared<spdlog::logger>("forcepad", sinks.begin(), sinks.end());
    logger->set_level(spdlog::level::debug);
    spdlog::set_default_logger(logger);
    spdlog::set_pattern("[%H:%M:%S.%e] [%^%l%$] %v");
}

void setLevel(spdlog::level::level_enum level)
{
    spdlog::set_level(level);
}

}
