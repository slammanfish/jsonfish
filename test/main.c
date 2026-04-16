#include <stdio.h>

#include "../jsonfish.h"

struct {
	char title[JSON_MAX_STRING];
	int num;

	struct {
		int resolution[2];
		bool is_full_screen;
	} graphics;

	char names[3][JSON_MAX_STRING];
	float numf;
} settings;

int main(int argc, char **argv) {
	json_t settings_map = (json_t){
		.type = JSON_VALUE_TYPE_OBJECT,
		.key = NULL,
		.count = 5,
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
			(json_t){
				.type = JSON_VALUE_TYPE_ARRAY,
				.key = "names",
				.value = &settings.names,
				.array_type = JSON_VALUE_TYPE_STRING,
				.count = 3,
			},
			(json_t){
				.type = JSON_VALUE_TYPE_FLOAT,
				.key = "numf",
				.value = &settings.numf,
			},
		}
	};

	// read json file
	json_read("../settings.json", settings_map);

	printf("\n");
	printf("title: %s\n", settings.title);
	printf("num: %d\n", settings.num);
	printf("graphics:\n");
	printf("  resolution:\n");
	printf("    %d\n", settings.graphics.resolution[0]);
	printf("    %d\n", settings.graphics.resolution[1]);
	printf("  is_full_screen: %b\n", settings.graphics.is_full_screen);
	printf("names:\n");
	printf("  %s\n", settings.names[0]);
	printf("  %s\n", settings.names[1]);
	printf("  %s\n", settings.names[2]);
	printf("numf: %f\n", settings.numf);

	// save json file
	// json_write("test.json", settings_map, true);
}
