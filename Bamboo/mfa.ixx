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
            >> value.global_numbers
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
