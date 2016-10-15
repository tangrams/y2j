#include "y2j.h"
#include "yaml.h"
#include <string>
#include <vector>

#ifndef Y2J_DEBUG
    #define Y2J_DEBUG 1
#endif

namespace y2j {

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

        yaml_event_type_t type = YAML_NO_EVENT;
        bool ok = true;

        do {
            if (!yaml_parser_parse(&parser, &event)) {
                if (errorMessage) {
                    *errorMessage = parser.problem;
                }
                if (errorOffset) {
                    *errorOffset = parser.context_mark.index;
                }
                ok = false;
                break;
            }

            #if Y2J_DEBUG
            printEvent(event, parser.indent);
            #endif

            type = event.type;

            switch (type) {
            case YAML_NO_EVENT:
                break;
            case YAML_STREAM_START_EVENT:
                break;
            case YAML_STREAM_END_EVENT:
                break;
            case YAML_DOCUMENT_START_EVENT:
                break;
            case YAML_DOCUMENT_END_EVENT:
                break;
            case YAML_SEQUENCE_START_EVENT:
                ok = handler.StartArray();
                pushCollection(false);
                // FIXME: If a sequence is found in a map key, add a key with a string-ified sequence instead.
                break;
            case YAML_SEQUENCE_END_EVENT:
                ok = handler.EndArray(getSeqLength());
                popCollection();
                break;
            case YAML_MAPPING_START_EVENT:
                ok = handler.StartObject();
                pushCollection(true);
                // FIXME: If a mapping is found in a map key, add a key with a string-ified mapping instead.
                break;
            case YAML_MAPPING_END_EVENT:
                ok = handler.EndObject(getMapLength());
                popCollection();
                break;
            case YAML_ALIAS_EVENT:
                // FIXME: Support aliased nodes.
                break;
            case YAML_SCALAR_EVENT:
                if (entryIsMapKey()) {
                    ok = handler.Key((char*)event.data.scalar.value, event.data.scalar.length, true);
                } else {
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

    #if Y2J_DEBUG
    void printEvent(yaml_event_t& event, int indentSize) {
        std::string indent(std::max(indentSize, 0), ' ');
        switch (event.type) {
        case YAML_NO_EVENT: printf("No event!\n"); break;
        case YAML_STREAM_START_EVENT: printf("Start Stream\n"); break;
        case YAML_STREAM_END_EVENT: printf("End Stream\n"); break;
        case YAML_DOCUMENT_START_EVENT: printf("%s%s\n", indent.c_str(), "Start Document"); break;
        case YAML_DOCUMENT_END_EVENT: printf("%s%s\n", indent.c_str(), "End Document"); break;
        case YAML_SEQUENCE_START_EVENT: printf("%s[\n", indent.c_str()); break;
        case YAML_SEQUENCE_END_EVENT: printf("%s] (members: %lu)\n", indent.c_str(), getSeqLength()); break;
        case YAML_MAPPING_START_EVENT: printf("%s{\n", indent.c_str()); break;
        case YAML_MAPPING_END_EVENT: printf("%s} (members: %lu)\n", indent.c_str(), getMapLength()); break;
        case YAML_ALIAS_EVENT: printf("%sGot alias (anchor %s)\n", indent.c_str(), event.data.alias.anchor); break;
        case YAML_SCALAR_EVENT: printf(entryIsMapKey() ? "%s\"%s\":\n" : "%s\"%s\"\n", indent.c_str(), event.data.scalar.value);
            break;
        }
    }
    #endif
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
