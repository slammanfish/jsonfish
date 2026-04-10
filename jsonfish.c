#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "jsonfish.h"

char *eat_whitespace(char *str);
char *get_next(char *str);
void parse_value(char **str, json_t json);

void object(char **str, json_t json);
void array(char **str, json_t json);
void string(char *str, char *end, char value[JSON_MAX_STRING]);
float number(char *str, char *end);
bool boolean(char *str, char *end);

char *eat_whitespace(char *str) {
	size_t len = strlen(str);
	char *raw = JSON_MALLOC(sizeof(char) * len);
	
	size_t size = 0;
	char c;
	bool in_string = false;

	for (size_t i = 0; i < len; i++) {
		c = str[i];

		if (c == '"') {
			in_string = !in_string;
		}

		if (c == '\\') {
			c = str[++i];
			switch (c) {
				case '\\': raw[size++] = '\\'; break;
				case '\"': raw[size++] = '\"'; break;
				case '/': raw[size++] = '/'; break;
				case 'b': raw[size++] = '\b'; break;
				case 'f': raw[size++] = '\f'; break;
				case 'n': raw[size++] = '\n'; break;
				case 'r': raw[size++] = '\r'; break;
				case 't': raw[size++] = '\t'; break;
				case 'u': break;
			}
		} else if (in_string) {
			raw[size++] = c;
		} else {
			if (!isspace(c)) {
				raw[size++] = c;
			}
		}
	}

	raw = JSON_REALLOC(raw, sizeof(char) * size + 1);
	raw[size] = '\0';

	return raw;
}

char *get_next(char *str) {
	size_t len = strlen(str);
	
	char c;
	bool in_string = false;

	for (size_t i = 0; i < len; i++) {
		c = str[i];

		if (c == '"') {
			in_string = !in_string;
		}

		if (c == '\\') {
			c = str[++i];
		} else if (!in_string) {
			if (c == ',') {
				return str + i;
			}
		}
	}

	return str;
}

void object(char **str, json_t json) {
	for (unsigned int i = 0; i < json.count; i++) {
		parse_value(str, ((json_t*)json.value)[i]);
	}
}

void array(char **str, json_t json) {
	char *key_str = strstr(*str, json.key);
	char *value_str = strchr(key_str, ':') + 2;
	char *value_end = get_next(value_str);

	char value_string[json.count][JSON_MAX_STRING];
	float value_number[json.count];
	bool value_boolean[json.count];

	for (size_t i = 0; i < json.count; i++) {
		if (json.array_type == JSON_VALUE_TYPE_STRING) {
			string(value_str, value_end, value_string[i]);
		} else if (json.array_type == JSON_VALUE_TYPE_NUMBER) {
			value_number[i] = number(value_str, value_end);
		} else if (json.array_type == JSON_VALUE_TYPE_BOOLEAN) {
			value_boolean[i] = boolean(value_str, value_end);
		}

		value_str = value_end + 1;
		value_end = get_next(value_str);
	}

	if (json.array_type == JSON_VALUE_TYPE_STRING) {
		memcpy(json.value, value_str, sizeof(char) * JSON_MAX_STRING * json.count);
	} else if (json.array_type == JSON_VALUE_TYPE_NUMBER) {
		memcpy(json.value, value_number, sizeof(float) * json.count);
	} else if (json.array_type == JSON_VALUE_TYPE_BOOLEAN) {
		memcpy(json.value, value_boolean, sizeof(bool) * json.count);
	}

	*str = (char*)value_end;
}

void string(char *str, char *end, char value[JSON_MAX_STRING]) {
	size_t i = 0;
	for (const char *c = str + 1; i < JSON_MAX_STRING && c && c != end - 1; c++) {
		value[i++] = *c;
	}
	value[JSON_MAX_STRING - 1] = '\0';
}

float number(char *str, char *end) {
	size_t len = end - str + 1;
	char number[len + 1];
	snprintf(number, len, "%s", str);
	number[len] = '\0';
	char *ends;
	float value = strtof(number, &ends);
	return value;
}

bool boolean(char *str, char *end) {
	bool value = false;
	if (str[0] == 't') {
		value = true;
	} else if (str[0] == 'f') {
		value = false;
	}
	return value;
}

void parse_value(char **str, json_t json) {
	char *key_str = strstr(*str, json.key);
	char *value_str = strchr(key_str, ':') + 1;
	char *value_end = get_next(value_str);

	if (json.type == JSON_VALUE_TYPE_OBJECT) {
		object(str, json);
	} else if (json.type == JSON_VALUE_TYPE_ARRAY) {
		array(str, json);
	} else if (json.type == JSON_VALUE_TYPE_STRING) {
		char value[JSON_MAX_STRING];
		string(value_str, value_end, value);
		memcpy(json.value, value, JSON_MAX_STRING);
	} else if (json.type == JSON_VALUE_TYPE_NUMBER) {
		float value = number(value_str, value_end);
		memcpy(json.value, &value, sizeof(float));
	} else if (json.type == JSON_VALUE_TYPE_BOOLEAN) {
		bool value = boolean(value_str, value_end);
		memcpy(json.value, &value, sizeof(bool));
	}

	*str = (char*)value_end;
}

bool json_parse(char *str, json_t json[], unsigned int count) {
	char *raw_start = eat_whitespace(str);
	char *raw = raw_start;

	for (unsigned int i = 0; i < count; i++) {
		parse_value(&raw, json[i]);
	}

	JSON_FREE(raw_start);

	return true;
}

bool json_read(const char *path, json_t json[], unsigned int count) {
	FILE *fp = fopen(path, "r");

	if (fp) {
		fseek(fp, 0, SEEK_END);
		size_t size = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		char *str = JSON_MALLOC(sizeof(char) * size + 1);
		if (!str) {
			printf("Failed to allocate string");
			fclose(fp);
			return false;
		}

		fread(str, size, sizeof(char), fp);
		fclose(fp);

		str[size] = '\0';

		bool success = json_parse(str, json, count);

		JSON_FREE(str);
		return success;
	}

	printf("Failed to open file: %s", path);
	return false;
}
