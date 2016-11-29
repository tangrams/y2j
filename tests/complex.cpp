#include "catch.hpp"
#include "y2j.h"

// YAML 1.2 Structures
// http://www.yaml.org/spec/1.2/spec.html#id2760395

static const char* source = R"END(
? - complex: key
: - value
[ another, complex, key ]: [ more, values ]
simple key: [ even, more, values ]
)END";

TEST_CASE("Complex mapping keys produce an 'error' node but don't halt parsing")
{
    const char* errorMessage = nullptr;
    size_t errorLine = 0;
    auto document = y2j::yamlParseBytes(source, strlen(source), &errorMessage, &errorLine);

    REQUIRE(errorMessage == nullptr);

    CHECK(document.IsObject());
    CHECK(document.MemberCount() == 3);

    auto& value = document["simple key"];
    CHECK(value.IsArray());
    CHECK(value.Size() == 3);
}
