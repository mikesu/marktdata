
//列表数据项
typedef struct {
	//编号
	char* code;
  //名字
  char*  name;
  //当前价格或点数
  char* value;
	//涨跌价格或点数
	char* point;
	//涨跌率(%)
	char* rate;
	//成交量
	char* volume;
	//成交额
	char* turnover;
} Item;

typedef void (*ListWindowSelectClick)(char* code);
typedef void (*ListWindowHandler)(void);

typedef struct {
	ListWindowSelectClick select_click;
	ListWindowHandler select_long_click;
	ListWindowHandler tap_handler;
} ListWindowCallbacks;

void set_list_window_callbacks(ListWindowCallbacks callbacks);
void reload_list_window(Item *items,uint16_t items_len);
void show_list_window(Item *items,uint16_t items_len);
void hide_list_window(void);
