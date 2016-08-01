#include <pebble.h>
#include "modules/data_source.h"
#include "modules/settings.h"
#include "windows/wellcome_window.h"
#include "windows/list_window.h"

static DataList s_data_list;
static DataDetail s_data_detail;

static void receive_list(DataList data_list){
  s_data_list = data_list;
  list_window_show(data_list);
}

static void receive_detail(DataDetail data_detail){

}

static void memu_select_click(char* code){
  APP_LOG(APP_LOG_LEVEL_DEBUG,"menu_select_click,code:%s",code);
}

static void init() {
  show_wellcome_window();
  
  data_source_init((DataSourceCallbacks){
    .receive_list = receive_list,
    .receive_detail = receive_detail,
  });
  list_window_set_callbacks((ListWindowCallbacks){
    .select_click = memu_select_click,
  });

  char* code = "sh000001";
  char** codes = &code;

  data_source_get_list(codes,1);

  
}

static void deinit() {
  hide_wellcome_window();
  data_source_deinit();
  data_source_free_list(s_data_list);
  data_source_free_detail(s_data_detail);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}