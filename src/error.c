/**********************************
 * Chicago Marathon for Pebble
 *
 * NOTE: NOT ASSOCIATED WITH THE CHICAGO MARATHON OR BANK OF AMERICA IN ANY
 *       WAY. EVERYTHING IS COPYRIGHT OF ITS RESPECTIVE OWNER.
 *
 * Created by: Brian Jett (http://logicalpixels.com)
 *
 * error.c
 */

#include "common.h"

static Window *error_window;
static TextLayer *text_layer;

void error_init() {
    error_window = window_create();

    Layer *window_layer = window_get_root_layer(error_window);

    GRect bounds = layer_get_frame(window_layer);

    text_layer = text_layer_create(GRect(0, 0, bounds.size.w, bounds.size.h));
    text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
    text_layer_set_overflow_mode(text_layer, GTextOverflowModeWordWrap);
    text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
    layer_add_child(window_layer, text_layer_get_layer(text_layer));
}

void error_destroy() {
    text_layer_destroy(text_layer);
    window_destroy_safe(error_window);
}

void show_error(DictionaryIterator *iter) {
    Tuple *error_tuple = dict_find(iter, ERROR);
    text_layer_set_text(text_layer, error_tuple->value->cstring);
    window_stack_push(error_window, true);
}
