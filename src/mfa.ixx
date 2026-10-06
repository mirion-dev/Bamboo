module;

#include <spdlog/spdlog.h>

export module bamboo.mfa;

import std;
import bamboo.types;
import bamboo.utils;
import bamboo.log;
import bamboo.stream_utils;
import bamboo.model;

export import bamboo.mfa.base;
export import bamboo.mfa.resource;
export import bamboo.mfa.event;
export import bamboo.mfa.object;
export import bamboo.mfa.frame;

namespace bamboo::mfa {

    export void load(Stream& stream, PreviewImage& value) {
        stream >> value.size;
        if (value.size == 0) {
            return;
        }

        stream >> value.header >> args(value.data, value.size - sizeof(value.header));
    }

    export void load(Stream& stream, Installer& value) {
        stream >> value.size;
        if (value.size == 0) {
            return;
        }

        stream >> args(value.data, value.size); // Unanalyzed
    }

    export void load(Stream& stream, BinaryFiles& value) {
        stream >> static_cast<std::vector<std::wstring>&>(value);
        logger()->debug("Read {} binary files.", value.size());
    }

    export void load(Stream& stream, Control& value) {
        stream >> value.type >> value.keys;
    }

    export void load(Stream& stream, Controls& value) {
        stream >> static_cast<std::vector<Control>&>(value);
        logger()->debug("Read {} controls.", value.size());
    }

    export void load(Stream& stream, MenuItems& value) {
        value.clear();

        do {
            MenuItem& item{ value.emplace_back() };
            stream >> static_cast<MenuEntry&>(item);
            if (item.flags[MenuItem::popup]) {
                stream >> item.children;
            }

            logger()->debug("Read menu item {:?}.", to_string(item.string));
        } while (!value.back().flags[MenuItem::end]);
    }

    export void load(Stream& stream, MenuAccels& value) {
        value.clear();

        do {
            stream >> value.emplace_back();
        } while (!value.back().flags[MenuAccel::end]);
    }

    export void load(Stream& stream, MenuImage& value) {
        stream
            >> value.id
            >> skip<i16> // Padding
            >> value.image;
    }

    export void load(Stream& stream, MenuBar& value) {
        stream >> value.size;

        auto begin{ static_cast<usize>(stream.tellg()) };
        stream >> value.header_size >> value.item_offset >> value.item_size >> value.accel_offset >> value.accel_size;
        if (stream.tellg() != begin + value.header_size) {
            throw std::runtime_error{ "Corrupt menu header" };
        }

        if (value.item_size != 0) {
            stream >> move(begin + value.item_offset) >> value.header;
            stream >> move(static_cast<usize>(stream.tellg()) + value.header.offset) >> value.items;
            if (stream.tellg() != begin + value.item_offset + value.item_size) {
                throw std::runtime_error{ "Corrupt menu items" };
            }
        }

        if (value.accel_size != 0) {
            stream >> move(begin + value.accel_offset) >> value.accels;
            if (stream.tellg() != begin + value.accel_offset + value.accel_size) {
                throw std::runtime_error{ "Corrupt menu accelerators" };
            }
        }

        stream >> move(begin + value.size) >> value.window_menu_index >> value.images;
        logger()->debug("Read a menu bar.");
    }

    export void load(Stream& stream, Qualifier& value) {
        stream >> value.name >> value.icon;
        logger()->debug("Read qualifier {:?}.", to_string(value.name));
    }

    export void load(Stream& stream, Qualifiers& value) {
        stream >> static_cast<std::vector<Qualifier>&>(value);
        logger()->debug("Read {} qualifiers.", value.size());
    }

    export void load(Stream& stream, Extension& value) {
        stream >> value.handle >> value.filename >> value.name >> value.magic_num >> value.subtype >> value.is_unicode;
        logger()->debug("Read extension {:?}.", to_string(value.name));
    }

    export void load(Stream& stream, Extensions& value) {
        stream >> static_cast<std::vector<Extension>&>(value);
        logger()->debug("Read {} extensions.", value.size());
    }

    export void load(Stream& stream, Application& value) {
        Timer timer;

        stream.app = &value;
        stream
            >> signature<"MFU2"> // Multimedia FUsion 2
            >> value.format_version
            >> value.format_subversion
            >> value.editor_version
            >> value.editor_build
            >> value.language
            >> value.name
            >> value.description
            >> value.path
            >> value.preview_image;

        logger()->info("Project name: {}.", to_string(value.name));
        logger()->info("Editor build: {}.", value.editor_build);

        stream
            >> value.font_bank
            >> value.sample_bank
            >> value.music_bank
            >> value.icon_bank
            >> value.image_bank
            >> skip<std::wstring> // Duplicate (name)
            >> value.author
            >> skip<std::wstring> // Duplicate (description)
            >> value.copyright
            >> value.company
            >> value.version
            >> value.window_width
            >> value.window_height
            >> value.border_color
            >> value.window_flags
            >> value.flags
            >> value.help_file
            >> skip<std::wstring> // Unused
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

        logger()->info("Read an MFA project in {:.3f} seconds.", timer.duration());
    }

}
