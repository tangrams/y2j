#include "catch.hpp"
#include "y2j.h"
#include <cmath>

// YAML 1.2 Core Schema
// http://www.yaml.org/spec/1.2/spec.html#id2805071

static const char* source = R"END(
floats:
  - 685230.15
  - 6.8523015e+5
  - -685.23015e+03
integers:
  - +68523015
  - -373
  - 0x3A
  - 0o100
infinity:
  - .inf
  - .Inf
  - .INF
  - +.Inf
  - -.Inf
nan:
  - .nan
  - .NaN
  - .NAN
)END";

TEST_CASE("Numbers parse correctly")
{
    const char* errorMessage = nullptr;
    size_t errorLine = 0;
    auto document = y2j::yamlParseBytes(source, strlen(source), &errorMessage, &errorLine);

    REQUIRE(errorMessage == nullptr);

    SECTION("floats")
    {
        auto& value = document["floats"];
        REQUIRE(value.IsArray());
        REQUIRE(value.Size() == 3);
        CHECK(value[0].IsDouble());
        CHECK(value[1].IsDouble());
        CHECK(value[2].IsDouble());
        CHECK(value[0].GetDouble() == 685230.15);
        CHECK(value[1].GetDouble() == 685230.15);
        CHECK(value[2].GetDouble() == -685230.15);
    }

    SECTION("integers")
    {
        auto& value = document["integers"];
        REQUIRE(value.IsArray());
        REQUIRE(value.Size() == 4);
        CHECK(value[0].IsInt64());
        CHECK(value[1].IsInt64());
        CHECK(value[2].IsInt64());
        CHECK(value[3].IsInt64());
        CHECK(value[0].GetInt64() == 68523015);
        CHECK(value[1].GetInt64() == -373);
        CHECK(value[2].GetInt64() == 58);
        CHECK(value[3].GetInt64() == 64);
    }

    SECTION("infinity")
    {
        auto& value = document["infinity"];
        REQUIRE(value.IsArray());
        CHECK(value[0].IsDouble());
        CHECK(value[1].IsDouble());
        CHECK(value[2].IsDouble());
        CHECK(value[3].IsDouble());
        CHECK(value[4].IsDouble());
        CHECK(std::isinf(value[0].GetDouble()));
        CHECK(std::isinf(value[1].GetDouble()));
        CHECK(std::isinf(value[2].GetDouble()));
        CHECK(std::isinf(value[3].GetDouble()));
        CHECK(std::isinf(value[4].GetDouble()));
        CHECK(!std::signbit(value[0].GetDouble()));
        CHECK(!std::signbit(value[1].GetDouble()));
        CHECK(!std::signbit(value[2].GetDouble()));
        CHECK(!std::signbit(value[3].GetDouble()));
        CHECK(std::signbit(value[4].GetDouble()));
    }

    SECTION("nan")
    {
        auto& value = document["nan"];
        REQUIRE(value.IsArray());
        CHECK(std::isnan(value[0].GetDouble()));
        CHECK(std::isnan(value[1].GetDouble()));
        CHECK(std::isnan(value[2].GetDouble()));
    }

}

