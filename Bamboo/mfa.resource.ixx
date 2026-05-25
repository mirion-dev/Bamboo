module;

#include <spdlog/spdlog.h>

export module bamboo.mfa.resource;

import std;
import bamboo.types;
import bamboo.diag;
import bamboo.stream;
import bamboo.model;
import bamboo.mfa.base;

namespace bamboo::mfa {

    export void load(Stream& stream, PreviewImage& value) {
        stream >> value.size;
        if (value.size != 0) {
            stream >> value.header >> args(value.data, value.size - sizeof(value.header));
        }
    }

    export void load(Stream& stream, Font& value) {
        stream >> value.handle
            >> value.checksum
            >> value.references
            >> skip<i32>
            >> value.data;

        spdlog::debug("Read font {:?}.", to_string(value.data.face_name));
    }

    export void load(Stream& stream, FontBank& value) {
        stream >> signature<"ATNF"> >> static_cast<std::vector<Font>&>(value);
        spdlog::debug("Read {} fonts.", value.size());
    }

    export void load(Stream& stream, Sound& value) {
        stream >> value.handle
            >> value.checksum
            >> value.references
            >> value.size
            >> value.flags
            >> value.frequency
            >> args(value.name, string_type_pascal_c);
        stream >> args(value.data, value.size - (value.flags[Sound::play_from_disk] ? 0 : (value.name.size() + 1) * 2));

        --value.handle;

        spdlog::debug("Read sound {:?}.", to_string(value.name));
    }

    export void load(Stream& stream, SoundBank& value) {
        stream >> signature<"APMS"> >> static_cast<std::vector<Sound>&>(value);
        spdlog::debug("Read {} sounds.", value.size());
    }

    export void load(Stream& stream, Music& value) {
        stream >> value.handle
            >> value.checksum
            >> value.references
            >> value.size
            >> value.flags
            >> value.frequency
            >> args(value.name, string_type_pascal_c);
        stream >> args(value.data, value.size - (value.name.size() + 1) * 2);

        spdlog::debug("Read music {:?}.", to_string(value.name));
    }

    export void load(Stream& stream, MusicBank& value) {
        stream >> signature<"ASUM"> >> static_cast<std::vector<Music>&>(value);
        spdlog::debug("Read {} music.", value.size());
    }

    export void load(Stream& stream, Image& value) {
        stream >> value.handle
            >> value.checksum
            >> value.references
            >> value.size
            >> value.width
            >> value.height
            >> value.format
            >> value.flags
            >> skip<i16>
            >> value.origin_x
            >> value.origin_y
            >> value.action_x
            >> value.action_y
            >> value.transparent_color
            >> args(value.data, value.size);

        if (stream.project->editor_build < 284) {
            ++value.handle;
        }
    }

    export void load(Stream& stream, ImageBank& value) {
        stream >> signature<"AGMI">
            >> skip<i32>
            >> value.palette
            >> static_cast<std::vector<Image>&>(value);

        spdlog::debug("Read {} images.", value.size());
    }

    export void load(Stream& stream, BinaryFiles& value) {
        stream >> static_cast<std::vector<std::wstring>&>(value);
        spdlog::debug("Read {} binary files.", value.size());
    }

    export void load(Stream& stream, Control& value) {
        stream >> value.type >> value.keys;
    }

    export void load(Stream& stream, Controls& value) {
        stream >> static_cast<std::vector<Control>&>(value);
        spdlog::debug("Read {} controls.", value.size());
    }

    export void load(Stream& stream, MenuItem& value) {
        stream >> value.flags;
        if (!value.flags[MenuItem::parent]) {
            stream >> value.id;
        }
        stream >> args(value.name, string_type_c);
        if (value.flags[MenuItem::parent]) {
            stream >> value.children;
        }

        spdlog::debug("Read menu item {:?}.", to_string(value.name));
    }

    export void load(Stream& stream, MenuItems& value) {
        value.clear();

        do {
            stream >> value.emplace_back();
        } while (!value.back().flags[MenuItem::last]);
    }

    export void load(Stream& stream, MenuAccel& value) {
        stream >> value.flags
            >> value.key
            >> value.id
            >> skip<i16>;
    }

    export void load(Stream& stream, MenuAccels& value) {
        value.clear();

        do {
            stream >> value.emplace_back();
        } while (!value.back().flags[MenuAccel::last]);
    }

    export void load(Stream& stream, MenuImage& value) {
        stream >> value.id >> skip<i16> >> value.image;
    }

    export void load(Stream& stream, MenuBar& value) {
        stream >> value.size;

        auto begin{ static_cast<usize>(stream.tellg()) };
        stream >> value.header_size
            >> value.item_offset
            >> value.item_size
            >> value.accel_offset
            >> value.accel_size;

        usize header_end{ begin + value.header_size };
        usize item_begin{ begin + value.item_offset };
        usize item_end{ item_begin + value.item_size };
        usize accel_begin{ begin + value.accel_offset };
        usize accel_end{ accel_begin + value.accel_size };
        usize end{ begin + value.size };
        if (stream.tellg() != header_end) {
            throw std::runtime_error{ "Corrupt menu header." };
        }

        stream.seekg(item_begin);
        stream >> skip<i32> >> value.items;
        if (stream.tellg() != item_end) {
            throw std::runtime_error{ "Corrupt menu items." };
        }

        stream.seekg(accel_begin);
        stream >> value.accels;
        if (stream.tellg() != accel_end) {
            throw std::runtime_error{ "Corrupt menu accelerators." };
        }

        stream.seekg(end);
        stream >> value.window_menu_index >> value.images;

        spdlog::debug("Read a menu bar.");
    }

    export void load(Stream& stream, Qualifier& value) {
        stream >> value.name >> value.icon;
        spdlog::debug("Read qualifier {:?}.", to_string(value.name));
    }

    export void load(Stream& stream, Qualifiers& value) {
        stream >> static_cast<std::vector<Qualifier>&>(value);
        spdlog::debug("Read {} qualifiers.", value.size());
    }

    export void load(Stream& stream, Extension& value) {
        stream >> value.handle
            >> value.filename
            >> value.name
            >> value.magic_num
            >> value.subtype
            >> value.is_unicode;

        spdlog::debug("Read extension {:?}.", to_string(value.name));
    }

    export void load(Stream& stream, Extensions& value) {
        stream >> static_cast<std::vector<Extension>&>(value);
        spdlog::debug("Read {} extensions.", value.size());
    }

}
