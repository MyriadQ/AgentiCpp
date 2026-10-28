#include "agenticpp/version.hpp"

#include <string>

namespace agenticpp {
namespace {

#ifdef NDEBUG
constexpr std::string_view kFlavor = "release";
#else
constexpr std::string_view kFlavor = "debug";
#endif

}  // namespace

std::string_view build_info() noexcept {
    // Function-local static => constructed once, thread-safe since C++11.
    static const std::string text = "agenticpp " + std::string(kVersion) + " " +
                                    std::string(kFlavor);
    return text;
}

}  // namespace agenticpp
