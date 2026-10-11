export module bamboo.mfa.event;

import std;
import bamboo.types;
import bamboo.utils;
import bamboo.log;
import bamboo.stream_utils;
import bamboo.model.event;
import bamboo.mfa.base;

namespace bamboo::mfa {

    export void load(Stream& stream, Parameter& value) {
        stream >> value.size >> args(value.data, value.size - sizeof(value.size)); // Unanalyzed
    }

    export void load(Stream& stream, EventEntry& value) {
        auto begin{ static_cast<usize>(stream.tellg()) };
        stream
            >> value.size
            >> value.object_type
            >> value.type
            >> value.object
            >> value.object_list
            >> value.flags
            >> value.flags2
            >> value.param_num
            >> value.def_type;

        if (std::to_underlying(value.type) < 0) {
            stream >> value.id;
        }

        stream >> args(value.params, value.param_num);

        if (stream.tellg() != begin + std::abs(value.size)) {
            throw std::runtime_error{ "Corrupt event entry" };
        }
    }

    export void load(Stream& stream, Event& value) {
        auto begin{ static_cast<usize>(stream.tellg()) };
        stream
            >> value.size
            >> value.condition_num
            >> value.action_num
            >> value.flags
            >> value.inhibit
            >> value.inhibit_cpt
            >> value.id
            >> value.undo
            >> args(value.conditions, value.condition_num)
            >> args(value.actions, value.action_num);

        if (stream.tellg() != begin + std::abs(value.size)) {
            throw std::runtime_error{ "Corrupt event" };
        }
    }

    export void load(Stream& stream, Events& value) {
        stream >> static_cast<std::vector<Event>&>(value);
        logger()->debug("Read {} events.", value.size());
    }

    export void load(Stream& stream, Remark& value) {
        stream >> value.handle >> value.string;
    }

    export void load(Stream& stream, Remarks& value) {
        stream >> static_cast<std::vector<Remark>&>(value);
        logger()->debug("Read {} remarks.", value.size());
    }

    export void load(Stream& stream, Group& value) {
        stream >> value.handle >> value.name >> value.uuid;
        logger()->debug("Read group {:?}.", to_string(value.name));
    }

    export void load(Stream& stream, Groups& value) {
        stream >> static_cast<std::vector<Group>&>(value);
        logger()->debug("Read {} groups.", value.size());
    }

    export void load(Stream& stream, EventObjectRef& value) {
        stream >> value.item_handle >> value.instance_handle;
    }

    export void load(Stream& stream, EventObjectIcon& value) {
        std::array<char, 4> buffer;
        stream >> buffer;

        std::string_view id{ buffer.data(), buffer.size() };
        if (id == "OIC2") { // Object ICon 2
            stream >> value.icon_buffer;
        }
    }

    export void load(Stream& stream, EventObjectQualifier& value) {
        stream >> value.system_qualifier;
    }

    export void load(Stream& stream, EventObject& value) {
        stream >> value.handle >> value.object_type >> value.item_type >> value.name >> value.type_name >> value.flags;
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
            throw std::runtime_error{ std::format("Unknown event object type {}", value.object_type) };
        }

        logger()->debug("Read event object {:?}.", to_string(value.name));
    }

    export void load(Stream& stream, EventObjects& value) {
        stream >> static_cast<std::vector<EventObject>&>(value);
        logger()->debug("Read {} event objects.", value.size());
    }

    export void load(Stream& stream, EventsBlock& value) {
        stream >> value.size;

        auto end{ static_cast<usize>(stream.tellg()) + value.size };
        while (static_cast<usize>(stream.tellg()) < end) {
            stream >> value.data.emplace_back();
        }
        if (stream.tellg() != end) {
            throw std::runtime_error{ "Corrupt events block" };
        }
    }

    export void load(Stream& stream, RemarksBlock& value) {
        stream >> value.data;
    }

    export void load(Stream& stream, ObjectsBlock& value) {
        stream >> value.data;
    }

    export void load(Stream& stream, ColumnsBlock& value) {
        stream >> value.editor_data >> value.condition_width >> value.object_height >> skip<std::array<char, 12>>;
    }

    export void load(Stream& stream, TablesBlock& value) {
        stream
            >> skip<u16> >> value.editor_x
            >> value.editor_y
            >> value.editor_caret_type
            >> value.editor_caret_x
            >> value.editor_caret_y;
    }

    export void load(Stream& stream, LinesBlock& value) {
        stream
            >> skip<u16> >> value.editor_line_y
            >> value.editor_line_type
            >> value.event_line_y
            >> value.event_line_type;
    }

    export void load(Stream& stream, BookmarksBlock& value) {
        stream >> value.data;
    }

    export void load(Stream& stream, EditorBlock& value) {
        u16 count;
        stream >> value.header;
        if (value.header == static_cast<u16>(-1)) {
            stream >> count;
        } else {
            count = value.header;
        }

        stream >> args(value.types, count) >> args(value.handles, count) >> args(value.flags, count);
        if (value.header == static_cast<u16>(-1)) {
            stream >> args(value.folders, type<u16>);
        }
    }

    export void load(Stream& stream, TemporaryBlock& value) {
        throw std::runtime_error{ "Unimplemented" };
    }

    export void load(Stream& stream, HeaderBlock& value) {
        stream
            >> value.version
            >> skip<u16> // Unknown; seems to always be 1
            >> value.flags;
    }

    export void load(Stream& stream, ObjectNameBlock& value) {
        throw std::runtime_error{ "Unimplemented" };
    }

    export void load(Stream& stream, GroupsBlock& value) {
        stream >> value.data_size >> value.max_handles >> args(value.data, value.data_size);
    }

    export void load(Stream& stream, LayoutBlock& value) {
        stream >> value.data;
    }

    export void load(Stream& stream, FrameEventBlock& value) {
        std::array<char, 4> buffer;
        stream >> buffer;

        std::string_view id{ buffer.data(), buffer.size() };
        std::string_view info;
        if (id == "Evts") {
            stream >> value.emplace<EventsBlock>();
            info = "an events block";
        } else if (id == "Rems") {
            stream >> value.emplace<RemarksBlock>();
            info = "a remarks block";
        } else if (id == "EvOb") {
            stream >> value.emplace<ObjectsBlock>();
            info = "an objects block";
        } else if (id == "EvCs") {
            stream >> value.emplace<ColumnsBlock>();
            info = "a columns block";
        } else if (id == "EvTs") {
            stream >> value.emplace<TablesBlock>();
            info = "a tables block";
        } else if (id == "EvLs") {
            stream >> value.emplace<LinesBlock>();
            info = "a lines block";
        } else if (id == "E2Ts") {
            stream >> value.emplace<BookmarksBlock>();
            info = "a bookmarks block";
        } else if (id == "EvEd") {
            stream >> value.emplace<EditorBlock>();
            info = "an editor block";
        } else if (id == "EvTe") {
            stream >> value.emplace<TemporaryBlock>();
            info = "a temporary block";
        } else if (id == "!DNE") { // END!
            value.emplace<EndBlock>();
            info = "an end block";
        } else {
            throw std::runtime_error{ std::format("Unknown event block {:?}", id) };
        }

        logger()->debug("Read {}.", info);
    }

    export void load(Stream& stream, FrameEvents& value) {
        value.clear();

        stream >> value.version >> skip<u16>; // Unknown; seems to always be 0

        FrameEventBlock block;
        while (stream >> block, !std::holds_alternative<EndBlock>(block)) {
            value.emplace_back(std::move(block));
        }

        logger()->debug("Read {} event blocks.", value.size());
    }

    export void load(Stream& stream, GlobalEventBlock& value) {
        std::array<char, 4> buffer;
        stream >> buffer;

        std::string_view id{ buffer.data(), buffer.size() };
        std::string_view info;
        if (id == "DAEH") { // HEADer
            stream >> value.emplace<HeaderBlock>();
            info = "a header block";
        } else if (id == "NJBO") { // OBJect Name
            stream >> value.emplace<ObjectNameBlock>();
            info = "an object name block";
        } else if (id == "STVE") { // EVenTS
            stream >> value.emplace<EventsBlock>();
            info = "an events block";
        } else if (id == "SMER") { // REMarkS
            stream >> value.emplace<RemarksBlock>();
            info = "a remarks block";
        } else if (id == "SJBO") { // OBJectS
            stream >> value.emplace<ObjectsBlock>();
            info = "an objects block";
        } else if (id == "SPRG") { // GRouPS
            stream >> value.emplace<GroupsBlock>();
            info = "a groups block";
        } else if (id == "TYAL") { // LAYouT
            stream >> value.emplace<LayoutBlock>();
            info = "a layout block";
        } else if (id == "!DNE") { // END!
            value.emplace<EndBlock>();
            info = "an end block";
        } else {
            throw std::runtime_error{ std::format("Unknown global event block {:?}", id) };
        }

        logger()->debug("Read {}.", info);
    }

    export void load(Stream& stream, GlobalEvents& value) {
        value.clear();

        stream >> value.size;
        if (value.size == 0) {
            return;
        }

        auto begin{ static_cast<usize>(stream.tellg()) };
        stream >> signature<"KLBE">; // Event BLocK

        GlobalEventBlock block;
        while (stream >> block, !std::holds_alternative<EndBlock>(block)) {
            value.emplace_back(std::move(block));
        }

        if (stream.tellg() != begin + value.size) {
            throw std::runtime_error{ "Corrupt global events" };
        }

        logger()->debug("Read {} global event blocks.", value.vector::size());
    }

}
