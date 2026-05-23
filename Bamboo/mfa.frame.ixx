module;

#include <spdlog/spdlog.h>

export module bamboo.mfa.frame;

import std;
import bamboo.types;
import bamboo.diag;
import bamboo.stream;
import bamboo.model;
import bamboo.mfa.base;
import bamboo.mfa.object;
import bamboo.mfa.event;

namespace bamboo::mfa {

    export void load(Stream& stream, Layer& value) {
        stream >> value.name
            >> value.flags
            >> value.x_coefficient
            >> value.y_coefficient;

        spdlog::debug("Read layer {:?}.", to_string(value.name));
    }

    export void load(Stream& stream, Layers& value) {
        stream >> static_cast<std::vector<Layer>&>(value);
        spdlog::debug("Read {} layers.", value.size());
    }

    export void load(Stream& stream, Folder& value) {
        stream >> value.header;
        if (((value.header & 0xff) == 112 ? value.header >> 24 : value.header) == 4) {
            stream >> value.name >> value.children;
            spdlog::debug("Read folder {:?}.", to_string(value.name));
        }
        else {
            stream >> skip<i32>;
        }
    }

    export void load(Stream& stream, Folders& value) {
        stream >> static_cast<std::vector<Folder>&>(value);
        spdlog::debug("Read {} folders.", value.size());
    }

    export void load(Stream& stream, Instance& value) {
        stream >> value.pos_x
            >> value.pos_y
            >> value.layer
            >> value.handle
            >> value.flags
            >> value.value
            >> value.parent_type
            >> value.object_info
            >> value.parent;
    }

    export void load(Stream& stream, Instances& value) {
        stream >> static_cast<std::vector<Instance>&>(value);
        spdlog::debug("Read {} instances.", value.size());
    }

    export void load(Stream& stream, Frame& value) {
        stream >> value.handle
            >> value.name
            >> value.width
            >> value.height
            >> value.background_color
            >> value.flags
            >> value.object_num
            >> value.password
            >> skip<std::vector<char>>
            >> value.editor_x
            >> value.editor_y
            >> value.palette
            >> value.icon
            >> value.editor_layer
            >> value.layers
            >> value.fade_in
            >> value.fade_out
            >> value.objects
            >> value.folders
            >> value.instances
            >> value.events
            >> value.chunks;

        spdlog::debug("Read frame {:?}.", to_string(value.name));
    }

    export void load(Stream& stream, Frames& value) {
        value.clear();

        i32 temp;
        stream >> temp;
        stream.seekg(temp > 900 ? -2 : -4, std::ios_base::cur);

        stream >> value.offsets >> value.end;
        for (u32 offset : value.offsets) {
            stream.seekg(offset);
            stream >> value.emplace_back();
        }

        stream.seekg(value.end);
        stream >> value.chunks;

        spdlog::debug("Read {} frames.", value.size());
    }

}
