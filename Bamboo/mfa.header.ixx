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

    export void load(Stream& stream, Header& value) {
        stream >> signature<"MFU2">
            >> value.runtime_version
            >> value.runtime_subversion
            >> value.product_version
            >> value.product_build
            >> value.language
            >> value.app_name
            >> value.description
            >> value.editor_filename
            >> value.bitmap;

        spdlog::info("App name: {:?}.", to_string(value.app_name));
        spdlog::info("Build: {}.", value.product_build);
    }

}
