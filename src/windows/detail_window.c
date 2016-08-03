#include <pebble.h>
#include "detail_window.h"

static Window *s_window = NULL;
static BitmapLayer *s_bitmap_layer = NULL;

static GBitmap *s_bitmap = NULL;

static void handle_window_load(Window* window){
  APP_LOG(APP_LOG_LEVEL_DEBUG,"detail_window load");
  Layer *root_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(root_layer);

  s_bitmap_layer = bitmap_layer_create(bounds);
  bitmap_layer_set_compositing_mode(s_bitmap_layer, GCompOpSet);
  layer_add_child(root_layer, bitmap_layer_get_layer(s_bitmap_layer));
}

static void handle_window_unload(Window* window) {
  APP_LOG(APP_LOG_LEVEL_DEBUG,"detail_window unload");
  bitmap_layer_destroy(s_bitmap_layer);

  if(s_bitmap) {
    gbitmap_destroy(s_bitmap);
    s_bitmap = NULL;
  }
  window_destroy(window);
}

void detail_window_show(DataDetail* data_detail) {
  s_window = window_create();
  #ifndef PBL_SDK_3
    window_set_fullscreen(s_window, true);
  #endif
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = handle_window_load,
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);

  if(s_bitmap) {
    gbitmap_destroy(s_bitmap);
    s_bitmap = NULL;
  }

  // Create new GBitmap from downloaded PNG data
	
  s_bitmap = gbitmap_create_from_png_data(data_detail->img_data, data_detail->img_size);
	
  if(s_bitmap) {
    bitmap_layer_set_bitmap(s_bitmap_layer, s_bitmap);
  } else {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Error creating GBitmap from PNG data!");
  }
}

void detail_window_hide(void) {
  window_stack_remove(s_window, true);
}
