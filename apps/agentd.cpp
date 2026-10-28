// agentd — the daemon. Day 1: argument parsing + version only.
// Grows in week 1 (--replay) and week 2 (--config, --dry-run).
#include <cstdio>
#include <cstring>
#include <string_view>

#include "agenticpp/version.hpp"

namespace {

void print_usage(std::string_view argv0) {
    std::printf(
        "usage: %.*s [options]\n"
        "\n"
        "  --version            print build info and exit\n"
        "  --help               print this help and exit\n"
        "\n"
        "planned:\n"
        "  --config PATH        runtime config (default config/agent.json)\n"
        "  --replay PATH        replay a log file instead of tailing\n"
        "  --from-start         read the log from offset 0\n"
        "  --dry-run            never execute commands (default: on)\n"
        "  --execute            allow the policy gate to run commands\n"
        "  --yes                with --execute: unattended, no per-command confirmation\n"
        "  --no-llm             deterministic path only\n",
        static_cast<int>(argv0.size()), argv0.data());
}

}  // namespace

int main(int argc, char** argv) {
    std::string_view self = (argc > 0 && argv[0] != nullptr) ? argv[0] : "agentd";

    for (int i = 1; i < argc; ++i) {
        std::string_view arg = argv[i];
        if (arg == "--version") {
            std::printf("%.*s\n", static_cast<int>(agenticpp::build_info().size()),
                        agenticpp::build_info().data());
            return 0;
        }
        if (arg == "--help" || arg == "-h") {
            print_usage(self);
            return 0;
        }
        std::fprintf(stderr, "agentd: unknown option '%.*s'\n", static_cast<int>(arg.size()),
                     arg.data());
        print_usage(self);
        return 2;
    }

    std::printf("%.*s\n", static_cast<int>(agenticpp::build_info().size()),
                agenticpp::build_info().data());
    std::fprintf(stderr, "agentd: not implemented yet (see PLAN.md week 1, day 7)\n");
    return 1;
}
