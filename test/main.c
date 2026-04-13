#include <stdio.h>

#include "../jsonfish.h"

struct {
	char title[JSON_MAX_STRING];
	int num;
	float numf;

	struct {
		int resolution[2];
		bool is_full_screen;
	} graphics;
} settings;

int main(int argc, char **argv) {
	json_t settings_map = (json_t){
		.type = JSON_VALUE_TYPE_OBJECT,
		.key = NULL,
		.count = 4,
		.value = (json_t[]){
			(json_t){
				.type = JSON_VALUE_TYPE_STRING,
				.key = "title",
				.value = &settings.title,
			},
			(json_t){
				.type = JSON_VALUE_TYPE_INT,
				.key = "num",
				.value = &settings.num,
			},
			(json_t){
				.type = JSON_VALUE_TYPE_FLOAT,
				.key = "numf",
				.value = &settings.numf,
			},
			(json_t){
				.type = JSON_VALUE_TYPE_OBJECT,
				.key = "graphics",
				.count = 2,
				.value = (json_t[]){
					(json_t){
						.type = JSON_VALUE_TYPE_ARRAY,
						.key = "resolution",
						.value = &settings.graphics.resolution,
						.array_type = JSON_VALUE_TYPE_INT,
						.count = 2,
					},
					(json_t){
						.type = JSON_VALUE_TYPE_BOOLEAN,
						.key = "is_full_screen",
						.value = &settings.graphics.is_full_screen,
					},
				},
			},
		}
	};

	// read json file
	json_read("../settings.json", settings_map);

	// save json file
	json_write("test.json", settings_map, true);
}
