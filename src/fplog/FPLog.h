#pragma once
#include <spdlog/spdlog.h>
#include <memory>
#include <vector>

// fmt-style level macros — ctx must be a string literal, fmt is the format string
// e.g. fp_info("FEM", "loop = {}, change = {}", loop, change)
#define fp_debug(ctx, fmt, ...) spdlog::debug("[" ctx "] " fmt, ##__VA_ARGS__)
#define fp_info(ctx, fmt, ...)  spdlog::info( "[" ctx "] " fmt, ##__VA_ARGS__)
#define fp_warn(ctx, fmt, ...)  spdlog::warn( "[" ctx "] " fmt, ##__VA_ARGS__)
#define fp_error(ctx, fmt, ...) spdlog::error("[" ctx "] " fmt, ##__VA_ARGS__)

// Drop-in replacements for the old so_print family — work in both FLTK and Qt builds.
// Guard against redefinition when LogWindow.h (FLTK build) is included first.
#ifndef so_print
#define so_print(ctx, msg)  spdlog::info("[{}] {}", ctx, msg)
#define so_println()        ((void)0)
#define so_show()           ((void)0)
#define so_hide()           ((void)0)
#endif

namespace FPLog {
    void init(std::vector<spdlog::sink_ptr> extra_sinks = {});
    void setLevel(spdlog::level::level_enum level);
}
