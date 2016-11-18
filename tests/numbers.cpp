#include "catch.hpp"
#include "y2j.h"
#include <math.h>

// YAML 1.2 Core Schema
// http://www.yaml.org/spec/1.2/spec.html#id2805071

const char* source = R"END(
canonical: 6.8523015e+5
exponential: 685.23015e+03
fixed: 685230.15
negative infinity: -.inf
not a number: .NaN
)END";

TEST_CASE("Floating point numbers parse correctly")
{
    const char* errorMessage = nullptr;
    size_t errorLine = 0;
    auto document = y2j::yamlParseBytes(source, strlen(source), &errorMessage, &errorLine);

    REQUIRE(errorMessage == nullptr);

    SECTION("Canonical form")
    {
        auto& value = document["canonical"];
        REQUIRE(value.IsDouble());
        CHECK(value.GetDouble() == 685230.15);
    }

    SECTION("Exponential form")
    {
        auto& value = document["exponential"];
        REQUIRE(value.IsDouble());
        CHECK(value.GetDouble() == 685230.15);
    }

    SECTION("Fixed decimal form")
    {
        auto& value = document["fixed"];
        REQUIRE(value.IsDouble());
        CHECK(value.GetDouble() == 685230.15);
    }

    SECTION("Negative infinity")
    {
        auto& value = document["negative infinity"];
        REQUIRE(value.IsDouble());
        CHECK(isinf(value.GetDouble()));
        CHECK(signbit(value.GetDouble()));
    }

    SECTION("Not-a-number")
    {
        auto& value = document["not a number"];
        REQUIRE(value.IsDouble());
        CHECK(isnan(value.GetDouble()));
    }

}

