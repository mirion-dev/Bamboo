export module bamboo.model.header;

import std;
import bamboo.types;

namespace bamboo {

    export struct Header {
        i16 runtime_version;
        i16 runtime_subversion;
        i32 product_version;
        i32 product_build;
        i32 language;                 // App - About
        std::wstring app_name;        // App - About
        std::wstring description;     // App - About
        std::wstring editor_filename; // App - About
        std::vector<char> bitmap;
    };

}
