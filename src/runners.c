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

#define MAX_RUNNERS 10

static Runner runners[MAX_RUNNERS];

static int num_runners;
static char runnersError[128];
static char buffer[128];

void runners_clean_list();
static uint16_t runners_menu_get_num_sections_callback(struct MenuLayer *menu_layer, void *callback_context);
static uint16_t runners_menu_get_num_rows_callback(struct MenuLayer *menu_layer, uint16_t section_index, void *callback_context);
static int16_t runners_menu_get_header_height_callback(struct MenuLayer *menu_layer, uint16_t section_index, void *callback_context);
static int16_t runners_menu_get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context);
static void runners_menu_draw_header_callback(GContext *ctx, const Layer *cell_layer, uint16_t section_index, void *callback_context);
static void runners_menu_draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *callback_context);
static void runners_menu_select_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context);
static void runners_menu_select_long_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context);

static Window *runners_window;
static MenuLayer *runners_menu_layer;

void runners_init(void) {
	runners_window = window_create();

	runners_menu_layer = menu_layer_create_fullscreen(runners_window);
	menu_layer_set_callbacks(runners_menu_layer, NULL, (MenuLayerCallbacks) {
		.get_num_sections = runners_menu_get_num_sections_callback,
		.get_num_rows = runners_menu_get_num_rows_callback,
		.get_header_height = runners_menu_get_header_height_callback,
		.get_cell_height = runners_menu_get_cell_height_callback,
		.draw_header = runners_menu_draw_header_callback,
		.draw_row = runners_menu_draw_row_callback,
		.select_click = runners_menu_select_callback,
		.select_long_click = runners_menu_select_long_callback,
	});

	menu_layer_set_click_config_onto_window(runners_menu_layer, runners_window);
	menu_layer_add_to_window(runners_menu_layer, runners_window);
}

void runners_show() {
	runners_clean_list();
	window_stack_push(runners_window, true);
}

void runners_destroy(void) {
	layer_remove_from_parent(menu_layer_get_layer(runners_menu_layer));
	menu_layer_destroy_safe(runners_menu_layer);
	window_destroy_safe(runners_window);
}

void runners_clean_list() {
	memset(runners, 0x0, sizeof(runners));
	num_runners = 0;
	runnersError[0] = '\0';
	menu_layer_set_selected_index(runners_menu_layer, (MenuIndex) { .row = 0, .section = 0 }, MenuRowAlignBottom, false);
	menu_layer_reload_data_and_mark_dirty(runners_menu_layer);
}

bool runners_is_on_top() {
	return runners_window == window_stack_get_top_window();
}

void runners_in_received_handler(DictionaryIterator *iter) {
	Tuple *index_tuple = dict_find(iter, RUNNER_INDEX);
	Tuple *name_tuple = dict_find(iter, RUNNER_NAME);
	Tuple *subtitle_tuple = dict_find(iter, SUBTITLE);

	if (index_tuple) {
		Runner runner;
		runner.index = index_tuple->value->int16;
		strncpy(runner.name, name_tuple->value->cstring, sizeof(runner.name));
		strncpy(runner.subtitle, subtitle_tuple->value->cstring, sizeof(runner.subtitle));
		runners[runner.index] = runner;
		num_runners++;
		menu_layer_reload_data_and_mark_dirty(runners_menu_layer);
	}
}

static uint16_t runners_menu_get_num_sections_callback(struct MenuLayer *menu_layer, void *callback_context) {
    return 1;
}

static uint16_t runners_menu_get_num_rows_callback(struct MenuLayer *menu_layer, uint16_t section_index, void *callback_context) {
    return (num_runners) ? num_runners : 1;
}

static int16_t runners_menu_get_header_height_callback(struct MenuLayer *menu_layer, uint16_t section_index, void *callback_context) {
    return MENU_CELL_BASIC_HEADER_HEIGHT;
}

static int16_t runners_menu_get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
    return MENU_CELL_BASIC_CELL_HEIGHT;
}

static void runners_menu_draw_header_callback(GContext *ctx, const Layer *cell_layer, uint16_t section_index, void *callback_context) {
    menu_cell_basic_header_draw(ctx, cell_layer, "Tracked Runners");
}

static void runners_menu_draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *callback_context) {
    if (strlen(runnersError) != 0) {
        menu_cell_basic_draw(ctx, cell_layer, "Error!", runnersError, NULL);
    } else if (num_runners == 0) {
        menu_cell_basic_draw(ctx, cell_layer, "Loading...", NULL, NULL);
    } else {
		menu_cell_basic_draw(ctx, cell_layer, runners[cell_index->row].name, runners[cell_index->row].subtitle, NULL);
	}
}

static void runners_menu_select_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Selected index %d", runners[cell_index->row].index);
	get_splits(runners[cell_index->row].index, runners[cell_index->row].name);
}

static void runners_menu_select_long_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
    //
}
