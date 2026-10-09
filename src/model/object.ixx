export module bamboo.model.object;

import std;
import bamboo.types;
import bamboo.model.base;

namespace bamboo {

    export struct Chunk {
        u8 id;
        std::vector<unsigned char> data;
    };

    export struct Chunks : std::vector<Chunk> {};

    // CItemValue
    export struct Value : std::variant<i32, f64, std::wstring> {
        std::wstring name;
    };

    // CItemValueArray
    export struct Values : std::vector<Value> {};

    // CMovement
    export struct Movement {
        std::wstring name;
        std::wstring extension_name;
        u32 id;
        std::vector<unsigned char> data;
    };

    // CMovementArray
    export struct Movements : std::vector<Movement> {};

    // CBehavior
    export struct Behavior {
        std::wstring name;
        std::vector<unsigned char> data;
    };

    // CBehaviorArray
    export struct Behaviors : std::vector<Behavior> {};

    export struct Transition {
        enum Flag {
            _0,
            use_color,
            unicode
        };

        std::wstring filename;
        std::wstring name;
        i32 file_handle;
        std::array<char, 4> id;
        i32 duration;
        Flags<u32> flags;
        Color color;
        std::vector<unsigned char> param;
    };

    // CDirection
    export struct Direction {
        i32 index;
        i32 max_speed;
        i32 min_speed;
        i32 repeat;
        i32 repeat_from;
        std::vector<u32> images;
    };

    // CDirSet
    export struct Directions : std::vector<Direction> {};

    // CAnimation
    export struct Animation {
        std::wstring name;
        Directions directions;
    };

    // CAnimSet
    export struct Animations : std::vector<Animation> {};

    // extHeader (SDK)
    export struct ExtensionHeader {
        using unpadded = void;

        u32 size;
        u32 max_size;
        u32 version;
        u32 id;
        u32 private_data;
    };

    // CText
    export struct Text {
        std::wstring string;
        u32 is_correct;
    };

    export struct Texts : std::vector<Text> {};

    // CTextGroup
    export struct Content {
        enum Flag {
            alignment_horizontal_center,
            alignment_horizontal_right,
            alignment_vertical_center,
            alignment_vertical_bottom,
            correct,
            relief,
            right_to_left
        };

        u32 font;
        Color color;
        Flags<u32> flags;
        u32 is_relief;
        Texts texts;
    };

    // CFrameItem
    export struct ObjectBase {
        enum Flag {
            load_on_call,  // Runtime
            discardable,   // OIF_* (SDK)
            global_object, // Runtime
            _3,
            editor_synchronization_no,                 // Runtime
            editor_synchronization_same_name_and_type, // Runtime
            _6,
            do_not_auto_update_icon // About
        };

        u32 handle;
        std::wstring name;    // About
        u32 transparent;      // Display
        i32 effect;           // Display
        i32 blend_coefficent; // Display
        u32 antialiasing;     // Display
        Flags<u32> flags;
        std::optional<u32> icon; // About
        Chunks chunks;
    };

    // CStaticItem
    export struct StaticObject : ObjectBase {
        enum class ObstacleType : i32 {
            none,
            obstacle,
            platform,
            ladder,
            transparent
        };

        ObstacleType obstacle_type; // Runtime
        u32 collision_with_box;     // Runtime
    };

    // CQuickBackdropItem
    export struct QuickBackdropObject : StaticObject {
        enum Flag {
            vertical_gradient,
            integral_dimensions
        };

        enum class Shape : i32 {
            none,
            line,
            rectangle,
            ellipse
        };

        enum class FillType : i32 {
            none,
            solid_color,
            gradient,
            motif
        };

        i32 width;          // Size / Position
        i32 height;         // Size / Position
        Shape shape;        // Settings
        i32 border_width;   // Settings
        Color border_color; // Settings
        FillType fill_type; // Settings
        Color color;        // Settings
        Color color2;       // Settings
        Flags<u32> flags;   // Settings
        u32 image;          // Settings
    };

    // CBackdropItem
    export struct BackdropObject : StaticObject {
        u32 image; // Settings
    };

    // CDynamicItem
    export struct DynamicObject : ObjectBase {
        enum Flag {
            display_in_front,                                                  // OEFLAG_* (SDK)
            background,                                                        // OEFLAG_* (SDK)
            save_background,                                                   // Display
            create_before_frame_fade_in_transition,                            // Runtime
            has_movements,                                                     // OEFLAG_* (SDK)
            has_animations,                                                    // OEFLAG_* (SDK)
            tab_stop_focus,                                                    // OEFLAG_* (SDK)
            is_window_process,                                                 // OEFLAG_* (SDK)
            has_values,                                                        // OEFLAG_* (SDK)
            has_sprites,                                                       // OEFLAG_* (SDK)
            internal_save_background,                                          // OEFLAG_* (SDK)
            do_not_follow_the_frame,                                           // Runtime
            display_as_background,                                             // Display
            do_not_destroy_object_if_too_far_from_frame,                       // Runtime
            inactivate_if_too_far_from_window_no,                              // Runtime
            inactivate_if_too_far_from_window_yes,                             // Runtime
            has_text,                                                          // OEFLAG_* (SDK)
            do_not_create_at_start,                                            // Runtime
            fake_sprite,                                                       // OEFLAG_* (SDK)
            fake_collisions,                                                   // OEFLAG_* (SDK)
            do_not_reset_current_frame_duration_when_the_animation_is_modified // Runtime
        };

        enum Flag2 {
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
        Flags<u32> flags2;
        Color background_color;             // Display
        std::array<u16, 8> qualifiers;      // Events
        Values values;                      // Values
        Values strings;                     // Values
        Movements movements;                // Movement
        Behaviors behaviors;                // Events
        std::optional<Transition> fade_in;  // Display
        std::optional<Transition> fade_out; // Display
    };

    // CAnimatedItem
    export struct AnimatedObject : DynamicObject {
        std::optional<Animations> animations;
    };

    // CActiveItem
    export struct ActiveObject : AnimatedObject {};

    // CExtendItem
    export struct ExtensionObject : AnimatedObject {
        i32 type;
        std::wstring name;
        std::wstring filename;
        i32 magic_num;
        std::wstring subtype;
        ExtensionHeader header;
        std::vector<unsigned char> data;
    };

    // CTextItem
    export struct TextObject : DynamicObject {
        i32 width;  // Size / Position
        i32 height; // Size / Position
    };

    // CStringItem
    export struct StringObject : TextObject {
        Content content; // Settings & Text
    };

    // CQuestionItem
    export struct QuestionObject : TextObject {
        Content question;
        Content answer;
    };

    // CRTFItem
    export struct RichTextObject : TextObject {
        enum Flag {
            _0,
            auto_vertical_scrollbar
        };

        Flags<u32> flags;       // Settings
        Color background_color; // Settings
        std::wstring data;
    };

    // CPlayerCounterItem
    export struct PlayerCounterObject : DynamicObject {
        u32 player;              // Settings
        std::vector<u32> images; // Settings
        u32 use_text;            // Settings
        Color color;             // Text
        u32 font;                // Text
        i32 width;               // Size / Position
        i32 height;              // Size / Position
    };

    // CScoreItem
    export struct ScoreObject : PlayerCounterObject {};

    // CLivesItem
    export struct LivesObject : PlayerCounterObject {};

    // CCounterItem
    export struct CounterObject : DynamicObject {
        enum class DisplayType : i32 {
            hidden,
            numbers,
            vertical_bar,
            horizontal_bar,
            animation,
            text
        };

        enum class FillType : i32 {
            none,
            solid_color,
            gradient,
            motif
        };

        i32 init;                 // Settings
        i32 min;                  // Settings
        i32 max;                  // Settings
        DisplayType display_type; // Settings
        FillType fill_type;       // Settings
        Color color;              // Settings
        Color color2;             // Settings
        u32 vertical_gradient;    // Settings
        u32 reverse;              // Settings
        i32 width;                // Size / Position
        i32 height;               // Size / Position
        std::vector<u32> images;  // Settings
        u32 font;                 // Text
    };

    // CCCAItem
    export struct SubapplicationObject : DynamicObject {
        enum Flag {
            share_global_values_strings,                 // Settings
            share_player_lives,                          // Settings
            share_player_scores,                         // Settings
            share_window_attributes,                     // CCAF_* (SDK)
            stretch_frame_to_object_size,                // Settings
            popup_window,                                // Settings
            caption,                                     // Settings
            tool_caption,                                // Settings
            border,                                      // Settings
            resizable,                                   // Settings
            system_menu,                                 // Settings
            disable_close,                               // Settings
            modal,                                       // Settings
            dialog_frame,                                // Settings
            source_frame_from_this_application,          // Settings
            hidden_on_close,                             // Settings
            customizable_size,                           // Settings
            internal_about_box,                          // CCAF_* (SDK)
            clip_siblings,                               // Settings
            share_player_controls,                       // Settings
            mdi_child_window,                            // Settings
            docked,                                      // Settings
            docked_top,                                  // Settings
            docked_right,                                // Settings
            reopen,                                      // CCAF_* (SDK)
            run_even_if_not_active,                      // Settings
            display_as_sprite,                           // Settings
            windows_ignore_parents_resize_display_option // Settings
        };

        std::wstring path; // Settings
        i32 width;         // Size / Position
        i32 height;        // Size / Position
        Flags<u32> flags;
        u32 frame; // Settings
        u32 icon;  // Settings
    };

    export struct Object : std::variant<
                               QuickBackdropObject,
                               BackdropObject,
                               ActiveObject,
                               StringObject,
                               QuestionObject,
                               ScoreObject,
                               LivesObject,
                               CounterObject,
                               RichTextObject,
                               SubapplicationObject,
                               ExtensionObject
                           > {};

    export struct Objects : std::vector<Object> {};

}
