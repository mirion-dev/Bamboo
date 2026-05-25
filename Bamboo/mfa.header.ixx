module;

#include <spdlog/spdlog.h>

export module bamboo.mfa.header;

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

    export void load(Stream& stream, Header& value) {
        stream >> signature<"MFU2">
            >> value.version
            >> value.subversion
            >> value.editor_version
            >> value.editor_build
            >> value.language
            >> value.name
            >> value.description
            >> value.path
            >> value.preview_image;

        spdlog::info("Project name: {:?}.", to_string(value.name));
        spdlog::info("Editor build: {}.", value.editor_build);
    }

}
