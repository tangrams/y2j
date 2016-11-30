#include "catch.hpp"
#include "y2j.h"

// YAML 1.2 Core Schema
// http://www.yaml.org/spec/1.2/spec.html#id2805071

static const char* source = R"END(
resolvable:
  - !!bool 'true'
  - !!int '10'
  - !!float 10
  - !!str 10
)END";

TEST_CASE("Scalars with resolvable tags receive correct types")
{
    const char* errorMessage = nullptr;
    size_t errorLine = 0;
    auto document = y2j::yamlParseBytes(source, strlen(source), &errorMessage, &errorLine);

    REQUIRE(errorLine == 0);
    REQUIRE(errorMessage == nullptr);

    auto& value = document["resolvable"];

    REQUIRE(value.IsArray());
    REQUIRE(value.Size() == 4);

    CHECK(value[0].IsBool());
    CHECK(value[1].IsInt64());
    CHECK(value[2].IsDouble());
    CHECK(value[3].IsString());
}
