#pragma once

typedef void (*HttpReceiveStringCallback)(char* str,uint32_t length);

typedef void (*HttpReceiveRawCallback)(uint8_t* data,uint32_t length);

//列表数据项
typedef struct {
	char* code; 	//编号
  	char* name;		//名字
  	char* value;	//当前价格或点数
	char* point;	//涨跌价格或点数
	char* rate;		//涨跌率(%)
	char* volume;	//成交量
	char* turnover;	//成交额
} Item;

typedef struct {
	HttpReceiveStringCallback receive_string;
	HttpReceiveRawCallback receive_raw;
} HttpCallbacks;

void http_get_string(char* url);

void http_get_raw(char* url);

void http_init(HttpCallbacks callbacks);