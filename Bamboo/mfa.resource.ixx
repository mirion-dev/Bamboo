module;

#include <spdlog/spdlog.h>

export module bamboo.mfa:resource;

import :base;

namespace bamboo::mfa {

    void load(Stream& stream, Font& value) {
        stream >> value.handle
            >> value.checksum
            >> value.references
            >> value.size
            >> value.height
            >> value.width
            >> value.escapement
            >> value.orientation
            >> value.weight
            >> value.italic
            >> value.underline
            >> value.strike_out
            >> value.charset
            >> value.out_precision
            >> value.clip_precision
            >> value.quality
            >> value.pitch_family
            >> args(value.name, string_type_fixed_c<32>);

        spdlog::debug("Read font {:?}.", to_string(value.name));
    }

    void load(Stream& stream, FontBank& value) {
        stream >> signature<"ATNF"> >> static_cast<std::vector<Font>&>(value);
        spdlog::debug("Read {} fonts.", value.size());
    }

    void load(Stream& stream, Sound& value) {
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

    void load(Stream& stream, SoundBank& value) {
        stream >> signature<"APMS"> >> static_cast<std::vector<Sound>&>(value);
        spdlog::debug("Read {} sounds.", value.size());
    }

    void load(Stream& stream, Music& value) {
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

    void load(Stream& stream, MusicBank& value) {
        stream >> signature<"ASUM"> >> static_cast<std::vector<Music>&>(value);
        spdlog::debug("Read {} music.", value.size());
    }

    void load(Stream& stream, Image& value) {
        stream >> value.handle
            >> value.checksum
            >> value.references
            >> value.size
            >> value.width
            >> value.height
            >> value.graphic_mode
            >> value.flags
            >> skip<i16>
            >> value.hotspot_x
            >> value.hotspot_y
            >> value.action_x
            >> value.action_y
            >> value.transparent_color
            >> args(value.data, value.size);

        if (stream.build < 284) {
            ++value.handle;
        }
    }

    void load(Stream& stream, ImageBank& value) {
        stream >> signature<"AGMI">
            >> value.graphic_mode
            >> value.palette_version
            >> args(value.palette, size_type<i16>)
            >> static_cast<std::vector<Image>&>(value);

        spdlog::debug("Read {} images.", value.size());
    }

    void load(Stream& stream, Resources& value) {
        stream >> value.font_bank
            >> value.sound_bank
            >> value.music_bank
            >> value.icon_bank
            >> value.image_bank;
    }

}
