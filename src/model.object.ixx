export module bamboo.model.object;

import std;
import bamboo.types;
import bamboo.model.base;

namespace bamboo {

    export struct Transition {
        enum Flag {
            _0,
            use_color, // From / To
            unicode
        };

        std::wstring filename;
        std::wstring name; // Transition
        i32 dll_handle;
        std::array<char, 4> id;
        i32 duration; // Duration
        Flags<u32> flags;
        Color color; // From / To
        std::vector<char> param;
    };

    export struct Movement {
        std::wstring name;
        std::wstring extension;
        i32 id;
        std::vector<char> data;
    };

    export struct Movements : std::vector<Movement> {};

    export struct Behavior {
        std::wstring name;
        std::vector<char> data;
    };

    export struct Behaviors : std::vector<Behavior> {};

    export struct Direction {
        i32 index;
        i32 max_speed;
        i32 min_speed;
        i32 repeat;
        i32 repeat_frame;
        std::vector<u32> frames;
    };

    export struct Directions : std::vector<Direction> {};

    export struct Animation {
        std::wstring name;
        Directions directions;
    };

    export struct Animations : std::vector<Animation> {};

    export struct Paragraph {
        enum Flag {
            mfa_correct,
            _1,
            _2,
            _3,
            _4,
            _5,
            _6,
            _7,
            correct,
            relief
        };

        std::wstring value;
        Flags<u32> flags;
    };

    export struct Paragraphs : std::vector<Paragraph> {};

    export struct Content {
        u32 font;
        Color color;
        Flags<u32> flags;
        i32 relief;
        Paragraphs paragraphs;
    };

    export struct QuickBackdrop {
        enum Flag {
            vertical_gradient,
            integral_dimensions
        };

        i32 obstacle_type;
        i32 collision_type;
        i32 width;
        i32 height;
        i32 shape;
        i32 border_size;
        Color border_color;
        i32 fill_type;
        Color color1;
        Color color2;
        Flags<u32> flags;
        u32 image;
    };

    export struct Backdrop {
        enum class ObstacleType : i32 {
            none,
            solid,
            platform,
            ladder,
            transparent
        };

        ObstacleType obstacle_type; // Runtime
        u32 collision_with_box;     // Runtime
        u32 image;                  // Settings
    };

    export struct ObjectBase {
        enum Flag {
            display_in_front,
            background,
            save_background, // Display
            run_before_fade_in,
            has_movements,
            has_animations,
            tab_stop_focus,
            is_window_process,
            has_alterables_values_strings_flags,
            uses_images,
            internal_save_background,
            do_not_follow_the_frame,                     // Runtime
            display_as_background,                       // Display
            do_not_destroy_object_if_too_far_from_frame, // Runtime
            inactivate_if_too_far_from_window_no,        // Runtime
            inactivate_if_too_far_from_window_yes,       // Runtime
            uses_text,
            create_at_start, // Runtime
            _18,
            _19,
            do_not_reset_current_frame_duration_when_the_animation_is_modified // Runtime
        };

        enum NewFlag {
            do_not_save_background,    // Display
            wipe_with_color,           // Display
            do_not_use_fine_detection, // Runtime
            visible_at_start,          // Display
            obstacle_type_obstacle,    // Runtime
            obstacle_type_platform,    // Runtime
            obstacle_type_ladder,      // Runtime
            automatic_rotations,       // Runtime
            initialize_flags
        };

        Flags<u32> flags;
        Flags<u32> new_flags;
        Color background_color;             // Display
        std::array<i16, 8> qualifiers;      // Events
        Values values;                      // Values
        Values strings;                     // Values
        Movements movements;                // Movement
        Behaviors behaviors;                // Events
        std::optional<Transition> fade_in;  // Display
        std::optional<Transition> fade_out; // Display
    };

    export struct ActiveObject : ObjectBase {
        std::optional<Animations> animations;
    };

    export struct StringObject : ObjectBase {
        i32 width;
        i32 height;
        Content content;
    };

    export struct QuestionAnswerObject : ObjectBase {
        i32 width;
        i32 height;
        Content question;
        Content answer;
    };

    export struct ScoreLivesObject : ObjectBase {
        i32 player;
        std::vector<u32> images;
        i32 use_text;
        Color color;
        u32 font;
        i32 width;
        i32 height;
    };

    export struct ScoreObject : ScoreLivesObject {};

    export struct LivesObject : ScoreLivesObject {};

    export struct CounterObject : ObjectBase {
        i32 value;
        i32 min;
        i32 max;
        i32 display_type;
        i32 fill_type;
        Color color1;
        Color color2;
        i32 vertical_gradient;
        i32 bar_direction;
        i32 width;
        i32 height;
        std::vector<u32> images;
        u32 font;
    };

    export struct FormattedTextObject : ObjectBase {
        enum Flag {
            _0,
            auto_scrollbar
        };

        i32 width;
        i32 height;
        Flags<u32> flags;
        Color color;
        std::vector<char> data;
    };

    export struct SubapplicationObject : ObjectBase {
        enum Flag {
            share_global_values,
            share_player_lives,
            share_player_scores,
            share_window_attributes,
            stretch,
            popup,
            caption,
            tool_caption,
            border,
            resize_window,
            system_menu,
            disable_close,
            modal,
            dialogue_frame,
            internal,
            hide_on_close,
            custom_size,
            internal_about_box,
            clip_siblings,
            share_player_controls,
            mdi,
            docked,
            mfa_check,
            docked_vertical,
            docked_horizontal,
            reopen,
            sprite,
            ignore_resize
        };

        std::wstring name;
        i32 width;
        i32 height;
        Flags<u32> flags;
        i32 start_frame;
    };

    export struct ExtensionObject : ActiveObject {
        i32 type;
        std::wstring name;
        std::wstring filename;
        i32 magic_num;
        std::wstring subtype;
        i32 real_size;
        i32 size;
        i32 version;
        i32 id;
        i32 private_data;
        std::vector<char> data;
    };

    export struct Chunk {
        i8 id;
        std::vector<char> data;
    };

    export struct Chunks : std::vector<Chunk> {};

    export struct Object : std::variant<
                               QuickBackdrop,
                               Backdrop,
                               ActiveObject,
                               StringObject,
                               QuestionAnswerObject,
                               ScoreObject,
                               LivesObject,
                               CounterObject,
                               FormattedTextObject,
                               SubapplicationObject,
                               ExtensionObject
                           > {

        enum Flag {
            load_on_call, // Runtime
            _1,
            global_object, // Runtime
            _3,
            editor_synchronization_no,                 // Runtime
            editor_synchronization_same_name_and_type, // Runtime
            _6,
            do_not_auto_update // About
        };

        u32 extension;
        u32 handle;
        std::wstring name; // About
        u32 transparent;   // Display
        i32 ink_effect;
        i32 ink_effect_param;
        u32 antialiasing; // Display
        Flags<u32> flags;
        u32 icon; // About
        Chunks chunks;
    };

    export struct Objects : std::vector<Object> {};

}
