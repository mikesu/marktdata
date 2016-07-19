#include <pebble.h>
#include "detail_window.h"

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static Layer *s_layer_1;

static void initialise_ui(void) {
  s_window = window_create();
  #ifndef PBL_SDK_3
    window_set_fullscreen(s_window, true);
  #endif
  
  // s_layer_1
  s_layer_1 = layer_create(GRect(20, 20, 40, 40));
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_layer_1);
}

static void destroy_ui(void) {
  window_destroy(s_window);
  layer_destroy(s_layer_1);
}
// END AUTO-GENERATED UI CODE

static void handle_window_unload(Window* window) {
  destroy_ui();
}

void show_detail_window(void) {
  initialise_ui();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);
}

void hide_detail_window(void) {
  window_stack_remove(s_window, true);
}
