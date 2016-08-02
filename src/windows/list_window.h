
#pragma once
#include "../modules/data_source.h"
typedef void (*ListWindowSelectClick)(char* code);
typedef void (*ListWindowHandler)(void);

typedef struct {
	ListWindowSelectClick select_click;
	ListWindowHandler select_long_click;
	ListWindowHandler tap_handler;
} ListWindowCallbacks;

void list_window_set_callbacks(ListWindowCallbacks callbacks);
void list_window_show(DataList* data_list);
void list_window_hide(void);
