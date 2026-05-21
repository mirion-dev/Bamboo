module;

#include <spdlog/spdlog.h>
#undef pascal

export module bamboo.mfa;

import std;
import bamboo.types;
import bamboo.diag;
import bamboo.stream;
import bamboo.stream.utils;
import bamboo.general;

namespace bamboo::mfa {

    export class Stream : public bamboo::Stream {
    public:
        i32 build{};

        using bamboo::Stream::Stream;
    };

    template <class T, usize N>
    static void load(Stream& stream, std::array<T, N>& value) {
        stream >> bamboo::args(value.data(), N);
    }

    template <std::integral T>
    struct SizeType {};

    template <std::integral T>
    static constexpr SizeType<T> size_type;

    template <class T>
    static void load(Stream& stream, std::vector<T>& value, std::integral auto size) {
        bamboo::resize_load(stream, value, size);
    }

    template <class T, std::integral S = i32>
    static void load(Stream& stream, std::vector<T>& value, SizeType<S> = {}) {
        S size;
        stream >> size >> bamboo::args(value, size);
    }

    enum class StringTypeEnum {
        pascal,
        c,
        pascal_c,
        fixed_c
    };

    template <StringTypeEnum Type, usize N = {}>
    struct StringType {};

    static constexpr StringType<StringTypeEnum::pascal> string_type_pascal;
    static constexpr StringType<StringTypeEnum::c> string_type_c;
    static constexpr StringType<StringTypeEnum::pascal_c> string_type_pascal_c;
    template <usize N>
    static constexpr StringType<StringTypeEnum::fixed_c, N> string_type_fixed_c;

    template <StringTypeEnum Type = {}, usize N = {}>
    static void load(Stream& stream, std::wstring& value, StringType<Type, N> = {}) {
        if constexpr (Type == StringTypeEnum::pascal) {
            static constexpr u32 MASK_UNICODE{ 1u << 31 };

            i32 size;
            stream >> size;
            if (!(size & MASK_UNICODE)) {
                throw std::runtime_error{ "ASCII strings are unsupported." };
            }
            resize_load(stream, value, size & ~MASK_UNICODE);
        }
        else if constexpr (Type == StringTypeEnum::c) {
            value.clear();
            wchar_t ch;
            while (stream >> ch, ch != '\0') {
                value.push_back(ch);
            }
        }
        else if constexpr (Type == StringTypeEnum::pascal_c) {
            i32 size;
            stream >> size;
            resize_load(stream, value, size);
            if (value.empty() || value.back() != '\0') {
                throw std::runtime_error{ "A Pascal-C string must be null-terminated." };
            }
            value.pop_back();
        }
        else if constexpr (Type == StringTypeEnum::fixed_c) {
            resize_load(stream, value, N);
            usize end{ value.find(L'\0') };
            if (end == -1) {
                throw std::runtime_error{ "A fixed C string must be null-terminated." };
            }
            value.resize(end);
        }
        else {
            static_assert(false, "Unknown StringTypeEnum.");
        }
    }

    template <class T>
    static void load(Stream& stream, std::optional<T>& value) {
        u8 has_value;
        stream >> has_value;
        if (has_value) {
            stream >> value.emplace();
        }
    }

    static void load(Stream& stream, Header& value) {
        stream >> signature<"MFU2">
            >> value.runtime_version
            >> value.runtime_subversion
            >> value.product_version
            >> value.product_build
            >> value.language
            >> value.app_name
            >> skip<std::wstring>
            >> value.editor_filename
            >> skip<std::vector<char>>;

        spdlog::debug("Application name: {:?}, build: {}.", to_string(value.app_name), value.product_build);
    }

    static void load(Stream& stream, Font& value) {
        stream >> value.handle
            >> value.checksum
            >> value.references
            >> value.size
            >> value.height
            >> value.width
            >> value.escapement
            >> value.orientation
            >> value.weight
            >> value.italic
            >> value.underline
            >> value.strike_out
            >> value.charset
            >> value.out_precision
            >> value.clip_precision
            >> value.quality
            >> value.pitch_family
            >> args(value.name, string_type_fixed_c<32>);

        spdlog::debug("Read font {:?}.", to_string(value.name));
    }

    static void load(Stream& stream, FontBank& value) {
        stream >> signature<"ATNF"> >> static_cast<std::vector<Font>&>(value);
        spdlog::debug("Read {} fonts.", value.size());
    }

    static void load(Stream& stream, Sound& value) {
        stream >> value.handle
            >> value.checksum
            >> value.references
            >> value.size
            >> value.flags
            >> value.frequency
            >> args(value.name, string_type_pascal_c);
        stream >> args(value.data, value.size - (value.flags[Sound::play_from_disk] ? 0 : (value.name.size() + 1) * 2));

        --value.handle;

        spdlog::debug("Read sound {:?}.", to_string(value.name));
    }

    static void load(Stream& stream, SoundBank& value) {
        stream >> signature<"APMS"> >> static_cast<std::vector<Sound>&>(value);
        spdlog::debug("Read {} sounds.", value.size());
    }

    static void load(Stream& stream, Music& value) {
        stream >> value.handle
            >> value.checksum
            >> value.references
            >> value.size
            >> value.flags
            >> value.frequency
            >> args(value.name, string_type_pascal_c);
        stream >> args(value.data, value.size - (value.name.size() + 1) * 2);

        spdlog::debug("Read music {:?}.", to_string(value.name));
    }

    static void load(Stream& stream, MusicBank& value) {
        stream >> signature<"ASUM"> >> static_cast<std::vector<Music>&>(value);
        spdlog::debug("Read {} music.", value.size());
    }

    static void load(Stream& stream, Image& value) {
        stream >> value.handle
            >> value.checksum
            >> value.references
            >> value.size
            >> value.width
            >> value.height
            >> value.graphic_mode
            >> value.flags
            >> skip<i16>
            >> value.hotspot_x
            >> value.hotspot_y
            >> value.action_x
            >> value.action_y
            >> value.transparent_color
            >> args(value.data, value.size);

        if (stream.build < 284) {
            ++value.handle;
        }
    }

    static void load(Stream& stream, ImageBank& value) {
        stream >> signature<"AGMI">
            >> value.graphic_mode
            >> value.palette_version
            >> args(value.palette, size_type<i16>)
            >> static_cast<std::vector<Image>&>(value);

        spdlog::debug("Read {} images.", value.size());
    }

    static void load(Stream& stream, BinaryFiles& value) {
        stream >> static_cast<std::vector<std::wstring>&>(value);
        spdlog::debug("Read {} binary files.", value.size());
    }

    static void load(Stream& stream, Control& value) {
        stream >> value.type >> value.data;
    }

    static void load(Stream& stream, Controls& value) {
        stream >> static_cast<std::vector<Control>&>(value);
        spdlog::debug("Read {} controls.", value.size());
    }

    static void load(Stream& stream, Qualifier& value) {
        stream >> value.name >> value.handle;
        spdlog::debug("Read qualifier {:?}.", to_string(value.name));
    }

    static void load(Stream& stream, Qualifiers& value) {
        stream >> static_cast<std::vector<Qualifier>&>(value);
        spdlog::debug("Read {} qualifiers.", value.size());
    }

    static void load(Stream& stream, Extension& value) {
        stream >> value.handle
            >> value.filename
            >> value.name
            >> value.magic_num
            >> value.subtype
            >> value.is_unicode;

        spdlog::debug("Read extension {:?}.", to_string(value.name));
    }

    static void load(Stream& stream, Extensions& value) {
        stream >> static_cast<std::vector<Extension>&>(value);
        spdlog::debug("Read {} extensions.", value.size());
    }

    static void load(Stream& stream, MenuItem& value) {
        stream >> value.flags;
        if (!value.flags[MenuItem::parent]) {
            stream >> value.id;
        }
        stream >> args(value.name, string_type_c);
        if (value.flags[MenuItem::parent]) {
            stream >> value.items;
        }
    }

    static void load(Stream& stream, MenuItems& value) {
        value.clear();

        MenuItem item;
        while (stream >> item, !item.flags[MenuItem::footer]) {
            value.emplace_back(std::move(item));
        }
    }

    static void load(Stream& stream, MenuAccel& value) {
        stream >> value.shift >> value.key >> value.id;
    }

    static void load(Stream& stream, Menu& value) {
        stream >> value.size;

        auto begin{ static_cast<usize>(stream.tellg()) };
        usize end{ begin + value.size };
        stream >> value.header_size
            >> value.item_offset
            >> value.item_size
            >> value.accel_offset
            >> value.accel_size;

        usize header_end{ begin + value.header_size };
        usize item_begin{ begin + value.item_offset };
        usize item_end{ item_begin + value.item_size };
        usize accel_begin{ begin + value.accel_offset };
        usize accel_end{ accel_begin + value.accel_size };
        if (stream.tellg() != header_end) {
            throw std::runtime_error{ "Corrupt menu header." };
        }

        stream.seekg(item_begin);
        stream >> skip<i32> >> value.items;
        if (stream.tellg() != item_end) {
            throw std::runtime_error{ "Corrupt menu items." };
        }

        stream.seekg(accel_begin);
        stream >> args(value.accels, value.accel_size / sizeof(MenuAccel));
        if (stream.tellg() != accel_end) {
            throw std::runtime_error{ "Corrupt menu accelerators." };
        }

        stream.seekg(end);
        stream >> value.window_menu >> value.images;

        spdlog::debug("Read a menu.");
    }

    static void load(Stream& stream, Value& value) {
        i32 type;
        stream >> value.name >> type;

        switch (type) {
        case Value::integer:
            stream >> value.value.emplace<i32>();
            break;
        case Value::decimal:
            stream >> value.value.emplace<f64>();
            break;
        case Value::string:
            stream >> value.value.emplace<std::wstring>();
            break;
        default:
            throw std::runtime_error{ std::format("Unknown value type {}.", type) };
        }
    }

    static void load(Stream& stream, Values& value) {
        stream >> static_cast<std::vector<Value>&>(value);
    }

    static void load(Stream& stream, GlobalEvents& value) {
        stream >> value.size;
        if (value.size != 0) {
            throw std::runtime_error{ std::format("Global events are unsupported at the moment.") };
        }
        spdlog::debug("Read {} global events.", value.size);
    }

    static void load(Stream& stream, Setting& value) {
        stream >> value.app_name
            >> value.author
            >> value.description
            >> value.copyright
            >> value.company
            >> value.version
            >> value.app_width
            >> value.app_height
            >> value.border_color
            >> value.display_flags
            >> value.graphic_flags
            >> value.help_file
            >> skip<std::wstring>
            >> value.score
            >> value.lives
            >> value.frame_rate
            >> value.build_type
            >> value.build_filename
            >> skip<std::wstring>
            >> skip<std::wstring>
            >> value.about
            >> skip<i32>
            >> value.binary_files
            >> value.controls
            >> value.menu
            >> value.global_numbers
            >> value.global_strings
            >> value.global_events
            >> value.graphic_mode
            >> value.icons
            >> value.qualifiers
            >> value.extensions;
    }

    static void load(Stream& stream, Layer& value) {
        stream >> value.name
            >> value.flags
            >> value.x_coefficient
            >> value.y_coefficient;

        spdlog::debug("Read layer {:?}.", to_string(value.name));
    }

    static void load(Stream& stream, Layers& value) {
        stream >> static_cast<std::vector<Layer>&>(value);
        spdlog::debug("Read {} layers.", value.size());
    }

    static void load(Stream& stream, Transition& value) {
        stream >> value.name
            >> value.module_name
            >> value.module
            >> value.id
            >> value.duration
            >> value.use_color
            >> value.color
            >> value.param;
    }

    static void load(Stream& stream, Movement& value) {
        stream >> value.name
            >> value.extension
            >> value.id
            >> value.data;
    }

    static void load(Stream& stream, Movements& value) {
        stream >> static_cast<std::vector<Movement>&>(value);
    }

    static void load(Stream& stream, Behavior& value) {
        stream >> value.name >> value.data;
    }

    static void load(Stream& stream, Behaviors& value) {
        stream >> static_cast<std::vector<Behavior>&>(value);
    }

    static void load(Stream& stream, Direction& value) {
        stream >> value.index
            >> value.max_speed
            >> value.min_speed
            >> value.repeat
            >> value.repeat_frame
            >> value.frames;
    }

    static void load(Stream& stream, Directions& value) {
        stream >> static_cast<std::vector<Direction>&>(value);
    }

    static void load(Stream& stream, Animation& value) {
        stream >> value.name >> value.directions;
    }

    static void load(Stream& stream, Animations& value) {
        stream >> static_cast<std::vector<Animation>&>(value);
    }

    static void load(Stream& stream, Paragraph& value) {
        stream >> value.value >> value.flags;
    }

    static void load(Stream& stream, Paragraphs& value) {
        stream >> static_cast<std::vector<Paragraph>&>(value);
    }

    static void load(Stream& stream, Content& value) {
        stream >> value.font
            >> value.color
            >> value.flags
            >> value.relief
            >> value.paragraphs;
    }

    static void load(Stream& stream, QuickBackdrop& value) {
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

    static void load(Stream& stream, Backdrop& value) {
        stream >> value.obstacle_type
            >> value.collision_type
            >> value.image;
    }

    static void load(Stream& stream, ObjectBase& value) {
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

    static void load(Stream& stream, ActiveObject& value) {
        stream >> static_cast<ObjectBase&>(value) >> value.animations;
    }

    static void load(Stream& stream, StringObject& value) {
        stream >> static_cast<ObjectBase&>(value)
            >> value.width
            >> value.height
            >> value.content;
    }

    static void load(Stream& stream, QuestionAnswerObject& value) {
        stream >> static_cast<ObjectBase&>(value)
            >> value.width
            >> value.height
            >> value.question
            >> value.answer;
    }

    static void load(Stream& stream, ScoreLivesObject& value) {
        stream >> static_cast<ObjectBase&>(value)
            >> value.player
            >> value.images
            >> value.use_text
            >> value.color
            >> value.font
            >> value.width
            >> value.height;
    }

    static void load(Stream& stream, CounterObject& value) {
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

    static void load(Stream& stream, FormattedTextObject& value) {
        stream >> static_cast<ObjectBase&>(value)
            >> value.width
            >> value.height
            >> value.flags
            >> value.color
            >> value.data;
    }

    static void load(Stream& stream, SubapplicationObject& value) {
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

    static void load(Stream& stream, ExtensionObject& value) {
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

    static void load(Stream& stream, Chunk& value) {
        stream >> value.id;
        if (value.id != 0) {
            stream >> value.data;
        }
    }

    static void load(Stream& stream, Chunks& value) {
        value.clear();

        Chunk chunk;
        while (stream >> chunk, chunk.id != 0) {
            value.emplace_back(std::move(chunk));
        }

        spdlog::debug("Read {} chunks.", value.size());
    }

    static void load(Stream& stream, Object& value) {
        stream >> value.type
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

        switch (value.type) {
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
        case Object::lives:
            stream >> value.emplace<ScoreLivesObject>();
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

    static void load(Stream& stream, Objects& value) {
        stream >> static_cast<std::vector<Object>&>(value);
        spdlog::debug("Read {} objects.", value.size());
    }

    static void load(Stream& stream, Folder& value) {
        stream >> value.header;
        if (((value.header & 0xff) == 112 ? value.header >> 24 : value.header) == 4) {
            stream >> value.name >> value.children;
            spdlog::debug("Read folder {:?}.", to_string(value.name));
        }
        else {
            stream >> skip<i32>;
        }
    }

    static void load(Stream& stream, Folders& value) {
        stream >> static_cast<std::vector<Folder>&>(value);
        spdlog::debug("Read {} folders.", value.size());
    }

    static void load(Stream& stream, Instance& value) {
        stream >> value.pos_x
            >> value.pos_y
            >> value.layer
            >> value.handle
            >> value.flags
            >> value.value
            >> value.parent_type
            >> value.object_info
            >> value.parent;
    }

    static void load(Stream& stream, Instances& value) {
        stream >> static_cast<std::vector<Instance>&>(value);
        spdlog::debug("Read {} instances.", value.size());
    }

    static void load(Stream& stream, Parameter& value) {
        stream >> value.size >> args(value.data, value.size - 2);
    }

    static void load(Stream& stream, Condition& value) {
        auto begin{ static_cast<usize>(stream.tellg()) };
        stream >> value.size
            >> value.object_type
            >> value.number
            >> value.object
            >> value.object_list
            >> value.flags
            >> value.other_flags
            >> value.params_num
            >> value.def_type
            >> value.id
            >> args(value.params, value.params_num);

        if (stream.tellg() != begin + value.size) {
            throw std::runtime_error{ "Corrupt condition." };
        }
    }

    static void load(Stream& stream, Action& value) {
        auto begin{ static_cast<usize>(stream.tellg()) };
        stream >> value.size
            >> value.object_type
            >> value.number
            >> value.object
            >> value.object_list
            >> value.flags
            >> value.other_flags
            >> value.params_num
            >> value.def_type
            >> args(value.params, value.params_num);

        if (stream.tellg() != begin + value.size) {
            throw std::runtime_error{ "Corrupt action." };
        }
    }

    static void load(Stream& stream, Event& value) {
        auto begin{ static_cast<usize>(stream.tellg()) };
        stream >> value.size
            >> value.condition_num
            >> value.action_num
            >> value.flags
            >> value.restricted
            >> value.restrict_cpt
            >> value.id
            >> value.undo
            >> args(value.conditions, value.condition_num)
            >> args(value.actions, value.action_num);

        if (stream.tellg() != begin + std::abs(value.size)) {
            throw std::runtime_error{ "Corrupt event." };
        }
    }

    static void load(Stream& stream, Events& value) {
        stream >> static_cast<std::vector<Event>&>(value);
        spdlog::debug("Read {} events.", value.size());
    }

    static void load(Stream& stream, Remark& value) {
        stream >> value.handle >> value.value;
    }

    static void load(Stream& stream, Remarks& value) {
        stream >> static_cast<std::vector<Remark>&>(value);
        spdlog::debug("Read {} remarks.", value.size());
    }

    static void load(Stream& stream, Group& value) {
        stream >> value.handle >> value.name >> value.uuid;
        spdlog::debug("Read group {:?}.", to_string(value.name));
    }

    static void load(Stream& stream, Groups& value) {
        stream >> static_cast<std::vector<Group>&>(value);
        spdlog::debug("Read {} groups.", value.size());
    }

    static void load(Stream& stream, EventObjectRef& value) {
        stream >> value.item_handle >> value.instance_handle;
    }

    static void load(Stream& stream, EventObjectIcon& value) {
        std::array<char, 4> buffer;
        stream >> buffer;

        std::string_view id{ buffer.data(), buffer.size() };
        if (id == "OIC2") {
            stream >> value.icon_buffer;
        }
    }

    static void load(Stream& stream, EventObjectQualifier& value) {
        stream >> value.system_qualifier;
    }

    static void load(Stream& stream, EventObject& value) {
        stream >> value.handle
            >> value.object_type
            >> value.item_type
            >> value.name
            >> value.type_name
            >> value.flags;

        switch (value.object_type) {
        case 1:
            stream >> value.data.emplace<EventObjectRef>();
            break;
        case 2:
            stream >> value.data.emplace<EventObjectIcon>();
            break;
        case 3:
            stream >> value.data.emplace<EventObjectQualifier>();
            break;
        default:
            throw std::runtime_error{ std::format("Unknown event object type {}.", value.object_type) };
        }

        spdlog::debug("Read event object {:?}.", to_string(value.name));
    }

    static void load(Stream& stream, EventObjects& value) {
        stream >> static_cast<std::vector<EventObject>&>(value);
        spdlog::debug("Read {} event objects.", value.size());
    }

    static void load(Stream& stream, EventItem& value) {
        stream >> value.type >> value.handle >> value.flags;
    }

    static void load(Stream& stream, EventItems& value) {
        stream >> static_cast<std::vector<EventItem>&>(value);
        spdlog::debug("Read {} event items.", value.size());
    }

    static void load(Stream& stream, EventsBlock& value) {
        stream >> value.size;

        auto end{ static_cast<usize>(stream.tellg()) + value.size };
        while (static_cast<usize>(stream.tellg()) < end) {
            Event event;
            stream >> event;
            value.data.emplace_back(std::move(event));
        }
        if (stream.tellg() != end) {
            throw std::runtime_error{ "Corrupt events block." };
        }
    }

    static void load(Stream& stream, RemarksBlock& value) {
        stream >> value.data;
    }

    static void load(Stream& stream, GroupsBlock& value) {
        stream >> value.len >> value.max_handles >> args(value.data, value.len);
    }

    static void load(Stream& stream, ObjectsBlock& value) {
        stream >> value.data;
    }

    static void load(Stream& stream, ConditionsBlock& value) {
        stream >> value.editor_data
            >> value.condition_width
            >> value.object_height
            >> skip<std::array<char, 12>>;
    }

    static void load(Stream& stream, DataBlock& value) {
        stream >> value.header;
        if (value.header == -1) {
            stream >> args(value.items, size_type<i16>);
            stream >> args(value.folders, size_type<i16>);
        }
        else {
            stream >> args(value.items, value.header);
        }
    }

    static void load(Stream& stream, TabsBlock& value) {
        stream >> skip<i16>
            >> value.editor_x
            >> value.editor_y
            >> value.editor_caret_type
            >> value.editor_caret_x
            >> value.editor_caret_y;
    }

    static void load(Stream& stream, LinesBlock& value) {
        stream >> skip<i16>
            >> value.editor_line_y
            >> value.editor_line_type
            >> value.event_line_y
            >> value.event_line_type;
    }

    static void load(Stream& stream, LayoutBlock& value) {
        stream >> value.data;
    }

    static void load(Stream& stream, EndBlock& value) {}

    static void load(Stream& stream, EventBlock& value) {
        std::array<char, 4> buffer;
        stream >> buffer;

        std::string_view id{ buffer.data(), buffer.size() };
        if (id == "Evts" || id == "STVE") {
            stream >> value.emplace<EventsBlock>();
        }
        else if (id == "Rems" || id == "SMER") {
            stream >> value.emplace<RemarksBlock>();
        }
        else if (id == "SPRG") {
            stream >> value.emplace<GroupsBlock>();
        }
        else if (id == "EvOb" || id == "SJBO") {
            stream >> value.emplace<ObjectsBlock>();
        }
        else if (id == "EvCs") {
            stream >> value.emplace<ConditionsBlock>();
        }
        else if (id == "EvEd") {
            stream >> value.emplace<DataBlock>();
        }
        else if (id == "EvTs") {
            stream >> value.emplace<TabsBlock>();
        }
        else if (id == "EvLs") {
            stream >> value.emplace<LinesBlock>();
        }
        else if (id == "E2Ts" || id == "TYAL") {
            stream >> value.emplace<LayoutBlock>();
        }
        else if (id == "!DNE") {
            stream >> value.emplace<EndBlock>();
        }
        else {
            throw std::runtime_error{ std::format("Unknown event block {:?}.", id) };
        }
    }

    static void load(Stream& stream, EventBlocks& value) {
        value.clear();

        stream >> value.data_size;
        if (value.data_size != 0) {
            EventBlock event;
            while (stream >> event, !std::holds_alternative<EndBlock>(event)) {
                value.emplace_back(std::move(event));
            }
        }

        spdlog::debug("Read {} event blocks.", value.size());
    }

    static void load(Stream& stream, Frame& value) {
        stream >> value.handle
            >> value.name
            >> value.width
            >> value.height
            >> value.background_color
            >> value.flags
            >> value.max_objects
            >> value.password
            >> skip<std::vector<char>>
            >> value.editor_x
            >> value.editor_y
            >> value.palette
            >> value.icon
            >> value.editor_layer
            >> value.layers
            >> value.transition_in
            >> value.transition_out
            >> value.objects
            >> value.folders
            >> value.instances
            >> value.events
            >> value.chunks;

        spdlog::debug("Read frame {:?}.", to_string(value.name));
    }

    static void load(Stream& stream, Frames& value) {
        value.clear();

        stream >> value.offsets >> value.end;

        for (u32 offset : value.offsets) {
            Frame frame;
            stream.seekg(offset);
            stream >> frame;
            value.emplace_back(std::move(frame));
        }

        stream.seekg(value.end);
        stream >> value.chunks;

        spdlog::debug("Read {} frames.", value.size());
    }

    class AutoTimer {
        Timer _timer;
        std::string_view _message;

    public:
        AutoTimer(std::string_view message) :
            _message{ message } {

            spdlog::info("Started {}.", _message);
        }

        ~AutoTimer() {
            spdlog::info("Finished {} in {:.3f} seconds.", _message, _timer.duration());
        }
    };

    export void load(Stream& stream, File& value) {
        AutoTimer _{ "parsing" };
        {
            AutoTimer _{ "reading the header" };
            stream >> value.header;
            stream.build = value.header.product_build;
        }
        {
            AutoTimer _{ "reading the font bank" };
            stream >> value.font_bank;
        }
        {
            AutoTimer _{ "reading the sound bank" };
            stream >> value.sound_bank;
        }
        {
            AutoTimer _{ "reading the music bank" };
            stream >> value.music_bank;
        }
        {
            AutoTimer _{ "reading the icon bank" };
            stream >> value.icon_bank;
        }
        {
            AutoTimer _{ "reading the image bank" };
            stream >> value.image_bank;
        }
        {
            AutoTimer _{ "reading settings" };
            stream >> value.setting;
        }
        {
            AutoTimer _{ "reading frames" };
            stream >> value.frames;
        }
    }

}
