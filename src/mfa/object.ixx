module;

#include <spdlog/spdlog.h>

export module bamboo.mfa.object;

import std;
import bamboo.types;
import bamboo.utils;
import bamboo.log;
import bamboo.stream;
import bamboo.stream_utils;
import bamboo.model;
import bamboo.mfa.base;

namespace bamboo::mfa {

    export void load(Stream& stream, Chunk& value) {
        stream >> value.id;
        if (value.id != 0) {
            stream >> value.data; // Unanalyzed
        }
    }

    export void load(Stream& stream, Chunks& value) {
        value.clear();

        Chunk chunk;
        while (stream >> chunk, chunk.id != 0) {
            value.emplace_back(std::move(chunk));
        }

        logger()->debug("Read {} chunks.", value.size());
    }

    export void load(Stream& stream, Value& value) {
        i32 type;
        stream >> value.name >> type;
        switch (type) {
        case 0:
            stream >> value.emplace<i32>();
            break;
        case 1:
            stream >> value.emplace<f64>();
            break;
        case 2:
            stream >> value.emplace<std::wstring>();
            break;
        default:
            throw std::runtime_error{ std::format("Unknown value type {}.", type) };
        }
    }

    export void load(Stream& stream, Movement& value) {
        stream >> value.name >> value.extension_name >> value.id >> value.data; // Unanalyzed
    }

    export void load(Stream& stream, Behavior& value) {
        stream >> value.name >> value.data; // Unanalyzed
    }

    export void load(Stream& stream, Transition& value) {
        stream
            >> value.filename
            >> value.name
            >> value.file_handle
            >> value.id
            >> value.duration
            >> value.flags
            >> value.color
            >> value.param;
    }

    export void load(Stream& stream, Direction& value) {
        stream
            >> value.index
            >> value.max_speed
            >> value.min_speed
            >> value.repeat
            >> value.repeat_from
            >> value.images;
    }

    export void load(Stream& stream, Animation& value) {
        stream >> value.name >> value.directions;
    }

    export void load(Stream& stream, Text& value) {
        stream >> value.string >> value.flags;
    }

    export void load(Stream& stream, Content& value) {
        stream >> value.font >> value.color >> value.flags >> value.is_relief >> value.texts;
    }

    export void load(Stream& stream, ObjectBase& value) {
        i32 effect_param;
        stream
            >> value.handle
            >> value.name
            >> value.transparent
            >> value.effect
            >> effect_param
            >> value.antialiasing
            >> value.flags
            >> args(value.icon, type<u32>)
            >> value.chunks;

        if (value.effect == 1) {
            value.blend_coefficent = std::clamp(effect_param * 2, 0, 255);
        }
    }

    export void load(Stream& stream, StaticObject& value) {
        stream >> static_cast<ObjectBase&>(value) >> value.obstacle_type >> value.collision_type;
    }

    export void load(Stream& stream, QuickBackdropObject& value) {
        stream
            >> static_cast<StaticObject&>(value)
            >> value.width
            >> value.height
            >> value.shape
            >> value.border_size
            >> value.border_color
            >> value.fill_type
            >> value.color1
            >> value.color2
            >> value.flags
            >> value.image;
    }

    export void load(Stream& stream, BackdropObject& value) {
        stream >> static_cast<StaticObject&>(value) >> value.image;
    }

    export void load(Stream& stream, DynamicObject& value) {
        stream
            >> static_cast<ObjectBase&>(value)
            >> value.flags
            >> value.new_flags
            >> value.background_color
            >> value.qualifiers
            >> skip<i16> >> value.values
            >> value.strings
            >> value.movements
            >> value.behaviors
            >> value.fade_in
            >> value.fade_out;
    }

    export void load(Stream& stream, AnimatedObject& value) {
        stream >> static_cast<DynamicObject&>(value) >> value.animations;
    }

    export void load(Stream& stream, ExtensionObject& value) {
        stream >> static_cast<AnimatedObject&>(value) >> value.type;

        if (value.type == -1) {
            stream >> value.name >> value.filename >> value.magic_num >> value.subtype;
        }

        stream >> value.real_size >> value.size >> skip<i32> >> value.version >> value.id >> value.private_data;
        stream >> args(value.data, value.real_size - 20);
    }

    export void load(Stream& stream, TextObject& value) {
        stream >> static_cast<DynamicObject&>(value) >> value.width >> value.height;
    }

    export void load(Stream& stream, StringObject& value) {
        stream >> static_cast<TextObject&>(value) >> value.content;
    }

    export void load(Stream& stream, QuestionObject& value) {
        stream >> static_cast<TextObject&>(value) >> value.question >> value.answer;
    }

    export void load(Stream& stream, RichTextObject& value) {
        stream >> static_cast<TextObject&>(value) >> value.flags >> value.color >> value.data;
    }

    export void load(Stream& stream, PlayerCounterObject& value) {
        stream
            >> static_cast<DynamicObject&>(value)
            >> value.player
            >> value.images
            >> value.use_text
            >> value.color
            >> value.font
            >> value.width
            >> value.height;
    }

    export void load(Stream& stream, CounterObject& value) {
        stream
            >> static_cast<DynamicObject&>(value)
            >> value.value
            >> value.min
            >> value.max
            >> value.display_type
            >> value.fill_type
            >> value.color1
            >> value.color2
            >> value.vertical_gradient
            >> value.bar_direction
            >> value.width
            >> value.height
            >> value.images
            >> value.font;
    }

    export void load(Stream& stream, SubapplicationObject& value) {
        stream >> static_cast<DynamicObject&>(value) >> value.name >> value.width >> value.height >> value.flags;

        if (value.flags[SubapplicationObject::internal]) {
            stream >> value.start_frame;
        }

        stream >> skip<i32>;
    }

    export void load(Stream& stream, Object& value) {
        static constexpr std::array OBJECT_TYPE{ "quick backdrop", "backdrop",       "active",   "string",
                                                 "question",       "score",          "lives",    "counter",
                                                 "rich text",      "subapplication", "extension" };

        i32 type;
        stream >> type;
        switch (type) {
        case 0:
            stream >> value.emplace<QuickBackdropObject>();
            break;
        case 1:
            stream >> value.emplace<BackdropObject>();
            break;
        case 2:
            stream >> value.emplace<ActiveObject>();
            break;
        case 3:
            stream >> value.emplace<StringObject>();
            break;
        case 4:
            stream >> value.emplace<QuestionObject>();
            break;
        case 5:
            stream >> value.emplace<ScoreObject>();
            break;
        case 6:
            stream >> value.emplace<LivesObject>();
            break;
        case 7:
            stream >> value.emplace<CounterObject>();
            break;
        case 8:
            stream >> value.emplace<RichTextObject>();
            break;
        case 9:
            stream >> value.emplace<SubapplicationObject>();
            break;
        default:
            value.extension = type - 32;
            stream >> value.emplace<ExtensionObject>();
        }

        logger()->debug(
            "Read {} object {:?}.",
            OBJECT_TYPE[std::max(type, 10)],
            to_string(std::visit([](auto& value) -> ObjectBase& { return value; }, value).name)
        );
    }

    export void load(Stream& stream, Objects& value) {
        stream >> static_cast<std::vector<Object>&>(value);
        logger()->debug("Read {} objects.", value.size());
    }

}
