#include "bn_log.h"
#include "bn_core.h"
#include "bn_math.h"
#include "bn_string.h"
#include "bn_keypad.h"
#include "bn_bg_palettes.h"
#include "bn_sprite_text_generator.h"
#include "bn_display.h"

#include "bn_regular_bg_ptr.h"
#include "bn_regular_bg_builder.h"
#include "bn_regular_bg_actions.h"
#include "bn_regular_bg_attributes.h"
#include "bn_bg_palette_actions.h"
#include "bn_bg_palettes_actions.h"

#include "common_info.h"
#include "common_variable_8x16_sprite_font.h"

#include "bn_regular_bg_items_road.h"
#include "bn_bg_palette_items_road_palette.h"

namespace {
    void main_scene(bn::sprite_text_generator& text_generator) {
        int speed = 140;

        bn::vector<bn::sprite_ptr, 2> text_sprites;
        bn::fixed max_cpu_usage;
        int counter = 1;

        bn::regular_bg_ptr road_bg = bn::regular_bg_items::road.create_bg(0, 0);
        const bn::bg_palette_item& road_original_palette = bn::regular_bg_items::road.palette_item();
        const bn::bg_palette_item& road_alt_palette = bn::bg_palette_items::road_palette;
        bn::bg_palette_ptr road_palette = road_bg.palette();

        BN_LOG(bn::display::height());
        BN_LOG(-bn::display::height() / 2);
        BN_LOG(bn::display::width());
        BN_LOG(-bn::display::width() / 2);

        while(true) {
            max_cpu_usage = bn::max(max_cpu_usage, bn::core::last_cpu_usage());
            --counter;

            if(! counter) {
                bn::string<32> text;
                bn::ostringstream text_stream(text);
                text_stream.append((max_cpu_usage * 100).right_shift_integer());
                text_stream.append("%");
                text_sprites.clear();
                text_generator.generate(-112, -72, text, text_sprites);

                max_cpu_usage = 0;
                counter = 60;
            }

            if( bn::keypad::a_held() ) {
              speed = speed + 1;
            } else if( bn::keypad::b_held() && speed >= 2 ) {
              speed = speed - 2;
            } else if( speed >= 2 && counter % 10 == 0 ) {
              speed = speed - 2;
            } else if ( speed <= 1 ) {
              speed = 0;
            }

            int bg_move = 3;
            if( speed >= 100 ) {
              bg_move = 4;
            } else if ( speed >= 90 ) {
              bg_move = 5;
            } else if ( speed >= 80 ) {
              bg_move = 6;
            } else if ( speed >= 60 ) {
              bg_move = 10;
            } else if ( speed >= 30 ) {
              bg_move = 20;
            } else if ( speed  >  0 ) {
              bg_move = 30;
            }

            if( speed != 0 && counter % bg_move == 0 ) {
              if(road_palette.colors() == road_original_palette.colors_ref()) {
                  road_palette.set_colors(road_alt_palette);
              }
              else {
                  road_palette.set_colors(road_original_palette);
              }
            }

            bn::string<32> text;
            bn::ostringstream text_stream(text);
            text_stream.append(speed);
            text_stream.append(" km/h");
            text_sprites.clear();
            text_generator.generate(64, -72, text, text_sprites);

            // testing basic logging
            if( bn::keypad::any_pressed() ) {
              if( bn::keypad::a_pressed() ) {
                BN_LOG("Key pressed: a");
              }
            }

            bn::core::update();
        }
    }
}

int main() {
    bn::core::init();

    bn::sprite_text_generator text_generator(common::variable_8x16_sprite_font);
    bn::bg_palettes::set_transparent_color(bn::color(16, 16, 16));

    main_scene(text_generator);
}
