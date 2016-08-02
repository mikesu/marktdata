#include <pebble.h>
#include "modules/data_source.h"
#include "modules/settings.h"
#include "windows/wellcome_window.h"
#include "windows/list_window.h"
#include "windows/detail_window.h"

static void receive_list(DataList* data_list){
  list_window_show(data_list);
  hide_wellcome_window();
}

static void receive_detail(DataDetail* data_detail){
  detail_window_show(data_detail);
}

static void data_source_ready(){
  char* code = "sh000001";
  char** codes = &code;
  data_source_get_list(codes,1);
}

static void list_select_click(char* code){
  APP_LOG(APP_LOG_LEVEL_DEBUG,"list_select_click,code:%s",code);
  data_source_get_detail(code);
}

static void init() {
  show_wellcome_window();
  
  data_source_init((DataSourceCallbacks){
    .receive_list = receive_list,
    .receive_detail = receive_detail,
    .ready = data_source_ready,
  });
  list_window_set_callbacks((ListWindowCallbacks){
    .select_click = list_select_click,
  });
  
}

static void deinit() {
  data_source_deinit();
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}