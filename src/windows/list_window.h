
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
