#pragma once

//列表数据项
typedef struct {
	char* code; 	//编号
  	char* name;		//名字
  	char* value;	//当前价格或点数
	char* point;	//涨跌价格或点数
	char* rate;		//涨跌率(%)
	char* volume;	//成交量
	char* turnover;	//成交额
} DataItem;
//
typedef struct {
	DataItem**	data_items;
	int	 		size;
} DataList;

typedef struct {
	DataItem*	data_item;
	uint8_t*	img_data;
	int			img_size;			
} DataDetail;


typedef void (*GetListCallback)(DataList* data_list);

typedef void (*GetDetailCallback)(DataDetail* data_detail);

typedef void (*DataSourceInitCallback)(void);

typedef struct {
	GetListCallback receive_list;
	GetDetailCallback receive_detail;
	DataSourceInitCallback ready;
} DataSourceCallbacks;

void data_source_init(DataSourceCallbacks callbacks);

void data_source_deinit();

void data_source_get_list(char **codes,int length);

void data_source_get_detail(char *code);