#include <pebble.h>
#include "list_window.h"

#define MENU_SEPARATOR_HEIGHT 1 
#define MENU_CELL_HEIGHT 44

static ListWindowCallbacks s_callbacks;
static Item *s_items;
static uint16_t s_items_len = 0;

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static MenuLayer *s_menulayer;
static StatusBarLayer *s_statusbar_layer;

static void initialise_ui(void) {
  s_window = window_create();
  #ifndef PBL_SDK_3
    window_set_fullscreen(s_window, true);
  #endif
  
  // s_menulayer
  s_menulayer = menu_layer_create(GRect(0, 16, 144, 152));
  menu_layer_set_click_config_onto_window(s_menulayer, s_window);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_menulayer);
	
	s_statusbar_layer = status_bar_layer_create();
	layer_add_child(window_get_root_layer(s_window), status_bar_layer_get_layer(s_statusbar_layer));
}

static void destroy_ui(void) {
  window_destroy(s_window);
  menu_layer_destroy(s_menulayer);
}
// END AUTO-GENERATED UI CODE

//多少行
static uint16_t menu_get_num_rows(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return s_items_len;
}
//分割线的高度
static int16_t menu_get_separator_height(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  return MENU_SEPARATOR_HEIGHT;
}
//画分割线
static void menu_draw_separator(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
	GRect cell_bounds = layer_get_bounds(cell_layer);
  GPoint p0 = { cell_bounds.origin.x, cell_bounds.origin.y };
  GPoint p1 = { cell_bounds.origin.x+cell_bounds.size.w, cell_bounds.origin.y };
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_draw_line(ctx,p0,p1);
}
//选项高度
static int16_t menu_get_cell_height(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
	  return MENU_CELL_HEIGHT;
}
//选项绘制
static void menu_draw_row(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
	GRect cell_bounds = layer_get_bounds(cell_layer);
	GFont font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  if (menu_layer_is_index_selected(s_menulayer,cell_index))
  {
    graphics_context_set_text_color(ctx, GColorWhite);
  }

	//graphics_context_set_text_color(ctx, GColorBlue); 
  GRect name_bounds = { .origin = { cell_bounds.origin.x, cell_bounds.origin.y }, .size = { cell_bounds.size.w/2, cell_bounds.size.h/2 } };
  graphics_draw_text(ctx, s_items[cell_index->row].name, font, name_bounds, GTextOverflowModeWordWrap,GTextAlignmentCenter, NULL);
          
  //graphics_context_set_text_color(ctx, GColorRed);
  GRect value_bounds = { .origin = { cell_bounds.origin.x+cell_bounds.size.w/2, cell_bounds.origin.y }, .size = { cell_bounds.size.w/2, cell_bounds.size.h/2 } };
  graphics_draw_text(ctx, s_items[cell_index->row].value, font, value_bounds, GTextOverflowModeWordWrap,GTextAlignmentCenter, NULL);
	
	//graphics_context_set_text_color(ctx, GColorRed); 
  GRect rate_bounds = { .origin = { cell_bounds.origin.x, cell_bounds.origin.y+cell_bounds.size.h/2 }, .size = { cell_bounds.size.w/2, cell_bounds.size.h/2 } };
  graphics_draw_text(ctx, s_items[cell_index->row].rate, font, rate_bounds, GTextOverflowModeWordWrap,GTextAlignmentCenter, NULL);
          
  //graphics_context_set_text_color(ctx, GColorRed);
  GRect point_bounds = { .origin = { cell_bounds.origin.x+cell_bounds.size.w/2, cell_bounds.origin.y+cell_bounds.size.h/2 }, .size = { cell_bounds.size.w/2, cell_bounds.size.h/2 } };
  graphics_draw_text(ctx, s_items[cell_index->row].point, font, point_bounds, GTextOverflowModeWordWrap,GTextAlignmentCenter, NULL);


}
//选择回调
static void menu_select_click(MenuLayer *menu_layer, MenuIndex *cell_index, void *data){
	APP_LOG(APP_LOG_LEVEL_DEBUG,"menu_select_click");
	if(s_callbacks.select_click!=NULL){
		s_callbacks.select_click(s_items[cell_index->row].code);
	}
}
//长按选择回调
static void menu_select_long_click(MenuLayer *menu_layer, MenuIndex *cell_index, void *data){
		APP_LOG(APP_LOG_LEVEL_DEBUG,"menu_select_long_click");
	if(s_callbacks.select_long_click!=NULL){
		s_callbacks.select_long_click();
	}
}

static void handle_window_load(Window* window){
  menu_layer_set_callbacks(s_menulayer, NULL, (MenuLayerCallbacks){
    .get_num_rows = menu_get_num_rows,
    .get_separator_height = menu_get_separator_height,
    .draw_separator = menu_draw_separator,
    .get_cell_height = menu_get_cell_height,
    .draw_row = menu_draw_row,
    .select_click = menu_select_click,
		.select_long_click = menu_select_long_click,
  });
}

static void handle_window_unload(Window* window) {
  destroy_ui();
}

void set_list_window_callbacks(ListWindowCallbacks callbacks){
	s_callbacks = callbacks;
}

void reload_list_window(Item *items,uint16_t items_len){
	s_items = items;
	s_items_len = items_len;
}

void show_list_window(Item *items,uint16_t items_len) {
	s_items = items;
	s_items_len = items_len;
  initialise_ui();
  window_set_window_handlers(s_window, (WindowHandlers) {
		.load = handle_window_load,
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);
}

void hide_list_window(void) {
  window_stack_remove(s_window, true);
}
