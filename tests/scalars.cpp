#include "catch.hpp"
#include "y2j.h"

// YAML 1.2 Core Schema
// http://www.yaml.org/spec/1.2/spec.html#id2805071

static const char* source = R"END(
true:
  - true
  - True
  - TRUE
false:
  - false
  - False
  - FALSE
null:
  - # Empty
  - ~
  - null
  - Null
  - NULL
string:
  - "double-quoted!"
  - 'single-quoted!'
  - unquoted!
  - 한글
  - |
    multiple
    lines
tricky: # Strings that look like other types
  - tRUE
  - FAlse
  - nUll
  - 0x
  - 0b1
  - ''
  - nan
)END";

TEST_CASE("Non-number scalars parse correctly")
{
    const char* errorMessage = nullptr;
    size_t errorLine = 0;
    auto document = y2j::yamlParseBytes(source, strlen(source), &errorMessage, &errorLine);

    REQUIRE(errorMessage == nullptr);

    SECTION("true")
    {
        auto& value = document["true"];
        REQUIRE(value.IsArray());
        REQUIRE(value.Size() == 3);
        CHECK(value[0].GetType() == rapidjson::kTrueType);
        CHECK(value[1].GetType() == rapidjson::kTrueType);
        CHECK(value[2].GetType() == rapidjson::kTrueType);
    }

    SECTION("false")
    {
        auto& value = document["false"];
        REQUIRE(value.IsArray());
        REQUIRE(value.Size() == 3);
        CHECK(value[0].GetType() == rapidjson::kFalseType);
        CHECK(value[1].GetType() == rapidjson::kFalseType);
        CHECK(value[2].GetType() == rapidjson::kFalseType);
    }

    SECTION("null")
    {
        auto& value = document["null"];
        REQUIRE(value.IsArray());
        REQUIRE(value.Size() == 5);
        CHECK(value[0].GetType() == rapidjson::kNullType);
        CHECK(value[1].GetType() == rapidjson::kNullType);
        CHECK(value[2].GetType() == rapidjson::kNullType);
        CHECK(value[3].GetType() == rapidjson::kNullType);
        CHECK(value[4].GetType() == rapidjson::kNullType);
    }

    SECTION("string")
    {
        auto& value = document["string"];
        REQUIRE(value.IsArray());
        REQUIRE(value.Size() == 5);
        CHECK(value[0].GetType() == rapidjson::kStringType);
        CHECK(value[1].GetType() == rapidjson::kStringType);
        CHECK(value[2].GetType() == rapidjson::kStringType);
        CHECK(value[3].GetType() == rapidjson::kStringType);
        CHECK(value[4].GetType() == rapidjson::kStringType);
        CHECK(value[0].GetString() == std::string("double-quoted!"));
        CHECK(value[1].GetString() == std::string("single-quoted!"));
        CHECK(value[2].GetString() == std::string("unquoted!"));
        CHECK(value[3].GetString() == std::string("한글"));
        CHECK(value[4].GetString() == std::string("multiple\nlines\n"));
    }

    SECTION("tricky")
    {
        auto& value = document["tricky"];
        REQUIRE(value.IsArray());
        REQUIRE(value.Size() == 7);
        CHECK(value[0].GetType() == rapidjson::kStringType);
        CHECK(value[1].GetType() == rapidjson::kStringType);
        CHECK(value[2].GetType() == rapidjson::kStringType);
        CHECK(value[3].GetType() == rapidjson::kStringType);
        CHECK(value[4].GetType() == rapidjson::kStringType);
        CHECK(value[5].GetType() == rapidjson::kStringType);
        // FIXME: strtod accepts several number formats outside the yaml spec.
        // CHECK(value[6].GetType() == rapidjson::kStringType);
    }
}
