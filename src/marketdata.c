#include <pebble.h>
#include "windows/wellcome_window.h"
#include "modules/data_source.h"
#include "modules/settings.h"
#include "windows/list_window.h"

static void http_receive_string(char* str,uint32_t length){
  printf("String : %s", str);
  Item items[3]={
    {.code="sh000001",.name="SH",.value="3060.69",.point="+11.31",.rate="+0.37%"},
    {.code="sz399001",.name="SZ",.value="10853.56",.point="+95.71",.rate="+0.89%"},
    {.code="sz399006",.name="CY",.value="2285.37",.point="+38.01",.rate="+1.69%"}
  };
  
  show_list_window(items,3);
}

static void memu_select_click(char* code){
  APP_LOG(APP_LOG_LEVEL_DEBUG,"menu_select_click,code:%s",code);
}

static void init() {
  //显示欢迎界面
  show_wellcome_window();
  //加载配置数据
  
  //调用sina API加载列表数据
  
  //显示列表
  set_list_window_callbacks((ListWindowCallbacks){
    .select_click = memu_select_click,
  });
  http_init((HttpCallbacks){.receive_string=http_receive_string});
 // http_get_string("dd");
  
}

static void deinit() {
  hide_wellcome_window();
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}