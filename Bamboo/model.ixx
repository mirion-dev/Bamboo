export module bamboo.model;

export import :base;
export import :resource;
export import :manifest;
export import :object;
export import :event;
export import :frame;

namespace bamboo {

    export struct File {
        Header header;
        Resources resources;
        Manifest manifest;
        Frames frames;
    };

}
