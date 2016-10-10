# y2j
Experimental yaml-to-json parser

### what?

This is an experiment aimed at enabling very fast parsing of YAML 1.2 documents on mobile and embedded devices.

### how?

It uses [libYAML](https://github.com/yaml/libyaml) to produce a stream of parsing events from a YAML document and then feeds those events into a [RapidJSON](https://github.com/miloyip/rapidjson/) document.

The output of this parser does *not* support all possible YAML structures, only the subset of YAML 1.2 that forms JSON. Notably, this means that "keys" in mapping nodes can only be scalar values.

### why?

`¯\_(ツ)_/¯`
