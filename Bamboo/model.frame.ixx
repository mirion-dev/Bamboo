export module bamboo.model.frame;

import std;
import bamboo.types;
import bamboo.model.base;
import bamboo.model.object;
import bamboo.model.event;

namespace bamboo {

    export struct Layer {
        enum Flag {
            visible,
            locked,
            _2,
            hidden_at_start,
            dont_save_background,
            wrap_horizontally,
            wrap_vertically,
            prev_effect
        };

        std::wstring name;
        Flags<u32> flags;
        f32 x_coefficient;
        f32 y_coefficient;
    };

    export struct Layers : std::vector<Layer> {};

    export struct Folder {
        u32 header;
        std::wstring name;
        std::vector<i32> children;
    };

    export struct Folders : std::vector<Folder> {};

    export struct Instance {
        enum Flag {
            _0,
            _1,
            locked,
            create_only
        };

        i32 pos_x;
        i32 pos_y;
        u32 layer;
        u32 handle;
        Flags<u16> flags;
        i16 value;
        i32 parent_type;
        i32 object_info;
        u32 parent;
    };

    export struct Instances : std::vector<Instance> {};

    export struct Frame {
        enum Flag {
            grab_desktop,
            keep_display,
            handle_background_collisions,
            display_title,
            resize_to_screen,
            force_load_on_call,
            _6,
            screen_saver_setup,
            timer_based_movements,
            _9,
            dont_include_global_events,
            _11,
            dont_include,
            dont_erase_background,
            _14,
            force_load_on_call_ignore
        };

        u32 handle;
        std::wstring name;
        i32 width;
        i32 height;
        Color background_color;
        Flags<u32> flags;
        i32 max_objects;
        std::wstring password;
        i32 editor_x;
        i32 editor_y;
        std::vector<Color> palette;
        i32 icon;
        i32 editor_layer;
        Layers layers;
        std::optional<Transition> transition_in;
        std::optional<Transition> transition_out;
        Objects objects;
        Folders folders;
        Instances instances;
        EventBlocks events;
        Chunks chunks;
    };

    export struct Frames : std::vector<Frame> {
        std::vector<u32> offsets;
        u32 end;
        Chunks chunks;
    };

}
