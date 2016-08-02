#include <pebble.h>
#include "list_window.h"

#define MENU_SEPARATOR_HEIGHT 1 
#define MENU_CELL_HEIGHT 44

static ListWindowCallbacks s_callbacks;
static DataList* s_data_list;

static Window *s_window = NULL;
static MenuLayer *s_menulayer;
static StatusBarLayer *s_statusbar_layer;

//多少行
static uint16_t menu_get_num_rows(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return s_data_list->size;
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
  graphics_draw_text(ctx, s_data_list->data_items[cell_index->row]->name, font, name_bounds, GTextOverflowModeWordWrap,GTextAlignmentCenter, NULL);
          
  //graphics_context_set_text_color(ctx, GColorRed);
  GRect value_bounds = { .origin = { cell_bounds.origin.x+cell_bounds.size.w/2, cell_bounds.origin.y }, .size = { cell_bounds.size.w/2, cell_bounds.size.h/2 } };
  graphics_draw_text(ctx, s_data_list->data_items[cell_index->row]->value, font, value_bounds, GTextOverflowModeWordWrap,GTextAlignmentCenter, NULL);
	
	//graphics_context_set_text_color(ctx, GColorRed); 
  GRect rate_bounds = { .origin = { cell_bounds.origin.x, cell_bounds.origin.y+cell_bounds.size.h/2 }, .size = { cell_bounds.size.w/2, cell_bounds.size.h/2 } };
  graphics_draw_text(ctx, s_data_list->data_items[cell_index->row]->rate, font, rate_bounds, GTextOverflowModeWordWrap,GTextAlignmentCenter, NULL);
          
  //graphics_context_set_text_color(ctx, GColorRed);
  GRect point_bounds = { .origin = { cell_bounds.origin.x+cell_bounds.size.w/2, cell_bounds.origin.y+cell_bounds.size.h/2 }, .size = { cell_bounds.size.w/2, cell_bounds.size.h/2 } };
  graphics_draw_text(ctx, s_data_list->data_items[cell_index->row]->point, font, point_bounds, GTextOverflowModeWordWrap,GTextAlignmentCenter, NULL);


}
//选择回调
static void menu_select_click(MenuLayer *menu_layer, MenuIndex *cell_index, void *data){
	APP_LOG(APP_LOG_LEVEL_DEBUG,"menu_select_click");
	if(s_callbacks.select_click!=NULL){
		s_callbacks.select_click(s_data_list->data_items[cell_index->row]->code);
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
  s_menulayer = menu_layer_create(GRect(0, 16, 144, 152));
  menu_layer_set_click_config_onto_window(s_menulayer, window);
  layer_add_child(window_get_root_layer(window), (Layer *)s_menulayer);
  menu_layer_set_callbacks(s_menulayer, NULL, (MenuLayerCallbacks){
    .get_num_rows = menu_get_num_rows,
    .get_separator_height = menu_get_separator_height,
    .draw_separator = menu_draw_separator,
    .get_cell_height = menu_get_cell_height,
    .draw_row = menu_draw_row,
    .select_click = menu_select_click,
		.select_long_click = menu_select_long_click,
  });
  
  s_statusbar_layer = status_bar_layer_create();
  layer_add_child(window_get_root_layer(window), status_bar_layer_get_layer(s_statusbar_layer));

}

static void handle_window_unload(Window* window) {
  window_destroy(window);
  menu_layer_destroy(s_menulayer);
  status_bar_layer_destroy(s_statusbar_layer);
}

void list_window_set_callbacks(ListWindowCallbacks callbacks){
	s_callbacks = callbacks;
}


void list_window_show(DataList* data_list) {
	s_data_list = data_list;
  if(!s_window){
    s_window = window_create();
    #ifndef PBL_SDK_3
    window_set_fullscreen(s_window, true);
    #endif
    window_set_window_handlers(s_window, (WindowHandlers) {
  		.load = handle_window_load,
      .unload = handle_window_unload,
    });
    window_stack_push(s_window, true);
  }else{
    menu_layer_reload_data(s_menulayer);
  }
}

void list_window_hide(void) {
  window_stack_remove(s_window, true);
}
