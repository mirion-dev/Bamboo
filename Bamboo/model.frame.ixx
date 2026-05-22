export module bamboo.model:frame;

import :base;
import :object;
import :event;

namespace bamboo {

    export struct Layer {
        enum Flag {
            visible,
            locked,
            hidden_at_start = 3,
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
            locked = 2,
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
            screen_saver_setup = 7,
            timer_based_movements,
            dont_include_global_events = 10,
            dont_include               = 12,
            dont_erase_background,
            force_load_on_call_ignore = 15
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
