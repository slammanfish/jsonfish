#ifndef JSONFISH_H
#define JSONFISH_H

#include <stdlib.h>

#ifndef bool
#include <stdbool.h>
#endif

#ifndef JSON_MALLOC
#define JSON_MALLOC malloc
#endif

#ifndef JSON_REALLOC
#define JSON_REALLOC realloc
#endif

#ifndef JSON_FREE
#define JSON_FREE free
#endif

#ifndef JSON_MAX_STRING
#define JSON_MAX_STRING (128)
#endif

typedef enum json_value_type {
	JSON_VALUE_TYPE_OBJECT,
	JSON_VALUE_TYPE_ARRAY,
	JSON_VALUE_TYPE_STRING,
	JSON_VALUE_TYPE_NUMBER,
	JSON_VALUE_TYPE_BOOLEAN,
	JSON_VALUE_TYPE_NULL,
} json_value_type_e;

typedef struct json {
	char *key;
	json_value_type_e type;
	struct {
		void *value;
		size_t count;
		json_value_type_e array_type;
	};
} json_t;

bool json_parse(char *str, json_t json[], unsigned int count);
bool json_read(const char *path, json_t json[], unsigned int count);

#endif
