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
            not_visible_at_start,
            do_not_save_background,
            wrap_horizontally,
            wrap_vertically,
            same_effect_as_previous_layer
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
            grab_desktop_at_start,
            keep_display_from_previous_frame,
            handle_background_collisions_even_out_of_window,
            display_frame_title_in_window_caption,
            resize_to_screen_size_at_start,
            force_load_on_call_option_for_all_objects_force,
            _6,
            screen_saver_setup_frame,
            timer_based_movements,
            _9,
            do_not_include_global_events,
            _11,
            do_not_include_at_build_time,
            direct3d_do_not_erase_background_if_the_frame_has_an_effect,
            _14,
            force_load_on_call_option_for_all_objects_ignore
        };

        u32 handle;
        std::wstring name;
        i32 width;
        i32 height;
        Color background_color;
        Flags<u32> flags;
        i32 object_num;
        std::wstring password;
        i32 editor_x;
        i32 editor_y;
        std::vector<Color> palette;
        i32 icon;
        i32 editor_layer;
        Layers layers;
        std::optional<Transition> fade_in;
        std::optional<Transition> fade_out;
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
