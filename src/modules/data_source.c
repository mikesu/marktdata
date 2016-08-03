#include <pebble.h>
#include "data_source.h"

static DataSourceCallbacks s_callbacks;
static DataList* s_data_list = NULL;
static DataDetail* s_data_detail = NULL;

static char* s_data;

static void inbox_received_handler(DictionaryIterator *iter, void *context);
static void outbox_sent_callback(DictionaryIterator *iterator, void *context);
static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context);
static void received_list(DictionaryIterator *iter);
static void received_detail(DictionaryIterator *iter);
static DataItem* received_item(DictionaryIterator *iter);
static void data_source_free_list(DataList* data_list);
static void data_source_free_detail(DataDetail* data_detail);
static void free_item(DataItem* dataItem);
static void send_data(char* action,char* data);
static char *translate_error(AppMessageResult result);
static char* dict_find_str(DictionaryIterator *iter,const uint32_t key);


void data_source_init(DataSourceCallbacks callbacks){
	s_callbacks = callbacks;
  	const int inbox_size = app_message_inbox_size_maximum();
  	const int outbox_size = 100;
  	app_message_open(inbox_size, outbox_size);
  	app_message_register_inbox_received(inbox_received_handler);
  	app_message_register_outbox_sent(outbox_sent_callback);
  	app_message_register_outbox_failed(outbox_failed_callback);
  	send_data("init","c ready");
}

void data_source_deinit(){
	data_source_free_list(s_data_list);
	data_source_free_detail(s_data_detail);
}

void data_source_get_list(char **codes,uint16_t length){
	if(s_data==NULL){
		uint16_t total = 0;
		for (uint16_t i = 0; i < length; ++i){
			total = total + strlen(codes[i]) + 1;
		}
		s_data = malloc(total);
		memset(s_data,0,total);
		for (uint16_t i = 0; i < length; ++i){
			strcat(s_data, codes[i]);
			if(i!=length-1){
				strcat(s_data,",");
			}
		}
		send_data("get_list",s_data);
	}
}

void data_source_get_detail(char *code){
	if(s_data==NULL){
		s_data = malloc(strlen(code)+1);
		strcpy(s_data, code);
		send_data("get_detail",s_data);
	}
}

static void data_source_free_list(DataList* data_list){
	APP_LOG(APP_LOG_LEVEL_DEBUG,"data_source_free_list");
	if(data_list){
		for (uint16_t i=0; i < data_list->size; ++i){
			DataItem* data_item = data_list->data_items[i];
			free_item(data_item);
			data_item = NULL;
		}
		free(data_list->data_items);
		data_list->data_items = NULL;
		free(data_list);
	}

}

static void data_source_free_detail(DataDetail* data_detail){
	APP_LOG(APP_LOG_LEVEL_DEBUG,"data_source_free_detail");
	if(data_detail){
		free_item(data_detail->data_item);
		data_detail->data_item = NULL;
		if(data_detail->img_data){
			free(data_detail->img_data);
			data_detail->img_data = NULL;
		}
		free(data_detail);
	}
}


static void send_data(char* action,char* data){
	DictionaryIterator* dict;
  	app_message_outbox_begin(&dict);
  	dict_write_cstring(dict, MESSAGE_KEY_action, action);
  	dict_write_cstring(dict, MESSAGE_KEY_data, data);
  	AppMessageResult result = app_message_outbox_send();
  	APP_LOG(APP_LOG_LEVEL_DEBUG, "%s", translate_error(result));
}


static void inbox_received_handler(DictionaryIterator *iter, void *context) {
	Tuple *init_t = dict_find(iter, MESSAGE_KEY_init);
	if(init_t){
		s_callbacks.ready();
	}
	received_list(iter);
	received_detail(iter);
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
	char* action = dict_find_str(iterator,MESSAGE_KEY_action);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "SENT: %s, %s", action,s_data);
	free(action);
	free(s_data);
	s_data = NULL;
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
	char* action = dict_find_str(iterator,MESSAGE_KEY_action);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "ERROR: %s, %s", action,s_data);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "%s", translate_error(reason));
	free(action);
	free(s_data);
	s_data = NULL;
}

static void received_list(DictionaryIterator *iter){
	static uint16_t data_list_index = 0;
	static bool data_list_completed = true;
	static DataList* data_list = NULL;

	Tuple *list_size_t = dict_find(iter, MESSAGE_KEY_list_size);
	if(list_size_t) {
		if(data_list_completed){
			data_list_completed = false;
		}else{
			data_source_free_list(data_list);
		}
		data_list_index = 0;
		//分配内存
		data_list = (DataList*)malloc(sizeof(DataList));
		data_list->size = list_size_t->value->uint16;
		data_list->data_items = (DataItem**)malloc(data_list->size * sizeof(DataItem*));
	}
	if(!data_list_completed){
		DataItem* data_item = received_item(iter);
		if(data_item) {
			data_list->data_items[data_list_index] = data_item;
			data_list_index ++;
		}
		if(data_list_index==data_list->size) {
			data_list_completed = true;
			s_callbacks.receive_list(data_list);
			data_source_free_list(s_data_list);
			s_data_list = data_list;
		}
	}
}

static void received_detail(DictionaryIterator *iter){
	static DataDetail* data_detail = NULL;
	static uint32_t data_detail_img_index = 0;
	static bool data_detail_completed = true;

	Tuple *img_size_t = dict_find(iter, MESSAGE_KEY_img_size);
  if(img_size_t) {
		if(data_detail_completed){
			data_detail_completed = false;
		}else{
			data_source_free_detail(data_detail);
		}
		data_detail_img_index = 0;
		data_detail = (DataDetail*)malloc(sizeof(DataDetail));
		data_detail->img_size = img_size_t->value->uint32;
    data_detail->img_data = (uint8_t*)malloc(data_detail->img_size * sizeof(uint8_t));
  }
	if(!data_detail_completed){
		// An image chunk
		Tuple *chunk_t = dict_find(iter, MESSAGE_KEY_img_chunk);
		if(chunk_t) {
			uint8_t *chunk_data = chunk_t->value->data;

			Tuple *chunk_size_t = dict_find(iter, MESSAGE_KEY_img_chunk_size);
			uint32_t chunk_size = chunk_size_t->value->uint32;

			data_detail_img_index = data_detail_img_index + chunk_size;

			Tuple *index_t = dict_find(iter, MESSAGE_KEY_img_chunk_index);
			uint32_t index = index_t->value->uint32;

			// Save the chunk
			memcpy(&data_detail->img_data[index], chunk_data, chunk_size);
			APP_LOG(APP_LOG_LEVEL_ERROR, "received img index:%lu,size:%lu",index,chunk_size);
		}

		DataItem* data_item = received_item(iter);
		if(data_item){
			data_detail->data_item = data_item;
		}

		if(data_detail->data_item&&(data_detail_img_index == data_detail->img_size)){
			data_detail_completed = true;
			s_callbacks.receive_detail(data_detail);
			data_source_free_detail(s_data_detail);
			s_data_detail = data_detail;
		}
	}
}

static char* dict_find_str(DictionaryIterator *iter,const uint32_t key){
	Tuple *str_t = dict_find(iter,key);
	if(str_t){
		char* result = malloc(strlen(str_t->value->cstring)+1);
		strcpy(result, str_t->value->cstring);
		return result;
	}else{
		return NULL;
	}
}

static void free_item(DataItem* data_item){
	APP_LOG(APP_LOG_LEVEL_DEBUG,"free_item");
	if(data_item){
		free(data_item->code);
		data_item->code = NULL;
		free(data_item->name);
		data_item->name = NULL;
		free(data_item->value);
		data_item->value = NULL;
		free(data_item->point);
		data_item->point = NULL;
		free(data_item->rate);
		data_item->rate = NULL;
		free(data_item->volume);
		data_item->volume = NULL;
		free(data_item->turnover);
		data_item->turnover = NULL;
		free(data_item);
	}
}

static DataItem* received_item(DictionaryIterator *iter){
	char* code = dict_find_str(iter,MESSAGE_KEY_item_code);
	if(code){
		DataItem* data_item = (DataItem*)malloc(sizeof(DataItem));
		data_item->code = code;
		data_item->name	= dict_find_str(iter,MESSAGE_KEY_item_name);
  	data_item->value = dict_find_str(iter,MESSAGE_KEY_item_value);
		data_item->point = dict_find_str(iter,MESSAGE_KEY_item_point);
		data_item->rate	= dict_find_str(iter,MESSAGE_KEY_item_rate);		//涨跌率(%)
		data_item->volume = dict_find_str(iter,MESSAGE_KEY_item_volume);;	//成交量
		data_item->turnover = dict_find_str(iter,MESSAGE_KEY_item_turnover);;	//成交额
		return data_item;
	}else{
		return NULL;
	}
}

static char *translate_error(AppMessageResult result) {
	switch (result) {
    	case APP_MSG_OK: return "APP_MSG_OK";
    	case APP_MSG_SEND_TIMEOUT: return "APP_MSG_SEND_TIMEOUT";
	    case APP_MSG_SEND_REJECTED: return "APP_MSG_SEND_REJECTED";
	    case APP_MSG_NOT_CONNECTED: return "APP_MSG_NOT_CONNECTED";
	    case APP_MSG_APP_NOT_RUNNING: return "APP_MSG_APP_NOT_RUNNING";
	    case APP_MSG_INVALID_ARGS: return "APP_MSG_INVALID_ARGS";
	    case APP_MSG_BUSY: return "APP_MSG_BUSY";
	    case APP_MSG_BUFFER_OVERFLOW: return "APP_MSG_BUFFER_OVERFLOW";
	    case APP_MSG_ALREADY_RELEASED: return "APP_MSG_ALREADY_RELEASED";
	    case APP_MSG_CALLBACK_ALREADY_REGISTERED: return "APP_MSG_CALLBACK_ALREADY_REGISTERED";
	    case APP_MSG_CALLBACK_NOT_REGISTERED: return "APP_MSG_CALLBACK_NOT_REGISTERED";
	    case APP_MSG_OUT_OF_MEMORY: return "APP_MSG_OUT_OF_MEMORY";
	    case APP_MSG_CLOSED: return "APP_MSG_CLOSED";
	    case APP_MSG_INTERNAL_ERROR: return "APP_MSG_INTERNAL_ERROR";
	    default: return "UNKNOWN ERROR";
  }
}


