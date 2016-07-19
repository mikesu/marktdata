#include <pebble.h>
#include "data_source.h"

static bool s_js_ready;
const int inbox_size = 128;
const int outbox_size = 128;

static HttpCallbacks s_callbacks;

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Message received!");
	Tuple *ready_tuple = dict_find(iterator, MESSAGE_KEY_JSReady);
	if(ready_tuple) {
	   	APP_LOG(APP_LOG_LEVEL_DEBUG, "MESSAGE_KEY_JSReady!");
		s_js_ready = true;
	}

	// Tuple *data_tuple = dict_find(iterator, MESSAGE_KEY_Data);
	// if(data_tuple) {
	//     char *data_str = data_tuple->value->cstring;
	// 	s_callbacks.receive_string(data_str,11);
	// }
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}


void http_init(HttpCallbacks callbacks){
	s_callbacks = callbacks;
	app_message_register_inbox_received(inbox_received_callback);
	app_message_register_inbox_dropped(inbox_dropped_callback);
	app_message_register_outbox_failed(outbox_failed_callback);
	app_message_register_outbox_sent(outbox_sent_callback);
	app_message_open(inbox_size, outbox_size);
}

void http_get_string(char* url){
	// DictionaryIterator *out_iter;
	// AppMessageResult result = app_message_outbox_begin(&out_iter);
	// if(result == APP_MSG_OK) {
 //  		// Construct the message
	// 	dict_write_cstring(out_iter,MESSAGE_KEY_Code,url);
	// 	// Send this message
	// 	result = app_message_outbox_send();

	// 	// Check the result
	// 	if(result != APP_MSG_OK) {
	// 	  APP_LOG(APP_LOG_LEVEL_ERROR, "Error sending the outbox: %d", (int)result);
	// 	}

	// } else {
 //  		// The outbox cannot be used right now
 //  		APP_LOG(APP_LOG_LEVEL_ERROR, "Error preparing the outbox: %d", (int)result);
	// }
}

void http_get_raw(char* url){
	
}





