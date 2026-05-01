#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <string>

inline std::shared_ptr<spdlog::logger> getOrCreateLogger(const std::string& name) {
	auto logger = spdlog::get(name);
	return logger ? logger : spdlog::stdout_color_mt(name);
}

#endif
