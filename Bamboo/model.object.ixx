export module bamboo.model.object;

import std;
import bamboo.types;
import bamboo.model.base;

namespace bamboo {

    export struct Transition {
        std::wstring filename;
        std::wstring module_name;
        i32 module;
        std::array<char, 4> id;
        i32 duration;
        i32 use_color;
        Color color;
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
        i32 obstacle_type;
        i32 collision_type;
        u32 image;
    };

    export struct ObjectBase {
        enum Flag {
            display_in_front,
            background,
            save_background,
            run_before_fade_in,
            has_movements,
            has_animations,
            tab_stop,
            window_process,
            has_alterables,
            has_sprites,
            internal_save_background,
            dont_follow_frame,
            display_as_background,
            dont_destroy_if_too_far,
            dont_inactivate_if_too_far,
            inactivate_if_too_far,
            has_text,
            create_at_start,
            ccn_check,
            _19,
            dont_reset_frame_duration
        };

        enum NewFlag {
            dont_save_background,
            wipe_with_color,
            dont_use_fine_detection,
            visible_at_start,
            solid_obstacle,
            platform_obstacle,
            ladder_obstacle,
            automatic_rotations,
            initialize_flags
        };

        Flags<u32> flags;
        Flags<u32> new_flags;
        Color background_color;
        std::array<i16, 8> qualifiers;
        Values numbers;
        Values strings;
        Movements movements;
        Behaviors behaviors;
        std::optional<Transition> transition_in;
        std::optional<Transition> transition_out;
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
            load_on_call,
            _1,
            global_object,
            _3,
            no_editor_sync,
            name_type_editor_sync,
            _6,
            no_auto_update
        };

        u32 handle;
        std::wstring name;
        i32 transparent;
        i32 ink_effect;
        i32 ink_effect_param;
        i32 anti_aliasing;
        Flags<u32> flags;
        i32 icon_type;
        u32 icon;
        Chunks chunks;
    };

    export struct Objects : std::vector<Object> {};

}
