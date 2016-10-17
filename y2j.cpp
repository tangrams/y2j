#include "y2j.h"
#include "yaml.h"
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

    Generator(const char* bytes, size_t length, const char** errorMessage, size_t* errorOffset) :
        errorMessage(errorMessage),
        errorOffset(errorOffset) {
        yaml_parser_initialize(&parser);
        yaml_parser_set_input_string(&parser, (const unsigned char*)bytes, length);
        yaml_parser_set_encoding(&parser, YAML_UTF8_ENCODING);
        yaml_stream_start_event_initialize(&event, YAML_UTF8_ENCODING);
    }

    ~Generator() {
        yaml_event_delete(&event);
        yaml_parser_delete(&parser);
    }

    yaml_parser_t parser;
    yaml_event_t event;
    Collection collection;
    std::vector<Collection> collectionStack;
    const char** errorMessage;
    size_t* errorOffset;

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

        bool ok = true;

        while (ok && event.type != YAML_STREAM_END_EVENT) {

            yaml_event_delete(&event);

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
            printEvent(event);
            #endif

            switch (event.type) {
            case YAML_NO_EVENT:
            case YAML_STREAM_START_EVENT:
            case YAML_STREAM_END_EVENT:
            case YAML_DOCUMENT_START_EVENT:
            case YAML_DOCUMENT_END_EVENT:
                break;
            case YAML_SEQUENCE_START_EVENT:
                // FIXME: If a sequence or mapping is found in a key, add a string version of this node instead.
                ok = handler.StartArray();
                pushCollection(false);
                break;
            case YAML_SEQUENCE_END_EVENT:
                ok = handler.EndArray(getSeqLength());
                popCollection();
                break;
            case YAML_MAPPING_START_EVENT:
                // FIXME: If a sequence or mapping is found in a key, add a string version of this node instead.
                ok = handler.StartObject();
                pushCollection(true);
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
                collection.count++;
                break;
            }
        }

        return ok;
    }

    #if Y2J_DEBUG
    void printEvent(yaml_event_t& event) {
        static int indent = 0;
        if (event.type == YAML_DOCUMENT_START_EVENT) { indent = 0; }
        if (event.type == YAML_SEQUENCE_END_EVENT || event.type == YAML_MAPPING_END_EVENT) { indent -= 2; }
        printf("%*s", indent, "");
        switch (event.type) {
        case YAML_NO_EVENT: printf("No event!\n"); break;
        case YAML_STREAM_START_EVENT: printf("Start Stream\n"); break;
        case YAML_STREAM_END_EVENT: printf("End Stream\n"); break;
        case YAML_DOCUMENT_START_EVENT: printf("Start Document\n"); break;
        case YAML_DOCUMENT_END_EVENT: printf("End Document\n"); break;
        case YAML_SEQUENCE_START_EVENT: printf("[\n"); indent += 2; break;
        case YAML_SEQUENCE_END_EVENT: printf("] (members: %lu)\n", getSeqLength()); break;
        case YAML_MAPPING_START_EVENT: printf("{\n"); indent += 2; break;
        case YAML_MAPPING_END_EVENT: printf("} (members: %lu)\n", getMapLength()); break;
        case YAML_ALIAS_EVENT: printf("Alias (anchor %s)\n", event.data.alias.anchor); break;
        case YAML_SCALAR_EVENT: printf(entryIsMapKey() ? "\"%s\":\n" : "\"%s\"\n", event.data.scalar.value); break;
        }
    }
    #endif
};

JsonDocument yamlParseBytes(const char* bytes, size_t length, const char** errorMessage, size_t* errorOffset) {

    Generator<JsonDocument> generator(bytes, length, errorMessage, errorOffset);
    JsonDocument document;
    document.Populate(generator);
    return document;
}

} // namespace y2j
