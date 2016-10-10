#include "y2j.h"
#include <cstdio>
#include <ctime>
#include <string>

const static std::string yaml = R"END(
config:
    default: hamburgers
    fries_with_that: true
stuff:
    backup: shake
    things: [ snout, trout, clout ]
)END";

using namespace y2j;

int main(int argc, char* argv[]) {

    const char* inputString = yaml.data();
    size_t inputSize = yaml.length();

    // If an argument names a file that we can read, use it as input;
    // otherwise just use the test string.
    if (argc > 1) {
        FILE* file = fopen(argv[1], "rb");
        if (!file) {
            printf("Could not open file: %s\n", argv[0]);
        } else {
            fseek(file, 0, SEEK_END);
            size_t size = (size_t)ftell(file);
            fseek(file, 0, SEEK_SET);
            auto buffer = (char*)malloc(size);
            auto read = fread(buffer, 1, size, file);
            fclose(file);
            printf("Read %lu bytes from: %s\n", inputSize, argv[1]);
            inputString = buffer;
            inputSize = read;
        }
    }

    clock_t t0 = clock();
    const char* error = nullptr;
    size_t errorOffset = 0;
    JsonDocument document = yamlParseBytes(inputString, inputSize, &error, &errorOffset);
    clock_t t1 = clock();
    double millis = 1000.0 * (t1 - t0) / CLOCKS_PER_SEC;
    printf("Parsed in %f ms\n", millis);

    return 0;
}
