module;

#include <spdlog/spdlog.h>

export module bamboo.mfa;

import bamboo.diag;
import bamboo.model;

export import bamboo.mfa.base;
export import bamboo.mfa.header;
export import bamboo.mfa.resource;
export import bamboo.mfa.event;
export import bamboo.mfa.object;
export import bamboo.mfa.manifest;
export import bamboo.mfa.frame;

namespace bamboo::mfa {

    export void load(Stream& stream, Project& value) {
        Timer timer;
        stream.project = &value;
        stream >> value.header >> value.resources >> value.manifest >> value.frames;
        spdlog::info("Read an MFA project in {:.3f} seconds.", timer.duration());
    }

}
