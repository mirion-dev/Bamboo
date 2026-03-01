export module bamboo.mfa;

import std;
import bamboo.core;
import bamboo.stream;

namespace bamboo::mfa {

    template <usize Size>
    struct Ignore {
        friend void load(Stream& stream, Ignore) {
            stream.ignore(Size);
        }
    };

    template <usize Size>
    static constexpr Ignore<Size> ignore;

    template <LiteralString Expected>
    struct Signature {
        friend void load(Stream& stream, Signature) {
            std::array<char, Expected.size()> buffer;
            stream.read(buffer.data(), buffer.size());

            std::string_view expected{ Expected }, actual{ buffer };
            if (expected != actual) {
                throw std::runtime_error{
                    std::format("Incorrect signature. Expected \"{}\" but read \"{}\".", expected, actual)
                };
            }
        }
    };

    template <LiteralString Expected>
    static constexpr Signature<Expected> signature;

    export struct Header {
        i16 runtime_version;
        i16 runtime_subversion;
        i32 product_version;
        i32 product_build;
        i32 language;

        friend void load(Stream& stream, Header& header) {
            stream >> signature<"MFU2">
                >> header.runtime_version
                >> header.runtime_subversion
                >> header.product_version
                >> header.product_build
                >> header.language;
        }
    };

}
