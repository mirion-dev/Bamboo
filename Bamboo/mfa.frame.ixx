module;

#include <spdlog/spdlog.h>

export module bamboo.mfa:frame;

import :base;
import :object;
import :event;

namespace bamboo::mfa {

    void load(Stream& stream, Layer& value) {
        stream >> value.name
            >> value.flags
            >> value.x_coefficient
            >> value.y_coefficient;

        spdlog::debug("Read layer {:?}.", to_string(value.name));
    }

    void load(Stream& stream, Layers& value) {
        stream >> static_cast<std::vector<Layer>&>(value);
        spdlog::debug("Read {} layers.", value.size());
    }

    void load(Stream& stream, Folder& value) {
        stream >> value.header;
        if (((value.header & 0xff) == 112 ? value.header >> 24 : value.header) == 4) {
            stream >> value.name >> value.children;
            spdlog::debug("Read folder {:?}.", to_string(value.name));
        }
        else {
            stream >> skip<i32>;
        }
    }

    void load(Stream& stream, Folders& value) {
        stream >> static_cast<std::vector<Folder>&>(value);
        spdlog::debug("Read {} folders.", value.size());
    }

    void load(Stream& stream, Instance& value) {
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

    void load(Stream& stream, Instances& value) {
        stream >> static_cast<std::vector<Instance>&>(value);
        spdlog::debug("Read {} instances.", value.size());
    }

    void load(Stream& stream, Frame& value) {
        stream >> value.handle
            >> value.name
            >> value.width
            >> value.height
            >> value.background_color
            >> value.flags
            >> value.max_objects
            >> value.password
            >> skip<std::vector<char>>
            >> value.editor_x
            >> value.editor_y
            >> value.palette
            >> value.icon
            >> value.editor_layer
            >> value.layers
            >> value.transition_in
            >> value.transition_out
            >> value.objects
            >> value.folders
            >> value.instances
            >> value.events
            >> value.chunks;

        spdlog::debug("Read frame {:?}.", to_string(value.name));
    }

    void load(Stream& stream, Frames& value) {
        value.clear();

        stream >> value.offsets >> value.end;

        for (u32 offset : value.offsets) {
            Frame frame;
            stream.seekg(offset);
            stream >> frame;
            value.emplace_back(std::move(frame));
        }

        stream.seekg(value.end);
        stream >> value.chunks;

        spdlog::debug("Read {} frames.", value.size());
    }

}
