# [json](https://json.org)fish

* simple
* lightweight
* C99

## about
**jsonfish** works by constructing a **map** that corresponds to a **c struct**, creating a simple yet effective json parser.<br>
the *downside* to this is that you have to know the format of the json file to parse it.<br>
*this should not be a problem for most use cases*

## examples
### parsing a settings file

```c
#include "jsonfish.h"

struct {
	char title[JSON_MAX_STRING];
	
	struct {
		float resolution[2];
		bool is_full_screen;
	} graphics;
} settings;

int main(int argc, char **argv) {
	json_t settings_map[] = {
		(json_t){
			.type = JSON_VALUE_TYPE_STRING,
			.key = "title",
			.value = &settings.title,
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
					.array_type = JSON_VALUE_TYPE_NUMBER,
					.count = 2,
				},
				(json_t){
					.type = JSON_VALUE_TYPE_BOOLEAN,
					.key = "is_full_screen",
					.value = &settings.graphics.is_full_screen,
				},
			},
		},
	};
	
	json_read("settings.json", settings_map, 2);
}
```
