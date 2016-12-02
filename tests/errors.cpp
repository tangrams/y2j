#include "catch.hpp"
#include "y2j.h"

static const char* unresolvable0 = R"END(
unresolvable:
  - !!int 'true'
)END";

static const char* unresolvable1 = R"END(
unresolvable:
  - !!null 1
)END";

static const char* unresolvable2 = R"END(
unresolvable:
  - !!dog 10
)END";

TEST_CASE("Unresolvable tags produce parsing errors", "[errors]")
{
    SECTION("Unresolvable int")
    {
        const char* errorMessage = nullptr;
        size_t errorLine = 0;
        auto document = y2j::yamlParseBytes(unresolvable0, strlen(unresolvable0), &errorMessage, &errorLine);
        CHECK(errorLine == 2);
        CHECK_FALSE(errorMessage == nullptr);
    }

    SECTION("Unresolvable null")
    {
        const char* errorMessage = nullptr;
        size_t errorLine = 0;
        auto document = y2j::yamlParseBytes(unresolvable1, strlen(unresolvable1), &errorMessage, &errorLine);
        CHECK(errorLine == 2);
        CHECK_FALSE(errorMessage == nullptr);
    }

    SECTION("Unresolvable tag")
    {
        const char* errorMessage = nullptr;
        size_t errorLine = 0;
        auto document = y2j::yamlParseBytes(unresolvable2, strlen(unresolvable2), &errorMessage, &errorLine);
        CHECK(errorLine == 2);
        CHECK_FALSE(errorMessage == nullptr);
    }
}

static const char* alias0 = R"END(
unidentified alias:
  - &1 true
  - *2
)END";

static const char* alias1 = R"END(
alias before anchor:
  - &1 true
  - *2
  - &2 false
)END";

TEST_CASE("Unidentified aliases produce parsing errors", "[errors]")
{
    SECTION("unidentified alias")
    {
        const char* errorMessage = nullptr;
        size_t errorLine = 0;
        auto document = y2j::yamlParseBytes(alias0, strlen(alias0), &errorMessage, &errorLine);
        CHECK(errorLine == 3);
        CHECK_FALSE(errorMessage == nullptr);
    }

    SECTION("alias before anchor")
    {
        const char* errorMessage = nullptr;
        size_t errorLine = 0;
        auto document = y2j::yamlParseBytes(alias1, strlen(alias1), &errorMessage, &errorLine);
        CHECK(errorLine == 3);
        CHECK_FALSE(errorMessage == nullptr);
    }
}

static const char* syntax0 = R"END(
syntax error:
  0: { a, b, c ]
)END";

TEST_CASE("Syntax errors from libyaml produce parsing errors", "[errors]")
{
    const char* errorMessage = nullptr;
    size_t errorLine = 0;
    auto document = y2j::yamlParseBytes(syntax0, strlen(syntax0), &errorMessage, &errorLine);
    CHECK(errorLine == 2);
    CHECK_FALSE(errorMessage == nullptr);
}
