#include <pebble.h>
#include "data_source.h"

static DataSourceCallbacks s_callbacks;
static DataList s_data_list;
static int s_data_list_index = 0;
static bool s_data_list_completed = true;
static DataDetail s_data_detail;
static int s_data_detail_img_index = 0;
static bool s_data_detail_completed = true;

static char* s_data;

static void inbox_received_handler(DictionaryIterator *iter, void *context);
static void outbox_sent_callback(DictionaryIterator *iterator, void *context);
static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context);
static void received_list(DictionaryIterator *iter);
static void received_detail(DictionaryIterator *iter);
static DataItem* received_item(DictionaryIterator *iter);
static void free_item(DataItem* dataItem);
static void send_data(char* action,char* data);
static char *translate_error(AppMessageResult result);
static char* dict_find_str(DictionaryIterator *iter,const uint32_t key);


void data_source_init(DataSourceCallbacks callbacks){
	s_callbacks = callbacks;
  	const int inbox_size = app_message_inbox_size_maximum();
  	const int outbox_size = 64;
  	app_message_open(inbox_size, outbox_size);
  	app_message_register_inbox_received(inbox_received_handler);
  	app_message_register_outbox_sent(outbox_sent_callback);
  	app_message_register_outbox_failed(outbox_failed_callback);
}

void data_source_deinit(){
	data_source_free_list(s_data_list);
	data_source_free_detail(s_data_detail);
}

void data_source_get_list(char **codes,int length){
	if(s_data==NULL){
		int total = 0;
		for (int i = 0; i < length; ++i){
			total = total + strlen(codes[i]) + 1;
		}
		s_data = malloc(total);
		memset(s_data,0,total);
		for (int i = 0; i < length; ++i){
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

void data_source_free_list(DataList data_list){
	if(data_list.data_items){
		for (int i=0; i < data_list.size; ++i){
			DataItem* data_item = *(data_list.data_items+i);
			free_item(data_item);
		}
		free(data_list.data_items);
	}

}

void data_source_free_detail(DataDetail data_detail){
	free_item(data_detail.data_item);
	if(data_detail.img_data){
		free(data_detail.img_data);
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
	received_list(iter);
	received_detail(iter);
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
	char* action = dict_find_str(iterator,MESSAGE_KEY_action);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "SENT: %s, %s", action,s_data);
	free(s_data);
	s_data = NULL;
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
	char* action = dict_find_str(iterator,MESSAGE_KEY_action);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "ERROR: %s, %s", action,s_data);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "%s", translate_error(reason));
	free(s_data);
	s_data = NULL;
}

static void received_list(DictionaryIterator *iter){
	Tuple *list_size_t = dict_find(iter, MESSAGE_KEY_list_size);
  	if(list_size_t) {
  		if(s_data_list_completed){
  			s_data_list_completed = false;
  		}else{
  			data_source_free_list(s_data_list);
  		}
  		s_data_list_index = 0;
  		s_data_list.size = list_size_t->value->int32;
  		//分配内存
  		s_data_list.data_items = (DataItem**)malloc(s_data_list.size * sizeof(DataItem*));
  	}
  	if(!s_data_list_completed){
		DataItem* data_item = received_item(iter);
		if(data_item) {
			s_data_list.data_items[s_data_list_index] = data_item;
			s_data_list_index ++;
		}
		if(s_data_list_index==s_data_list.size) {
			s_data_list_completed = true;
			s_callbacks.receive_list(s_data_list);
		}
  	}

}

static void received_detail(DictionaryIterator *iter){
	Tuple *img_size_t = dict_find(iter, MESSAGE_KEY_img_size);
  	if(img_size_t) {
  		if(s_data_detail_completed){
  			s_data_detail_completed = false;
  		}else{
  			data_source_free_detail(s_data_detail);
  		}
  		s_data_detail_img_index = 0;
		s_data_detail.img_size = img_size_t->value->int32;
    	s_data_detail.img_data = (uint8_t*)malloc(s_data_detail.img_size * sizeof(uint8_t));
  	}
  	if(!s_data_detail_completed){
  		// An image chunk
		Tuple *chunk_t = dict_find(iter, MESSAGE_KEY_img_chunk);
		if(chunk_t) {
			uint8_t *chunk_data = chunk_t->value->data;

		    Tuple *chunk_size_t = dict_find(iter, MESSAGE_KEY_img_chunk_size);
		    int chunk_size = chunk_size_t->value->int32;

		    s_data_detail_img_index = s_data_detail_img_index + chunk_size;

		    Tuple *index_t = dict_find(iter, MESSAGE_KEY_img_chunk_index);
		    int index = index_t->value->int32;

		    // Save the chunk
		    memcpy(&s_data_detail.img_data[index], chunk_data, chunk_size);
		}

		DataItem* data_item = received_item(iter);
		if(data_item){
			s_data_detail.data_item = data_item;
		}

		if(s_data_detail.data_item&&(s_data_detail_img_index == s_data_detail.img_size)){
			s_data_detail_completed = true;
			s_callbacks.receive_detail(s_data_detail);
		}
  	}
}

static char* dict_find_str(DictionaryIterator *iter,const uint32_t key){
	Tuple *str_t = dict_find(iter,key);
	if(str_t){
		return str_t->value->cstring;
	}else{
		return NULL;
	}
}

static void free_item(DataItem* data_item){
	if(data_item){
		free(data_item->code);
		free(data_item->name);
		free(data_item->value);
		free(data_item->point);
		free(data_item->rate);
		free(data_item->volume);
		free(data_item->turnover);
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


