/**********************************
* Chicago Marathon for Pebble
*
* NOTE: NOT ASSOCIATED WITH THE CHICAGO MARATHON OR BANK OF AMERICA IN ANY
*       WAY. EVERYTHING IS COPYRIGHT OF ITS RESPECTIVE OWNER.
*
* Created by: Brian Jett (http://logicalpixels.com)
*
* runners.c
*/

#include "common.h"

#define MAX_SPLITS 10

static Split splits[MAX_SPLITS];

static int num_splits;
static char splitsError[128];

static void splits_clean_list();
static uint16_t splits_menu_get_num_sections_callback(struct MenuLayer *menu_layer, void *callback_context);
static uint16_t splits_menu_get_num_rows_callback(struct MenuLayer *menu_layer, uint16_t section_index, void *callback_context);
static int16_t splits_menu_get_header_height_callback(struct MenuLayer *menu_layer, uint16_t section_index, void *callback_context);
static int16_t splits_menu_get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context);
static void splits_menu_draw_header_callback(GContext *ctx, const Layer *cell_layer, uint16_t section_index, void *callback_context);
static void splits_menu_draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *callback_context);
static void splits_menu_select_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context);
static void splits_menu_select_long_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context);

static Window *splits_window;
static MenuLayer *splits_menu_layer;

char *runner_name;

void splits_init(void) {
	splits_window = window_create();

	splits_menu_layer = menu_layer_create_fullscreen(splits_window);
	menu_layer_set_callbacks(splits_menu_layer, NULL, (MenuLayerCallbacks) {
		.get_num_sections = splits_menu_get_num_sections_callback,
		.get_num_rows = splits_menu_get_num_rows_callback,
		.get_header_height = splits_menu_get_header_height_callback,
		.get_cell_height = splits_menu_get_cell_height_callback,
		.draw_header = splits_menu_draw_header_callback,
		.draw_row = splits_menu_draw_row_callback,
		.select_click = splits_menu_select_callback,
		.select_long_click = splits_menu_select_long_callback,
	});

	menu_layer_set_click_config_onto_window(splits_menu_layer, splits_window);
	menu_layer_add_to_window(splits_menu_layer, splits_window);
}

void splits_show() {
	splits_clean_list();
	window_stack_push(splits_window, true);
}

void splits_destroy(void) {
	layer_remove_from_parent(menu_layer_get_layer(splits_menu_layer));
	menu_layer_destroy_safe(splits_menu_layer);
	window_destroy_safe(splits_window);
}

void get_splits(int index, char *name) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Getting splits...");
	Tuplet get_splits_tuple = TupletInteger(GET_SPLITS, index);
	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);

	if (iter == NULL) {
		return;
	}

	dict_write_tuplet(iter, &get_splits_tuple);
	dict_write_end(iter);
	app_message_outbox_send();

	runner_name = name;
}

static void splits_clean_list() {
	memset(splits, 0x0, sizeof(splits));
	num_splits = 0;
	splitsError[0] = '\0';
	menu_layer_set_selected_index(splits_menu_layer, (MenuIndex) { .row = 0, .section = 0 }, MenuRowAlignBottom, false);
	menu_layer_reload_data_and_mark_dirty(splits_menu_layer);
}

bool splits_is_on_top() {
	return splits_window == window_stack_get_top_window();
}

void splits_in_received_handler(DictionaryIterator *iter) {
	Tuple *index_tuple = dict_find(iter, SPLIT_INDEX);
	Tuple *location_tuple = dict_find(iter, SPLIT_LOCATION);
	Tuple *time_tuple = dict_find(iter, SPLIT_TIME);

	if (index_tuple) {
		Split split;
		split.index = index_tuple->value->int16;
		strncpy(split.location, location_tuple->value->cstring, sizeof(split.location));
		strncpy(split.time, time_tuple->value->cstring, sizeof(split.time));
		splits[split.index] = split;
		num_splits++;
		menu_layer_reload_data_and_mark_dirty(splits_menu_layer);
	}
}

static uint16_t splits_menu_get_num_sections_callback(struct MenuLayer *menu_layer, void *callback_context) {
    return 1;
}

static uint16_t splits_menu_get_num_rows_callback(struct MenuLayer *menu_layer, uint16_t section_index, void *callback_context) {
    return (num_splits) ? num_splits : 1;
}

static int16_t splits_menu_get_header_height_callback(struct MenuLayer *menu_layer, uint16_t section_index, void *callback_context) {
    return MENU_CELL_BASIC_HEADER_HEIGHT;
}

static int16_t splits_menu_get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
    return MENU_CELL_BASIC_CELL_HEIGHT;
}

static void splits_menu_draw_header_callback(GContext *ctx, const Layer *cell_layer, uint16_t section_index, void *callback_context) {
    menu_cell_basic_header_draw(ctx, cell_layer, runner_name);
}

static void splits_menu_draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *callback_context) {
    if (strlen(splitsError) != 0) {
        menu_cell_basic_draw(ctx, cell_layer, "Error!", splitsError, NULL);
    } else if (num_splits == 0) {
        menu_cell_basic_draw(ctx, cell_layer, "Loading...", NULL, NULL);
    } else {
		menu_cell_basic_draw(ctx, cell_layer, splits[cell_index->row].location, splits[cell_index->row].time, NULL);
	}
}

static void splits_menu_select_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
    //
}

static void splits_menu_select_long_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
    //
}
