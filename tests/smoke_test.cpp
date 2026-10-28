// Day-1 smoke test. Its job is to prove the toolchain works, and to show you
// the three Catch2 features you will use constantly: REQUIRE, SECTION, Approx.
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <string>
#include <vector>

#include "agenticpp/version.hpp"

TEST_CASE("smoke: version string is well formed") {
    const auto info = agenticpp::build_info();
    REQUIRE(!info.empty());
    REQUIRE(info.starts_with("agenticpp "));
}

TEST_CASE("smoke: sanitizer exercise is documented", "[exercise]") {
    // Intentionally NOT enabled. On day 1, uncomment the line below, build with
    // the `debug` preset, and read the ASan report. Then comment it out again.
    //
    //     int* p = new int[4];
    //     delete[] p;
    //     p[0] = 42;              // use-after-free -> ASan should abort here
    //
    // This is the exercise from STUDY_PLAN.md S0. Seeing one real ASan report
    // is worth more than a page of documentation.
    SUCCEED("exercises are documented in STUDY_PLAN.md");
}

TEST_CASE("smoke: sections re-run the test body", "[demo]") {
    // Each SECTION re-enters the TEST_CASE from the top with fresh locals.
    // This is how you get 8 test cases for the price of one setup.
    std::vector<int> v;
    int setup_runs = 0;
    ++setup_runs;

    SECTION("empty") {
        REQUIRE(v.empty());
        REQUIRE(setup_runs == 1);
    }
    SECTION("after push_back") {
        v.push_back(7);
        REQUIRE(v.size() == 1);
        REQUIRE(setup_runs == 1);
    }
}

TEST_CASE("smoke: floating point needs Approx") {
    const double a = 0.1 + 0.2;
    REQUIRE(a != 0.3);                     // true: 0.30000000000000004
    REQUIRE(a == Catch::Approx(0.3));
    REQUIRE(a == Catch::Approx(0.3).margin(1e-12));
}
