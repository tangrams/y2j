#include "catch.hpp"
#include "y2j.h"

// YAML 1.2 Structures
// http://www.yaml.org/spec/1.2/spec.html#id2760395

static const char* source = R"END(
? - Detroit Tigers
  - Chicago cubs
: - 2001-07-23
New York Yankees: [ 2001-07-02, 2001-08-12, 2001-08-14 ]
Atlanta Braves : [ 2001-07-02, 2001-08-12, 2001-08-14 ]
)END";

TEST_CASE("Complex mapping keys produce an 'error' node but don't halt parsing")
{
    const char* errorMessage = nullptr;
    size_t errorLine = 0;
    auto document = y2j::yamlParseBytes(source, strlen(source), &errorMessage, &errorLine);

    REQUIRE(errorMessage == nullptr);

    CHECK(document.IsObject());
    CHECK(document.MemberCount() == 3);

    auto& value0 = document["New York Yankees"];
    CHECK(value0.IsArray());
    CHECK(value0.Size() == 3);

    auto& value1 = document["Atlanta Braves"];
    CHECK(value1.IsArray());
    CHECK(value1.Size() == 3);
}
