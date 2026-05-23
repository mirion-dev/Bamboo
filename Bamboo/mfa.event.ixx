module;

#include <spdlog/spdlog.h>

export module bamboo.mfa.event;

import std;
import bamboo.types;
import bamboo.diag;
import bamboo.stream;
import bamboo.model;
import bamboo.mfa.base;

namespace bamboo::mfa {

    export void load(Stream& stream, Parameter& value) {
        stream >> value.size >> args(value.data, value.size - 2);
    }

    export void load(Stream& stream, Condition& value) {
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

    export void load(Stream& stream, Action& value) {
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

    export void load(Stream& stream, Event& value) {
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

    export void load(Stream& stream, Events& value) {
        stream >> static_cast<std::vector<Event>&>(value);
        spdlog::debug("Read {} events.", value.size());
    }

    export void load(Stream& stream, Remark& value) {
        stream >> value.handle >> value.value;
    }

    export void load(Stream& stream, Remarks& value) {
        stream >> static_cast<std::vector<Remark>&>(value);
        spdlog::debug("Read {} remarks.", value.size());
    }

    export void load(Stream& stream, Group& value) {
        stream >> value.handle >> value.name >> value.uuid;
        spdlog::debug("Read group {:?}.", to_string(value.name));
    }

    export void load(Stream& stream, Groups& value) {
        stream >> static_cast<std::vector<Group>&>(value);
        spdlog::debug("Read {} groups.", value.size());
    }

    export void load(Stream& stream, EventObjectRef& value) {
        stream >> value.item_handle >> value.instance_handle;
    }

    export void load(Stream& stream, EventObjectIcon& value) {
        std::array<char, 4> buffer;
        stream >> buffer;

        std::string_view id{ buffer.data(), buffer.size() };
        if (id == "OIC2") {
            stream >> value.icon_buffer;
        }
    }

    export void load(Stream& stream, EventObjectQualifier& value) {
        stream >> value.system_qualifier;
    }

    export void load(Stream& stream, EventObject& value) {
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

    export void load(Stream& stream, EventObjects& value) {
        stream >> static_cast<std::vector<EventObject>&>(value);
        spdlog::debug("Read {} event objects.", value.size());
    }

    export void load(Stream& stream, EventItem& value) {
        stream >> value.type >> value.handle >> value.flags;
    }

    export void load(Stream& stream, EventItems& value) {
        stream >> static_cast<std::vector<EventItem>&>(value);
        spdlog::debug("Read {} event items.", value.size());
    }

    export void load(Stream& stream, EventsBlock& value) {
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

    export void load(Stream& stream, RemarksBlock& value) {
        stream >> value.data;
    }

    export void load(Stream& stream, GroupsBlock& value) {
        stream >> value.len >> value.max_handles >> args(value.data, value.len);
    }

    export void load(Stream& stream, ObjectsBlock& value) {
        stream >> value.data;
    }

    export void load(Stream& stream, ConditionsBlock& value) {
        stream >> value.editor_data
            >> value.condition_width
            >> value.object_height
            >> skip<std::array<char, 12>>;
    }

    export void load(Stream& stream, DataBlock& value) {
        stream >> value.header;
        if (value.header == -1) {
            stream >> args(value.items, size_type<i16>);
            stream >> args(value.folders, size_type<i16>);
        }
        else {
            stream >> args(value.items, value.header);
        }
    }

    export void load(Stream& stream, TabsBlock& value) {
        stream >> skip<i16>
            >> value.editor_x
            >> value.editor_y
            >> value.editor_caret_type
            >> value.editor_caret_x
            >> value.editor_caret_y;
    }

    export void load(Stream& stream, LinesBlock& value) {
        stream >> skip<i16>
            >> value.editor_line_y
            >> value.editor_line_type
            >> value.event_line_y
            >> value.event_line_type;
    }

    export void load(Stream& stream, LayoutBlock& value) {
        stream >> value.data;
    }

    export void load(Stream& stream, EndBlock& value) {}

    export void load(Stream& stream, EventBlock& value) {
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

    export void load(Stream& stream, EventBlocks& value) {
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

    export void load(Stream& stream, GlobalEventBlocks& value) {
        auto begin{ static_cast<usize>(stream.tellg()) };
        stream >> static_cast<EventBlocks&>(value);
        stream.seekg(begin + value.data_size + 4);
    }

}
