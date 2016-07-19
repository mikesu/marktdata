#pragma once

typedef void (*HttpReceiveStringCallback)(char* str,uint32_t length);

typedef void (*HttpReceiveRawCallback)(uint8_t* data,uint32_t length);

typedef struct {
	HttpReceiveStringCallback receive_string;
	HttpReceiveRawCallback receive_raw;
} HttpCallbacks;

void http_get_string(char* url);

void http_get_raw(char* url);

void http_init(HttpCallbacks callbacks);