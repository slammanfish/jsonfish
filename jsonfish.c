#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <inttypes.h>

#include "jsonfish.h"

static void parse_value(char **str, json_t json) {
	char *key = NULL;
	if (json.key) {
		key = strstr(*str, json.key);
	} else {
		key = *str;
	}

	char *start = strchr(key, ':') + 1;
	char *end = strchr(start, ',');
	size_t len = end - start + 1;

	if (json.type == JSON_VALUE_TYPE_OBJECT) {
		for (unsigned int i = 0; i < json.count; i++) {
			parse_value(str, ((json_t*)json.value)[i]);
		}
	} else if (json.type == JSON_VALUE_TYPE_ARRAY) {
		start++;

		char vals[json.count][JSON_MAX_STRING];
		int vali[json.count];
		float valf[json.count];
		bool valb[json.count];

		for (unsigned int i = 0; i < json.count; i++) {
			if (json.array_type == JSON_VALUE_TYPE_STRING) {
				strncpy(vals[i], start + 1, len - 3);
			} else if (json.array_type == JSON_VALUE_TYPE_INT) {
				char num[len + 1];
				snprintf(num, len, "%s", start);
				num[len] = '\0';
				vali[i] = strtol(num, NULL, 10);
			} else if (json.array_type == JSON_VALUE_TYPE_FLOAT) {
				char num[len + 1];
				snprintf(num, len, "%s", start);
				num[len] = '\0';
				valf[i] = strtof(num, NULL);
			} else if (json.array_type == JSON_VALUE_TYPE_BOOLEAN) {
				if (start[0] == 't') {
					valb[i] = true;
				} else if (start[0] == 'f') {
					valb[i] = false;
				}
			} else if (json.array_type == JSON_VALUE_TYPE_NULL) {
				json.value = NULL;
			}

			if (i < json.count - 1) {
				start = end + 1;
				end = strchr(start, ',');
				len = end - start + 1;
			}
		}

		if (json.array_type == JSON_VALUE_TYPE_STRING) {
			memcpy(json.value, vals, sizeof(char) * JSON_MAX_STRING * json.count);
		} else if (json.array_type == JSON_VALUE_TYPE_INT) {
			memcpy(json.value, vali, sizeof(int) * json.count);
		} else if (json.array_type == JSON_VALUE_TYPE_FLOAT) {
			memcpy(json.value, valf, sizeof(float) * json.count);
		} else if (json.array_type == JSON_VALUE_TYPE_BOOLEAN) {
			memcpy(json.value, valb, sizeof(bool) * json.count);
		} 
	} else if (json.type == JSON_VALUE_TYPE_STRING) {
		strncpy(json.value, start + 1, len - 3);
	} else if (json.type == JSON_VALUE_TYPE_INT) {
		char num[len + 1];
		snprintf(num, len, "%s", start);
		num[len] = '\0';
		*(int*)json.value = strtol(num, NULL, 10);
	} else if (json.type == JSON_VALUE_TYPE_FLOAT) {
		char num[len + 1];
		snprintf(num, len, "%s", start);
		num[len] = '\0';
		*(float*)json.value = strtof(num, NULL);
	} else if (json.type == JSON_VALUE_TYPE_BOOLEAN) {
		if (start[0] == 't') {
			*(bool*)json.value = true;
		} else if (start[0] == 'f') {
			*(bool*)json.value = false;
		}
	} else if (json.type == JSON_VALUE_TYPE_NULL) {
		json.value = NULL;
	}

	(*str) = end;
}

void json_parse(char *str, json_t json) {
	size_t len = strlen(str);
	char *json_str = JSON_MALLOC(sizeof(char) * len);
	size_t size = 0;

	for (size_t i = 0; i < len; i++) {
		if (!isspace(str[i])) {
			json_str[size++] = str[i];
		}
	}

	json_str = JSON_REALLOC(json_str, sizeof(char) * size + 1);
	json_str[size] = '\0';

	char *temp = json_str;
	parse_value(&temp, json);

	JSON_FREE(json_str);
}

void json_read(const char *path, json_t json) {
	FILE *fp = fopen(path, "r");

	if (fp) {
		fseek(fp, 0, SEEK_END);
		size_t size = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		char *json_str = JSON_MALLOC(sizeof(char) * size + 1);
		if (!json_str) {
			printf("Failed to allocate string\n");
			fclose(fp);
		}

		fread(json_str, size, sizeof(char), fp);
		fclose(fp);

		json_str[size] = '\0';

		json_parse(json_str, json);

		JSON_FREE(json_str);
	} else {
		printf("Failed to open file: %s\n", path);
	}
}

#define ADD(_str) do { \
		if (*(len) > INT32_MAX) break; \
		snprintf(*(str) + *(len), INT32_MAX - *(len), "%s", (_str)); \
		*(len) += strlen((_str)); \
	} while (0)

#define IND(_lvl) do { \
		for (int i = 0; i < (_lvl); i++) ADD("\t"); \
	} while (0)

static void save_value(const json_t json, char **str, size_t *len, int level, bool is_pretty) {
	if (json.key) {
		ADD("\"");
		ADD(json.key);
		ADD("\"");
		ADD(":");
		if (is_pretty) {
			ADD(" ");
		}
	}

	if (json.type == JSON_VALUE_TYPE_OBJECT) {
		ADD("{");

		if (is_pretty) {
			ADD("\n");
		}

		for (unsigned int i = 0; i < json.count; i++) {
			if (is_pretty) {
				IND(level);
			}

			save_value(((json_t*)json.value)[i], str, len, level + 1, is_pretty);

			if (i < json.count - 1) {
				ADD(",");
			}

			if (is_pretty) {
				ADD("\n");
			}
		}

		if (is_pretty) {
			IND(level - 1);
		}

		ADD("}");
	} else if (json.type == JSON_VALUE_TYPE_ARRAY) {
		ADD("[");

		if (is_pretty) {
			ADD("\n");
		}

		for (unsigned int i = 0; i < json.count; i++) {
			if (is_pretty) {
				IND(level);
			}

			if (json.array_type == JSON_VALUE_TYPE_STRING) {
				ADD("\"");
				ADD(((char**)json.value)[i]);
				ADD("\"");
			} else if (json.array_type == JSON_VALUE_TYPE_INT) {
				char value[JSON_MAX_STRING];
				snprintf(value, JSON_MAX_STRING, "%d", ((int*)json.value)[i]);
				ADD(value);
			} else if (json.array_type == JSON_VALUE_TYPE_FLOAT) {
				char value[JSON_MAX_STRING];
				snprintf(value, JSON_MAX_STRING, "%f", ((float*)json.value)[i]);
				ADD(value);
			} else if (json.array_type == JSON_VALUE_TYPE_BOOLEAN) {
				if (((bool*)json.value)[i]) {
					ADD("true");
				} else {
					ADD("false");
				}
			} else if (json.array_type == JSON_VALUE_TYPE_NULL) {
				ADD("null");
			}

			if (i < json.count - 1) {
				ADD(",");
			}

			if (is_pretty) {
				ADD("\n");
			}
		}

		if (is_pretty) {
			IND(level - 1);
		}

		ADD("]");
	} else if (json.type == JSON_VALUE_TYPE_STRING) {
		ADD("\"");
		ADD((char*)json.value);
		ADD("\"");
	} else if (json.type == JSON_VALUE_TYPE_INT) {
		char value[JSON_MAX_STRING];
		snprintf(value, JSON_MAX_STRING, "%d", *(int*)json.value);
		ADD(value);
	} else if (json.type == JSON_VALUE_TYPE_FLOAT) {
		char value[JSON_MAX_STRING];
		snprintf(value, JSON_MAX_STRING, "%f", *(float*)json.value);
		ADD(value);
	} else if (json.type == JSON_VALUE_TYPE_BOOLEAN) {
		if (*(bool*)json.value) {
			ADD("true");
		} else {
			ADD("false");
		}
	} else if (json.type == JSON_VALUE_TYPE_NULL) {
		ADD("null");
	}
}

#undef IND
#undef ADD

char *json_save(const json_t json, size_t *len, bool is_pretty) {
	char *json_str = JSON_MALLOC(sizeof(char) * INT32_MAX);
	
	*len = 0;

	save_value(json, &json_str, len, 1, is_pretty);

	json_str = realloc(json_str, sizeof(char) * *len + 1);
	json_str[*len] = '\0';

	return json_str;
}

void json_write(const char *path, const json_t json, bool is_pretty) {
	FILE *fp = fopen(path, "w");

	if (fp) {
		size_t len = 0;
		char *json_str = json_save(json, &len, is_pretty);

		fprintf(fp, "%s", json_str);
		fclose(fp);

		JSON_FREE(json_str);
	} else {
		printf("Failed to open file: %s\n", path);
	}
}
