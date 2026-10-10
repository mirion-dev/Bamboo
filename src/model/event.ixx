export module bamboo.model.event;

import std;
import bamboo.types;
import bamboo.model.base;

namespace bamboo {

    export struct Parameter {
        u16 size;
        std::vector<unsigned char> data;
    };

    export struct Parameters : std::vector<Parameter> {};

    export struct Condition {
        u16 size;
        u16 object_type;
        i16 number;
        u16 object;
        i16 object_list;
        u8 flags;
        u8 other_flags;
        u8 params_num;
        u8 def_type;
        u16 id;
        Parameters params;
    };

    export struct Conditions : std::vector<Condition> {};

    export struct Action {
        u16 size;
        u16 object_type;
        i16 number;
        u16 object;
        i16 object_list;
        u8 flags;
        u8 other_flags;
        u8 params_num;
        u8 def_type;
        Parameters params;
    };

    export struct Actions : std::vector<Action> {};

    // eventGroup (SDK)
    export struct Event {
        // EVGFLAGS_* (SDK)
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
            or_in_group,
            stop_in_group,
            or_logical,
            grouped,
            inactive,
            has_parent
        };

        i16 size;
        u8 condition_num;
        u8 action_num;
        Flags<u16> flags;
        i16 inhibit;
        u16 inhibit_cpt;
        u16 id;
        i16 undo;
        Conditions conditions;
        Actions actions;
    };

    export struct Events : std::vector<Event> {};

    // CEvtRemark
    export struct Remark {
        u32 handle;
        std::wstring string;
    };

    // CEvtRemarkList
    export struct Remarks : std::vector<Remark> {};

    export struct Group {
        u32 handle;
        std::wstring name;
        std::array<wchar_t, 75> uuid;
    };

    export struct Groups : std::vector<Group> {};

    export struct EventObjectRef {
        u32 item_handle;
        u32 instance_handle;
    };

    export struct EventObjectIcon {
        std::vector<unsigned char> icon_buffer;
    };

    export struct EventObjectQualifier {
        u16 system_qualifier;
    };

    // COI
    export struct EventObject {
        u32 handle;
        u16 object_type;
        u16 item_type;
        std::wstring name;
        std::wstring type_name;
        Flags<u16> flags;
        std::variant<EventObjectRef, EventObjectIcon, EventObjectQualifier> data;
    };

    // COIList
    export struct EventObjects : std::vector<EventObject> {};

    export struct EventsBlock {
        u32 size;
        Events data;
    };

    export struct RemarksBlock {
        Remarks data;
    };

    export struct ObjectsBlock {
        EventObjects data;
    };

    export struct ColumnsBlock {
        i32 editor_data;
        i16 condition_width;
        i16 object_height;
    };

    export struct TablesBlock {
        i32 editor_x;
        i32 editor_y;
        u32 editor_caret_type;
        i32 editor_caret_x;
        i32 editor_caret_y;
    };

    export struct LinesBlock {
        i32 editor_line_y;
        u32 editor_line_type;
        i32 event_line_y;
        u32 event_line_type;
    };

    export struct BookmarksBlock {
        std::vector<unsigned char> data;
    };

    export struct EditorBlock {
        u16 header;
        std::vector<u16> types;
        std::vector<u16> handles;
        std::vector<Flags<u16>> flags;
        std::vector<std::wstring> folders;
    };

    export struct TemporaryBlock {
        // Unanalyzed
    };

    export struct HeaderBlock {
        // Unanalyzed
    };

    export struct ObjectNameBlock {
        // Unanalyzed
    };

    export struct GroupsBlock {
        u32 data_size;
        u32 max_handles;
        Groups data;
    };

    export struct LayoutBlock {
        std::vector<unsigned char> data;
    };

    export struct EndBlock {};

    export struct FrameEventBlock : std::variant<
                                        EventsBlock,
                                        RemarksBlock,
                                        ObjectsBlock,
                                        ColumnsBlock,
                                        TablesBlock,
                                        LinesBlock,
                                        BookmarksBlock,
                                        EditorBlock,
                                        TemporaryBlock,
                                        EndBlock
                                    > {};

    export struct FrameEvents : std::vector<FrameEventBlock> {
        u32 size;
    };

    export struct GlobalEventBlock : std::variant<
                                         HeaderBlock,
                                         ObjectNameBlock,
                                         EventsBlock,
                                         RemarksBlock,
                                         ObjectsBlock,
                                         GroupsBlock,
                                         LayoutBlock,
                                         EndBlock
                                     > {};

    export struct GlobalEvents : std::vector<GlobalEventBlock> {
        u32 size;
    };

}
