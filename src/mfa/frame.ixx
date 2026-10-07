module;

#include <spdlog/spdlog.h>

export module bamboo.mfa.frame;

import std;
import bamboo.types;
import bamboo.utils;
import bamboo.log;
import bamboo.stream;
import bamboo.stream_utils;
import bamboo.model;
import bamboo.mfa.base;
import bamboo.mfa.event;
import bamboo.mfa.object;

namespace bamboo::mfa {

    export void load(Stream& stream, Layer& value) {
        stream >> value.name >> value.flags >> value.x_coefficient >> value.y_coefficient;
        logger()->debug("Read layer {:?}.", to_string(value.name));
    }

    export void load(Stream& stream, Layers& value) {
        stream >> static_cast<std::vector<Layer>&>(value);
        logger()->debug("Read {} layers.", value.size());
    }

    export void load(Stream& stream, Folder& value) {
        stream >> value.name >> value.children;
        logger()->debug("Read folder entry {:?}.", to_string(value.name));
    }

    export void load(Stream& stream, Entry& value) {
        i32 type;
        stream >> type;
        if (type == 0x70000004) {
            stream >> value.emplace<Folder>();
        } else {
            stream >> value.emplace<u32>();
        }
    }

    export void load(Stream& stream, Entries& value) {
        stream >> static_cast<std::vector<Entry>&>(value);
        logger()->debug("Read {} entries.", value.size());
    }

    export void load(Stream& stream, Instance& value) {
        stream
            >> value.x
            >> value.y
            >> value.layer
            >> value.handle
            >> value.flags
            >> value.value
            >> value.type
            >> value.object
            >> value.relative;
    }

    export void load(Stream& stream, Instances& value) {
        stream >> static_cast<std::vector<Instance>&>(value);
        logger()->debug("Read {} instances.", value.size());
    }

    export void load(Stream& stream, Frame& value) {
        stream
            >> value.handle
            >> value.name
            >> value.width
            >> value.height
            >> value.background_color
            >> value.flags
            >> value.max_objects
            >> value.password
            >> skip<std::vector<unsigned char>> // Unused (metadata)
            >> value.editor_x
            >> value.editor_y
            >> value.palette
            >> value.icon
            >> value.editor_layer
            >> value.layers
            >> value.fade_in
            >> value.fade_out
            >> value.objects
            >> value.entries
            >> value.instances
            >> value.events
            >> value.chunks;

        logger()->debug("Read frame {:?}.", to_string(value.name));
    }

    export void load(Stream& stream, Frames& value) {
        value.clear();

        stream >> value.offsets >> value.end;
        for (u32 offset : value.offsets) {
            stream >> move(offset) >> value.emplace_back();
        }

        stream >> move(value.end);
        logger()->debug("Read {} frames.", value.size());
    }

}
