export module bamboo.model.header;

import std;
import bamboo.types;
import bamboo.model.base;

namespace bamboo {

    export struct PreviewImage {
        i32 size;
        BitmapInfoHeader header;
        std::vector<char> data;
    };

    export struct Header {
        i16 version;
        i16 subversion;
        i32 editor_version;
        i32 editor_build;
        LangCodeId language;      // About
        std::wstring name;        // About
        std::wstring description; // About
        std::wstring path;        // About
        PreviewImage preview_image;
    };

}
