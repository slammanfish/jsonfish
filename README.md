# [json](https://json.org)fish

* simple
* lightweight
* C99

## about
**jsonfish** works by constructing a **map** that corresponds to a **c struct**.
### API
the **API** is incredibly simple.<br>
using the ```json_t``` struct, you define a **map** that reflects a **c struct**
```c
typedef struct json {
	char *key; // the value key, NULL for the root object
	json_value_type_e type; // the value type
	struct {
		void *value;  // pointer to value in the c struct or
                      // array of json_t
		size_t count; // number of child objects or array length
		json_value_type_e array_type; // the array element type, 
                                      // ignored if type != JSON_VALUE_TYPE_ARRAY
	};
} json_t;
```
a simple map would look like this
```c
/*
example.json
{
    "number": 10
}
*/

struct {
    int number;
} example;

json_t example_map = (json_t){
    .type = JSON_VALUE_TYPE_OBJECT,
    .key = NULL,
    .count = 1,
    .value = (json_t[]){
        (json_t){
            .type = JSON_VALUE_TYPE_INT,
            .key = "number",
            .value = &example.number,
        },
    }
};
```

### limitations
to maintain simplicity, i made a few shortcuts
these include:

* the json map **MUST** be in the same order as the json string
* strings cannot contain ' ', '"', ',', ':'
* strings cannot contain control characters (spaces are allowed)
* arrays cannot store objects
* arrays cannot store arrays

this was made pretty much exclusively to load settings for my game so these features werent worth implementing (skill issue or lazy?)
if you need a proper, full featured json parser, i recommend [parson](https://github.com/kgabis/parson)

## examples
### parsing a settings file
```c
/*
settings.json
{
	"title": "application",
	"graphics": {
		"resolution": [
			1920,
			1080
		],
		"is_full_screen": false
	}
}
*/

#include "jsonfish.h"

struct {
	char title[JSON_MAX_STRING];

	struct {
		int resolution[2];
		bool is_full_screen;
	} graphics;
} settings;

int main(int argc, char **argv) {
    json_t settings_map = (json_t){
        .type = JSON_VALUE_TYPE_OBJECT,
        .key = NULL,
        .count = 2,
        .value = (json_t[]){
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

    json_read("../settings.json", settings_map);
}
```
