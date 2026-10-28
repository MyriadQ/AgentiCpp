#pragma once

#include <string_view>

namespace agenticpp {

// Bump this on every demonstrable milestone; it shows up in the audit log.
inline constexpr std::string_view kVersion = "0.1.0";

// Human-readable build description, e.g. "agenticpp 0.1.0 debug".
// Returns a view into a function-local static: valid for the process lifetime.
std::string_view build_info() noexcept;

}  // namespace agenticpp
