export module bamboo.model.event;

import std;
import bamboo.types;
import bamboo.model.base;

namespace bamboo {

    export struct Parameter {
        i16 size;
        std::vector<char> data;
    };

    export struct Parameters : std::vector<Parameter> {};

    export struct Condition {
        i16 size;
        i16 object_type;
        i16 number;
        u16 object;
        i16 object_list;
        u8 flags;
        u8 other_flags;
        i8 params_num;
        i8 def_type;
        i16 id;
        Parameters params;
    };

    export struct Conditions : std::vector<Condition> {};

    export struct Action {
        i16 size;
        i16 object_type;
        i16 number;
        u16 object;
        i16 object_list;
        u8 flags;
        u8 other_flags;
        i8 params_num;
        i8 def_type;
        Parameters params;
    };

    export struct Actions : std::vector<Action> {};

    export struct Event {
        enum Flag {
            once,
            not_always,
            repeat,
            no_more,
            shuffle,
            editor_mark,
            has_children,
            break_,
            break_point,
            always_clean,
            has_or,
            has_stop,
            has_or_logical,
            grouped,
            inactive,
            has_parent
        };

        i16 size;
        i8 condition_num;
        i8 action_num;
        Flags<u16> flags;
        i16 restricted;
        i16 restrict_cpt;
        i16 id;
        i16 undo;
        Conditions conditions;
        Actions actions;
    };

    export struct Events : std::vector<Event> {};

    export struct Remark {
        u32 handle;
        std::wstring value;
    };

    export struct Remarks : std::vector<Remark> {};

    export struct Group {
        u32 handle;
        std::wstring name;
        std::array<char16_t, 75> uuid;
    };

    export struct Groups : std::vector<Group> {};

    export struct EventObjectRef {
        u32 item_handle;
        u32 instance_handle;
    };

    export struct EventObjectIcon {
        std::vector<char> icon_buffer;
    };

    export struct EventObjectQualifier {
        u16 system_qualifier;
    };

    export struct EventObject {
        u32 handle;
        i16 object_type;
        i16 item_type;
        std::wstring name;
        std::wstring type_name;
        Flags<u16> flags;
        std::variant<EventObjectRef, EventObjectIcon, EventObjectQualifier> data;
    };

    export struct EventObjects : std::vector<EventObject> {};

    export struct EventItem {
        i16 type;
        u16 handle;
        Flags<u16> flags;
    };

    export struct EventItems : std::vector<EventItem> {};

    export struct EventsBlock {
        i32 size;
        Events data;
    };

    export struct RemarksBlock {
        Remarks data;
    };

    export struct GroupsBlock {
        i32 len;
        i32 max_handles;
        Groups data;
    };

    export struct ObjectsBlock {
        EventObjects data;
    };

    export struct ConditionsBlock {
        i32 editor_data;
        i16 condition_width;
        i16 object_height;
    };

    export struct DataBlock {
        i16 header;
        EventItems items;
        std::vector<std::wstring> folders;
    };

    export struct TabsBlock {
        i32 editor_x;
        i32 editor_y;
        i32 editor_caret_type;
        i32 editor_caret_x;
        i32 editor_caret_y;
    };

    export struct LinesBlock {
        i32 editor_line_y;
        i32 editor_line_type;
        i32 event_line_y;
        i32 event_line_type;
    };

    export struct LayoutBlock {
        std::vector<char> data;
    };

    export struct EndBlock {};

    export struct EventBlock : std::variant<
            EventsBlock,
            RemarksBlock,
            GroupsBlock,
            ObjectsBlock,
            ConditionsBlock,
            DataBlock,
            TabsBlock,
            LinesBlock,
            LayoutBlock,
            EndBlock
        > {};

    export struct EventBlocks : std::vector<EventBlock> {
        i32 data_size;
    };

    export struct GlobalEventBlocks : EventBlocks {};

}
