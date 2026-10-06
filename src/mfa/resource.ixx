module;

#include <miniz/miniz.h>
#include <spdlog/spdlog.h>

export module bamboo.mfa.resource;

import std;
import bamboo.types;
import bamboo.utils;
import bamboo.log;
import bamboo.stream;
import bamboo.stream_utils;
import bamboo.model;
import bamboo.mfa.base;

namespace bamboo::mfa {

    export void load(Stream& stream, Font& value) {
        stream
            >> value.handle
            >> value.checksum
            >> value.references
            >> skip<i32> // Unused
            >> value.data;

        logger()->debug("Read font {:?}.", to_string(value.data.face_name));
    }

    export void load(Stream& stream, FontBank& value) {
        stream
            >> signature<"ATNF"> // FoNT Array
            >> static_cast<std::vector<Font>&>(value);

        logger()->debug("Read {} fonts.", value.size());
    }

    export void load(Stream& stream, Sample& value) {
        stream
            >> value.handle
            >> value.checksum
            >> value.references
            >> value.size
            >> value.flags
            >> value.frequency
            >> args(value.name, string_type_pascal_c);
        stream
            >> args(value.data, value.size - (value.flags[Sample::play_from_disk] ? 0 : (value.name.size() + 1) * 2));

        --value.handle;

        logger()->debug("Read sample {:?}.", to_string(value.name));
    }

    export void load(Stream& stream, Music& value) {
        stream
            >> value.handle
            >> value.checksum
            >> value.references
            >> value.size
            >> value.flags
            >> value.frequency
            >> args(value.name, string_type_pascal_c);
        stream >> args(value.data, value.size - (value.name.size() + 1) * 2);

        logger()->debug("Read music {:?}.", to_string(value.name));
    }

    export void load(Stream& stream, SampleBank& value) {
        stream
            >> signature<"APMS"> // SaMPle Array
            >> static_cast<std::vector<Sample>&>(value);

        logger()->debug("Read {} samples.", value.size());
    }

    export void load(Stream& stream, MusicBank& value) {
        stream
            >> signature<"ASUM"> // MUSic Array
            >> static_cast<std::vector<Music>&>(value);

        logger()->debug("Read {} music.", value.size());
    }

    export void load(Stream& stream, Image& value) {
        stream
            >> value.handle
            >> value.checksum
            >> value.references
            >> value.size
            >> value.width
            >> value.height
            >> value.format
            >> value.flags
            >> skip<i16> // Unused
            >> value.origin_x
            >> value.origin_y
            >> value.action_x
            >> value.action_y
            >> value.transparent_color;

        if (!value.flags[Image::lzx]) {
            stream >> args(value.data, value.size);
        } else {
            i32 decomp_size;
            std::vector<unsigned char> raw_data;
            stream >> decomp_size >> args(raw_data, value.size - sizeof(decomp_size));

            value.data.resize(decomp_size);
            auto actual_size{ static_cast<mz_ulong>(decomp_size) };
            if (mz_uncompress(value.data.data(), &actual_size, raw_data.data(), static_cast<mz_ulong>(raw_data.size()))
                    != MZ_OK
                || decomp_size != actual_size) {
                throw std::runtime_error{ "Failed to decompress image data" };
            }
        }

        if (stream.app->editor_build < 284) {
            ++value.handle;
        }
    }

    export void load(Stream& stream, ImageBank& value) {
        stream
            >> signature<"AGMI"> // IMaGe Array
            >> skip<i32>         // Duplicate (app.graphic_mode)
            >> value.palette
            >> static_cast<std::vector<Image>&>(value);

        logger()->debug("Read {} images.", value.size());
    }

}
