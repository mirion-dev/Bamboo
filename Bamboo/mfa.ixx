module;

#include <spdlog/spdlog.h>

export module bamboo.mfa;

import std;
import bamboo.types;
import bamboo.diag;
import bamboo.stream;
import bamboo.model;

export import bamboo.mfa.base;
export import bamboo.mfa.resource;
export import bamboo.mfa.event;
export import bamboo.mfa.object;
export import bamboo.mfa.frame;

namespace bamboo::mfa {

    export void load(Stream& stream, PreviewImage& value) {
        stream >> value.size;
        if (value.size != 0) {
            stream >> value.header >> args(value.data, value.size - sizeof(value.header));
        }
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

    export void load(Stream& stream, Project& value) {
        Timer timer;

        stream.project = &value;
        stream >> signature<"MFU2">
            >> value.format_version
            >> value.format_subversion
            >> value.editor_version
            >> value.editor_build
            >> value.language
            >> value.name
            >> value.description
            >> value.path
            >> value.preview_image;

        spdlog::info("Project name: {:?}.", to_string(value.name));
        spdlog::info("Editor build: {}.", value.editor_build);

        stream >> value.font_bank
            >> value.sound_bank
            >> value.music_bank
            >> value.icon_bank
            >> value.image_bank
            >> skip<std::wstring> // name
            >> value.author
            >> skip<std::wstring> // description
            >> value.copyright
            >> value.company
            >> value.version
            >> value.window_width
            >> value.window_height
            >> value.border_color
            >> value.window_flags
            >> value.flags
            >> value.help_file
            >> value.vitalize_preview_file
            >> value.init_score
            >> value.init_lives
            >> value.frame_rate
            >> value.build_type
            >> value.build_filename
            >> value.effects_folder
            >> value.command_line
            >> value.about
            >> value.installer
            >> value.binary_files
            >> value.controls
            >> value.menu_bar
            >> value.global_values
            >> value.global_strings
            >> value.global_events
            >> value.graphic_mode
            >> value.window_icons
            >> value.qualifiers
            >> value.extensions
            >> value.frames
            >> value.chunks;

        spdlog::info("Read an MFA project in {:.3f} seconds.", timer.duration());
    }

}
