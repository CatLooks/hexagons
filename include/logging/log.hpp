#pragma once

#include <string>

namespace logging {
	/// Redirects stdout/stderr to timestamped log files inside `logs/`.
	/// No-op on failure (still writes to console).
	void redirect_stdout_stderr();
}