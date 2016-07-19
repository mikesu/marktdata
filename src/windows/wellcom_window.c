#include <pebble.h>
#include "wellcome_window.h"

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static GFont s_res_bitham_30_black;
static GFont s_res_roboto_condensed_21;
static TextLayer *s_textlayer;
static TextLayer *s_textlayer_1;

static void initialise_ui(void) {
  s_window = window_create();
  #ifndef PBL_SDK_3
    window_set_fullscreen(s_window, true);
  #endif
  
  s_res_bitham_30_black = fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK);
  s_res_roboto_condensed_21 = fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21);
  // s_textlayer
  s_textlayer = text_layer_create(GRect(18, 9, 107, 70));
  text_layer_set_text(s_textlayer, "Market Data");
  text_layer_set_text_alignment(s_textlayer, GTextAlignmentCenter);
  text_layer_set_font(s_textlayer, s_res_bitham_30_black);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_textlayer);
  
  // s_textlayer_1
  s_textlayer_1 = text_layer_create(GRect(22, 113, 100, 27));
  text_layer_set_text(s_textlayer_1, "loading");
  text_layer_set_text_alignment(s_textlayer_1, GTextAlignmentCenter);
  text_layer_set_font(s_textlayer_1, s_res_roboto_condensed_21);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_textlayer_1);
}

static void destroy_ui(void) {
  window_destroy(s_window);
  text_layer_destroy(s_textlayer);
  text_layer_destroy(s_textlayer_1);
}
// END AUTO-GENERATED UI CODE

static void handle_window_unload(Window* window) {
  destroy_ui();
}

void show_wellcome_window(void) {
  initialise_ui();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);
}

void hide_wellcome_window(void) {
  window_stack_remove(s_window, true);
}
