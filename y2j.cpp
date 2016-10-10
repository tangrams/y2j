#include "y2j.h"
#include "yaml.h"
#include <string>
#include <vector>

#if 0
    #define DBG(stmt) stmt
#else
    #define DBG(stmt)
#endif

namespace y2j {

const char* yamlErrorStrings[] = {
    "YAML_NO_ERROR",
    "YAML_MEMORY_ERROR",
    "YAML_READER_ERROR",
    "YAML_SCANNER_ERROR",
    "YAML_PARSER_ERROR",
    "YAML_COMPOSER_ERROR",
    "YAML_WRITER_ERROR",
    "YAML_EMITTER_ERROR"
};

struct Collection {
    size_t count = 0;
    bool isMapping = false;
};

template<typename Handler>
struct Generator {

    yaml_parser_t parser;
    yaml_event_t event;
    Collection collection;
    std::vector<Collection> collectionStack;
    const char** errorMessage = nullptr;
    size_t* errorOffset = nullptr;

    size_t getSeqLength() {
        return collection.count;
    }

    size_t getMapLength() {
        return collection.count / 2;
    }

    bool entryIsMapKey() {
        return collection.isMapping && collection.count % 2 == 0;
    }

    void pushCollection(bool isMapping) {
        ++collection.count;
        collectionStack.push_back(collection);
        collection.count = 0;
        collection.isMapping = isMapping;
    }

    void popCollection() {
        collection = collectionStack.back();
        collectionStack.pop_back();
    }

    bool operator()(Handler& handler) {

        DBG(std::string indent;)
        yaml_event_type_t type = YAML_NO_EVENT;
        bool ok = true;

        do {
            if (!yaml_parser_parse(&parser, &event)) {
                *errorMessage = yamlErrorStrings[parser.error];
                *errorOffset = event.start_mark.index;
                ok = false;
                break;
            }

            type = event.type;

            switch (type) {
            case YAML_NO_EVENT:
                DBG(printf("No event!\n");)
                break;
            case YAML_STREAM_START_EVENT:
                DBG(printf("Start Stream\n");)
                break;
            case YAML_STREAM_END_EVENT:
                DBG(printf("End Stream\n");)
                break;
            case YAML_DOCUMENT_START_EVENT:
                DBG(printf("%s%s\n", indent.c_str(), "Start Document");)
                break;
            case YAML_DOCUMENT_END_EVENT:
                DBG(printf("%s%s\n", indent.c_str(), "End Document");)
                break;
            case YAML_SEQUENCE_START_EVENT:
                DBG(printf("%s[\n", indent.c_str()); indent += "    ";)
                ok = handler.StartArray();
                pushCollection(false);
                // FIXME: If a sequence is found in a map key, add a key with a string-ified sequence instead.
                break;
            case YAML_SEQUENCE_END_EVENT:
                DBG(indent.resize(indent.size() - 4); printf("%s] (members: %lu)\n", indent.c_str(), getSeqLength());)
                ok = handler.EndArray(getSeqLength());
                popCollection();
                break;
            case YAML_MAPPING_START_EVENT:
                DBG(printf("%s{\n", indent.c_str()); indent += "    ";)
                ok = handler.StartObject();
                pushCollection(true);
                // FIXME: If a mapping is found in a map key, add a key with a string-ified mapping instead.
                break;
            case YAML_MAPPING_END_EVENT:
                DBG(indent.resize(indent.size() - 4); printf("%s} (members: %lu)\n", indent.c_str(), getMapLength());)
                ok = handler.EndObject(getMapLength());
                popCollection();
                break;
            case YAML_ALIAS_EVENT:
                DBG(printf("%sGot alias (anchor %s)\n", indent.c_str(), event.data.alias.anchor);)
                // FIXME: Support aliased nodes.
                break;
            case YAML_SCALAR_EVENT:
                if (entryIsMapKey()) {
                    DBG(printf("%s\"%s\":\n", indent.c_str(), event.data.scalar.value);)
                    ok = handler.Key((char*)event.data.scalar.value, event.data.scalar.length, true);
                } else {
                    DBG(printf("%s\"%s\"\n", indent.c_str(), event.data.scalar.value);)
                    ok = handler.String((char*)event.data.scalar.value, event.data.scalar.length, true);
                    // FIXME: Deduce types for integer, float, boolean, and null scalar values.
                }
                ++collection.count;
                break;
            }

            yaml_event_delete(&event);

            if (!ok) {
                break;
            }

        } while (type != YAML_STREAM_END_EVENT);

        yaml_event_delete(&event);
        yaml_parser_delete(&parser);

        return ok;
    }
};

JsonDocument yamlParseBytes(const char* bytes, size_t length, const char** errorMessage, size_t* errorOffset) {

    Generator<JsonDocument> generator;
    generator.errorMessage = errorMessage;
    generator.errorOffset = errorOffset;

    yaml_parser_initialize(&generator.parser);
    yaml_parser_set_input_string(&generator.parser, (const unsigned char*)bytes, length);
    yaml_parser_set_encoding(&generator.parser, YAML_UTF8_ENCODING);

    JsonDocument document;
    document.Populate(generator);

    return document;
}

} // namespace y2j
