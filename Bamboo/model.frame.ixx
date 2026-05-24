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
            not_visible_at_start,         // Settings
            do_not_save_background,       // Settings
            wrap_horizontally,            // Settings
            wrap_vertically,              // Settings
            same_effect_as_previous_layer // Settings
        };

        std::wstring name; // Settings
        Flags<u32> flags;
        f32 x_coefficient; // Settings
        f32 y_coefficient; // Settings
    };

    export struct Layers : std::vector<Layer> {};

    export struct Folder {
        Flags<u32> flags;
        std::wstring name;
        std::vector<u32> objects;
    };

    export struct Folders : std::vector<Folder> {};

    export struct Instance {
        enum Flag {
            _0,
            _1,
            locked,
            fake
        };

        i32 x; // Size / Position
        i32 y; // Size / Position
        u32 layer;
        u32 handle;
        Flags<u16> flags;
        i16 value; // Values
        i32 parent_type;
        u32 object;
        u32 parent;
    };

    export struct Instances : std::vector<Instance> {};

    export struct Frame {
        enum Flag {
            grab_desktop_at_start,                           // Runtime
            keep_display_from_previous_frame,                // Runtime
            handle_background_collisions_even_out_of_window, // Runtime
            display_frame_title_in_window_caption,           // Runtime
            resize_to_screen_size_at_start,                  // Runtime
            force_load_on_call_option_for_all_objects_force, // Runtime
            _6,
            screen_saver_setup_frame, // Runtime
            timer_based_movements,    // Runtime
            _9,
            do_not_include_global_events, // Runtime
            _11,
            do_not_include_at_build_time,                                // Settings
            direct3d_do_not_erase_background_if_the_frame_has_an_effect, // Runtime
            _14,
            force_load_on_call_option_for_all_objects_ignore // Runtime
        };

        u32 handle;
        std::wstring name;      // About
        i32 width;              // Settings
        i32 height;             // Settings
        Color background_color; // Settings
        Flags<u32> flags;
        i32 object_num;        // Runtime
        std::wstring password; // Runtime
        i32 editor_x;
        i32 editor_y;
        std::vector<Color> palette; // Settings
        i32 icon;
        i32 editor_layer;
        Layers layers;
        std::optional<Transition> fade_in;  // Settings
        std::optional<Transition> fade_out; // Settings
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
