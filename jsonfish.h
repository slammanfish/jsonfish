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
	JSON_VALUE_TYPE_INT,
	JSON_VALUE_TYPE_FLOAT,
	JSON_VALUE_TYPE_BOOLEAN,
	JSON_VALUE_TYPE_NULL,
} json_value_type_e;

typedef struct json {
	char *key; // the value key, NULL for the root object
	json_value_type_e type; // the value type
	struct {
		void *value;	// pointer to value in the c struct or
                	// array of json_t
		size_t count; // number of child objects or array length
		json_value_type_e array_type; // the array element type, 
                                  // ignored if type != JSON_VALUE_TYPE_ARRAY
	};
} json_t;

/* @brief parse string to json
 * 
 * @param str json string
 * @param json json map root object
 */
void json_parse(char *str, json_t json);
/* @brief read file and parse string to json
 *
 * @param path file path
 * @param json json map root object
 */
void json_read(const char *path, json_t json);
/* save json to string
 *
 * @param json json map root object
 * @param len length of the serialised string
 * @param is_pretty should the string be pretty
 * @return serialised string
 */
char *json_save(const json_t json, size_t *len, bool is_pretty);
/* save json to string and write to file
 *
 * @param path file path
 * @param json json map root object
 * @param is_pretty should the string be pretty
 */
void json_write(const char *path, const json_t json, bool is_pretty);

#endif
