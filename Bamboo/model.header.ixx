export module bamboo.model.header;

import std;
import bamboo.types;

namespace bamboo {

    export struct Header {
        i16 runtime_version;
        i16 runtime_subversion;
        i32 product_version;
        i32 product_build;
        i32 language;                 // About - Language
        std::wstring app_name;        // About - Name
        std::wstring description;     // About - Description
        std::wstring editor_filename; // About - Filename
        std::vector<char> bitmap;
    };

}
