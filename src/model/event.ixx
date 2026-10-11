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

    // event (SDK)
    export struct EventEntry {
        // EVFLAGS_* (SDK)
        enum Flag {
            repeat,
            done,
            default_,
            done_before_fade_in,
            not_done_in_start,
            always,
            bad,
            bad_object
        };

        // EVFLAGS_*, EVFLAGS2_* (SDK)
        enum Flag2 {
            negated,
            negatable,
            monitorable,
            to_delete,
            new_sound,
            no_interdependence
        };

        // CNDL_*, ACTL_* (SDK)
        enum class Type : i16 {
            // TODO
        };

        i16 size;
        i16 object_type;
        Type type;
        i16 object;
        i16 object_list;
        Flags<u8> flags;
        Flags<u8> flags2;
        u8 param_num;
        u8 def_type;
        u16 id;
        Parameters params;
    };

    export struct EventEntries : std::vector<EventEntry> {};

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
        EventEntries conditions;
        EventEntries actions;
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
        u16 version;
        Flags<u32> flags;
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
        u16 version;
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
