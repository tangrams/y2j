#include "y2j.h"
#include "benchmark/benchmark.h"
#include <cstdio>

static const char* fileName = "refill-style.yaml";

class YamlParseFixture : public benchmark::Fixture {

public:
    const char* data = nullptr;
    size_t length = 0;

    void SetUp(benchmark::State& st) override;
    void TearDown(benchmark::State& st) override;
};

void YamlParseFixture::SetUp(benchmark::State& st) {
    FILE* file = fopen(fileName, "rb");
    if (!file) {
        printf("Could not open file: %s\n", fileName);
    } else {
        fseek(file, 0, SEEK_END);
        size_t size = (size_t)ftell(file);
        fseek(file, 0, SEEK_SET);
        auto buffer = (char*)malloc(size);
        auto read = fread(buffer, 1, size, file);
        fclose(file);
        printf("Read %lu bytes from: %s\n", read, fileName);
        data = buffer;
        length = read;
    }
}

void YamlParseFixture::TearDown(benchmark::State& st) {
    free((void*)data);
}

BENCHMARK_DEFINE_F(YamlParseFixture, Parse)(benchmark::State& st) {

    const char* errorMessage = nullptr;
    size_t errorLine = 0;

    if (!data) {
        st.SkipWithError("Test data not loaded!");
    }

    while (st.KeepRunning()) {
        y2j::JsonDocument document = y2j::yamlParseBytes(data, length, &errorMessage, &errorLine);
    }
}

BENCHMARK_REGISTER_F(YamlParseFixture, Parse);

BENCHMARK_MAIN();
