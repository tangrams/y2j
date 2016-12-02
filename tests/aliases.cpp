#include "catch.hpp"
#include "y2j.h"

// YAML 1.2 Anchors and Aliases
// http://yaml.org/spec/1.2/spec.html#id2765878

static const char* source = R"END(
anchors:
  - &0 a
  - &1 b
  - &1 c
  - &2 [α, β, γ, &delta δ]
  - &3 { a: apple }
  - { b: &4 boson }
aliases:
  - *0
  - *1
  - *2
  - *3
  - *4
  - *delta
)END";

TEST_CASE("Nodes with anchors can be repeated later using an alias", "[aliases]")
{
    const char* errorMessage = nullptr;
    size_t errorLine = 0;
    auto document = y2j::yamlParseBytes(source, strlen(source), &errorMessage, &errorLine);

    REQUIRE(errorLine == 0);
    REQUIRE(errorMessage == nullptr);

    auto& value = document["aliases"];

    REQUIRE(value.IsArray());
    REQUIRE(value.Size() == 6);

    CHECK(value[0].IsString());
    CHECK(value[1].IsString());
    CHECK(value[0].GetString() == std::string("a"));
    CHECK(value[1].GetString() == std::string("c"));

    CHECK(value[2].IsArray());
    CHECK(value[2].Size() == 4);

    CHECK(value[3].IsObject());
    CHECK(value[3].MemberCount() == 1);

    CHECK(value[4].IsString());
    CHECK(value[4].GetString() == std::string("boson"));

    CHECK(value[5].IsString());
    CHECK(value[5].GetString() == std::string("δ"));
}
