export module bamboo.mfa;

import std;
import bamboo.core;
import bamboo.stream;

namespace bamboo::mfa {

    template <LiteralString Expected>
    struct Signature {
        friend void load(Stream& stream, Signature&) {
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

    export struct Header {
        Signature<"MFU2"> sign;

        friend void load(Stream& stream, Header& header) {
            stream.load(header.sign);
        }
    };

}
