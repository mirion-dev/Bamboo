module;

#include <cassert>

export module bamboo.general;

import std;
import bamboo.types;

namespace bamboo {

    export struct Color {
        using is_dense_layout = void;

        u8 r, g, b, a;
    };

    export template <std::unsigned_integral T>
    struct Flags {
        using is_dense_layout = void;

        T value;

        class Ref {
            friend Flags;

            T* _ptr;
            T _mask;

            Ref(T* ptr, T mask) noexcept :
                _ptr{ ptr },
                _mask{ mask } {}

        public:
            operator bool() const noexcept {
                return *_ptr & _mask;
            }

            const Ref& operator=(bool value) const noexcept {
                if (value) {
                    *_ptr |= _mask;
                }
                else {
                    *_ptr &= ~_mask;
                }
                return *this;
            }
        };

        Ref operator[](usize index) noexcept {
            assert(index < std::numeric_limits<T>::digits);
            return { &value, static_cast<T>(T{ 1 } << index) };
        }

        bool operator[](usize index) const noexcept {
            assert(index < std::numeric_limits<T>::digits);
            return value & static_cast<T>(T{ 1 } << index);
        }
    };

    export struct Header {
        i16 runtime_version;
        i16 runtime_subversion;
        i32 product_version;
        i32 product_build;
        i32 language;
        std::wstring app_name;
        std::wstring editor_filename;
    };

    export struct Font {
        u32 handle;
        u32 checksum;
        i32 references;
        i32 size;
        i32 height;
        i32 width;
        i32 escapement;
        i32 orientation;
        i32 weight;
        u8 italic;
        u8 underline;
        u8 strike_out;
        i8 charset;
        i8 out_precision;
        i8 clip_precision;
        i8 quality;
        i8 pitch_family;
        std::wstring name;
    };

    export struct FontBank : std::vector<Font> {};

    export struct Sound {
        enum Flag {
            check,
            load_on_call   = 4,
            play_from_disk = 5,
            has_name       = 8,
            name_crop      = 14
        };

        u32 handle;
        u32 checksum;
        i32 references;
        i32 size;
        Flags<u32> flags;
        i32 frequency;
        std::wstring name;
        std::vector<char> data;
    };

    export struct SoundBank : std::vector<Sound> {};

    export struct Music {
        u32 handle;
        u32 checksum;
        i32 references;
        i32 size;
        Flags<u32> flags;
        i32 frequency;
        std::wstring name;
        std::vector<char> data;
    };

    export struct MusicBank : std::vector<Music> {};

    export struct Image {
        enum Flag {
            rle,
            rlew,
            rlet,
            lzx,
            alpha,
            ace,
            mac,
            rgba
        };

        u32 handle;
        u32 checksum;
        i32 references;
        i32 size;
        i16 width;
        i16 height;
        i8 graphic_mode;
        Flags<u8> flags;
        i16 hotspot_x;
        i16 hotspot_y;
        i16 action_x;
        i16 action_y;
        Color transparent_color;
        std::vector<char> data;
    };

    export struct ImageBank : std::vector<Image> {
        i32 graphic_mode;
        i16 palette_version;
        std::vector<u32> palette;
    };

    export struct BinaryFiles : std::vector<std::wstring> {};

    export struct Control {
        i32 type;
        std::vector<i32> data;
    };

    export struct Controls : std::vector<Control> {};

    export struct Qualifier {
        std::wstring name;
        u32 handle;
    };

    export struct Qualifiers : std::vector<Qualifier> {};

    export struct Extension {
        u32 handle;
        std::wstring filename;
        std::wstring name;
        i32 magic_num;
        std::wstring subtype;
        i32 is_unicode;
    };

    export struct Extensions : std::vector<Extension> {};

    export struct MenuItem;

    export struct MenuItems : std::vector<MenuItem> {};

    struct MenuItem {
        enum Flag {
            disabled,
            checked = 3,
            parent,
            footer = 7
        };

        Flags<u16> flags;
        i16 id;
        std::wstring name;
        MenuItems items;
    };

    export struct MenuAccel {
        i16 shift;
        i16 key;
        i32 id;
    };

    export struct Menu {
        i32 size;
        i32 header_size;
        i32 item_offset;
        i32 item_size;
        i32 accel_offset;
        i32 accel_size;
        MenuItems items;
        std::vector<MenuAccel> accels;
        i32 window_menu;
        std::vector<i64> images;
    };

    export struct Value {
        enum Type {
            integer,
            decimal,
            string
        };

        std::wstring name;
        std::variant<i32, f64, std::wstring> value;
    };

    export struct Values : std::vector<Value> {};

    export struct GlobalEvents {
        i32 size;
    };

    export struct Setting {
        enum DisplayFlag {
            maximized_on_boot,
            resize_display,
            change_resolution_mode,
            allow_fullscreen_switch,
            heading,
            heading_when_maximized,
            menu_bar,
            menu_displayed,
            no_minimize_box,
            no_maximize_box,
            no_thick_frame,
            dont_center_frame,
            disable_close_button,
            hidden_at_start,
            keep_screen_ratio = 15,
            anti_aliasing,
            right_to_left_reading = 18,
            right_to_left_layout,
            fit_inside = 21
        };

        enum GraphicFlag {
            multi_samples,
            machine_independent_speed,
            play_sounds_over_frames,
            dont_mute_on_lost_focus,
            dont_stop_screen_saver,
            enable_visual_themes = 7,
            v_sync,
            run_when_minimized,
            run_while_resizing,
            debugger_shortcuts,
            dont_show_debugger,
            dont_share_sub_app_data,
            direct3d9,
            direct3d8,
            dont_ignore_destroy_far = 21,
            disable_ime,
            reduce_cpu_usage,
            enable_profiling = 26,
            dont_start_profiling,
            direct3d11,
            premultiplied_alpha,
            dont_optimize_events,
            record_slowest_loops
        };

        std::wstring app_name;
        std::wstring author;
        std::wstring description;
        std::wstring copyright;
        std::wstring company;
        std::wstring version;
        i32 app_width;
        i32 app_height;
        Color border_color;
        Flags<u32> display_flags;
        Flags<u32> graphic_flags;
        std::wstring help_file;
        i32 score;
        i32 lives;
        i32 frame_rate;
        i32 build_type;
        std::wstring build_filename;
        std::wstring about;
        BinaryFiles binary_files;
        Controls controls;
        Menu menu;
        Values global_numbers;
        Values global_strings;
        GlobalEvents global_events;
        i32 graphic_mode;
        std::vector<u32> icons;
        Qualifiers qualifiers;
        Extensions extensions;
    };

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

    export struct Transition {
        std::wstring name;
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
            correct = 8,
            relief
        };

        std::wstring value;
        Flags<u32> flags;
    };

    export struct Paragraphs : std::vector<Paragraph> {};

    export struct Content {
        i32 font;
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
            dont_reset_frame_duration = 20
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
        i32 font;
        i32 width;
        i32 height;
    };

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
            auto_scrollbar = 1
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

    export struct Object {
        enum Type {
            quick_backdrop,
            backdrop,
            active,
            string,
            question_answer,
            score,
            lives,
            counter,
            formatted_text,
            subapplication,
            extension
        };

        enum Flag {
            load_on_call,
            global_object  = 2,
            no_editor_sync = 4,
            name_type_editor_sync,
            no_auto_update = 7
        };

        Type type;
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
        std::variant<
            QuickBackdrop,
            Backdrop,
            ActiveObject,
            StringObject,
            QuestionAnswerObject,
            ScoreLivesObject,
            CounterObject,
            FormattedTextObject,
            SubapplicationObject,
            ExtensionObject
        > data;
    };

    export struct Objects : std::vector<Object> {};

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

    export struct Parameter {
        i16 size;
        std::vector<char> data;
    };

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
        std::vector<Parameter> params;
    };

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
        std::vector<Parameter> params;
    };

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
        std::vector<Condition> conditions;
        std::vector<Action> actions;
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
        std::variant<
            EventObjectRef,
            EventObjectIcon,
            EventObjectQualifier
        > data;
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

    export struct EventBlock {
        std::variant<
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
        > data;
    };

    export struct EventBlocks : std::vector<EventBlock> {
        i32 data_size;
    };

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

    export struct File {
        Header header;
        FontBank font_bank;
        SoundBank sound_bank;
        MusicBank music_bank;
        ImageBank icon_bank;
        ImageBank image_bank;
        Setting setting;
        Frames frames;
    };

}
