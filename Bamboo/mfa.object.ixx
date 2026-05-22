module;

#include <spdlog/spdlog.h>

export module bamboo.mfa:object;

import :base;

namespace bamboo::mfa {

    void load(Stream& stream, Transition& value) {
        stream >> value.name
            >> value.module_name
            >> value.module
            >> value.id
            >> value.duration
            >> value.use_color
            >> value.color
            >> value.param;
    }

    void load(Stream& stream, Movement& value) {
        stream >> value.name
            >> value.extension
            >> value.id
            >> value.data;
    }

    void load(Stream& stream, Behavior& value) {
        stream >> value.name >> value.data;
    }

    void load(Stream& stream, Direction& value) {
        stream >> value.index
            >> value.max_speed
            >> value.min_speed
            >> value.repeat
            >> value.repeat_frame
            >> value.frames;
    }

    void load(Stream& stream, Animation& value) {
        stream >> value.name >> value.directions;
    }

    void load(Stream& stream, Paragraph& value) {
        stream >> value.value >> value.flags;
    }

    void load(Stream& stream, Content& value) {
        stream >> value.font
            >> value.color
            >> value.flags
            >> value.relief
            >> value.paragraphs;
    }

    void load(Stream& stream, QuickBackdrop& value) {
        stream >> value.obstacle_type
            >> value.collision_type
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

    void load(Stream& stream, Backdrop& value) {
        stream >> value.obstacle_type
            >> value.collision_type
            >> value.image;
    }

    void load(Stream& stream, ObjectBase& value) {
        stream >> value.flags
            >> value.new_flags
            >> value.background_color
            >> value.qualifiers
            >> skip<i16>
            >> value.numbers
            >> value.strings
            >> value.movements
            >> value.behaviors
            >> value.transition_in
            >> value.transition_out;
    }

    void load(Stream& stream, ActiveObject& value) {
        stream >> static_cast<ObjectBase&>(value) >> value.animations;
    }

    void load(Stream& stream, StringObject& value) {
        stream >> static_cast<ObjectBase&>(value)
            >> value.width
            >> value.height
            >> value.content;
    }

    void load(Stream& stream, QuestionAnswerObject& value) {
        stream >> static_cast<ObjectBase&>(value)
            >> value.width
            >> value.height
            >> value.question
            >> value.answer;
    }

    void load(Stream& stream, ScoreLivesObject& value) {
        stream >> static_cast<ObjectBase&>(value)
            >> value.player
            >> value.images
            >> value.use_text
            >> value.color
            >> value.font
            >> value.width
            >> value.height;
    }

    void load(Stream& stream, CounterObject& value) {
        stream >> static_cast<ObjectBase&>(value)
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

    void load(Stream& stream, FormattedTextObject& value) {
        stream >> static_cast<ObjectBase&>(value)
            >> value.width
            >> value.height
            >> value.flags
            >> value.color
            >> value.data;
    }

    void load(Stream& stream, SubapplicationObject& value) {
        stream >> static_cast<ObjectBase&>(value)
            >> value.name
            >> value.width
            >> value.height
            >> value.flags;

        if (value.flags[SubapplicationObject::internal]) {
            stream >> value.start_frame;
        }

        stream >> skip<i32>;
    }

    void load(Stream& stream, ExtensionObject& value) {
        stream >> static_cast<ActiveObject&>(value) >> value.type;

        if (value.type == -1) {
            stream >> value.name
                >> value.filename
                >> value.magic_num
                >> value.subtype;
        }

        stream >> value.real_size
            >> value.size
            >> skip<i32>
            >> value.version
            >> value.id
            >> value.private_data;
        stream >> args(value.data, value.real_size - 20);
    }

    void load(Stream& stream, Chunk& value) {
        stream >> value.id;
        if (value.id != 0) {
            stream >> value.data;
        }
    }

    void load(Stream& stream, Chunks& value) {
        value.clear();

        Chunk chunk;
        while (stream >> chunk, chunk.id != 0) {
            value.emplace_back(std::move(chunk));
        }

        spdlog::debug("Read {} chunks.", value.size());
    }

    void load(Stream& stream, Object& value) {
        i32 type;
        stream >> type
            >> value.handle
            >> value.name
            >> value.transparent
            >> value.ink_effect
            >> value.ink_effect_param
            >> value.anti_aliasing
            >> value.flags
            >> value.icon_type
            >> value.icon
            >> value.chunks;

        switch (type) {
        case Object::quick_backdrop:
            stream >> value.emplace<QuickBackdrop>();
            break;
        case Object::backdrop:
            stream >> value.emplace<Backdrop>();
            break;
        case Object::active:
            stream >> value.emplace<ActiveObject>();
            break;
        case Object::string:
            stream >> value.emplace<StringObject>();
            break;
        case Object::question_answer:
            stream >> value.emplace<QuestionAnswerObject>();
            break;
        case Object::score:
            stream >> value.emplace<ScoreObject>();
            break;
        case Object::lives:
            stream >> value.emplace<LivesObject>();
            break;
        case Object::counter:
            stream >> value.emplace<CounterObject>();
            break;
        case Object::formatted_text:
            stream >> value.emplace<FormattedTextObject>();
            break;
        case Object::subapplication:
            stream >> value.emplace<SubapplicationObject>();
            break;
        default:
            stream >> value.emplace<ExtensionObject>();
        }

        spdlog::debug("Read object {:?}.", to_string(value.name));
    }

    void load(Stream& stream, Objects& value) {
        stream >> static_cast<std::vector<Object>&>(value);
        spdlog::debug("Read {} objects.", value.size());
    }

}
