#include "logging/log.hpp"

#include <cstdio>
#include <filesystem>
#include <format>
#include <chrono>

#if defined(_WIN32)
#include <io.h>
#endif

namespace logging {
	static std::string now_stamp() {
		using namespace std::chrono;

		const auto now = system_clock::now();
		const auto t = system_clock::to_time_t(now);

		std::tm tm{};
#if defined(_WIN32)
		localtime_s(&tm, &t);
#else
		localtime_r(&t, &tm);
#endif

		return std::format(
			"{:04}-{:02}-{:02}_{:02}-{:02}-{:02}",
			tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
			tm.tm_hour, tm.tm_min, tm.tm_sec
		);
	}

	void redirect_stdout_stderr() {
		namespace fs = std::filesystem;

		const fs::path dir = "logs";
		std::error_code ec;
		fs::create_directories(dir, ec);

		const std::string stamp = now_stamp();
		const fs::path log_path = dir / (stamp + ".log");

		if (!std::freopen(log_path.string().c_str(), "a", stdout)) {
			return;
		}

		setvbuf(stdout, nullptr, _IONBF, 0);

#if defined(_WIN32)
		// Redirect stderr to the same underlying file descriptor as stdout.
		_dup2(_fileno(stdout), _fileno(stderr));
		setvbuf(stderr, nullptr, _IONBF, 0);
#else
		// Best-effort fallback: redirect stderr by reopening the same file.
		if (std::freopen(log_path.string().c_str(), "a", stderr)) {
			setvbuf(stderr, nullptr, _IONBF, 0);
		}
#endif
	}
}