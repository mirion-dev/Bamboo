module;

#include <spdlog/spdlog.h>

export module bamboo.mfa;

export import :base;
export import :resource;
export import :manifest;
export import :object;
export import :event;
export import :frame;

namespace bamboo::mfa {

    export void load(Stream& stream, File& value) {
        Timer timer;

        stream >> value.header;
        stream.build = value.header.product_build;
        stream >> value.resources >> value.manifest >> value.frames;

        spdlog::info("Read a MFA file in {:.3f} seconds.", timer.duration());
    }

}
