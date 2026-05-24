export module bamboo.model;

export import bamboo.model.base;
export import bamboo.model.header;
export import bamboo.model.resource;
export import bamboo.model.event;
export import bamboo.model.object;
export import bamboo.model.manifest;
export import bamboo.model.frame;

namespace bamboo {

    export struct File {
        Header header;
        Resources resources;
        Manifest manifest;
        Frames frames;
    };

}
