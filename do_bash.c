#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <cjson/cJSON.h>

void print_json(char *keys_arr) {
	

	int current_depth = (int) strlen(keys_arr);

	FILE *layout_file = fopen("layout.json", "r");
	char buffer[1024];
	int len = fread(buffer, 1, sizeof(buffer), layout_file);
	fclose(layout_file);

	cJSON *json = cJSON_Parse(buffer);

	
	//Do a nested search
	cJSON *current_json = json;

	for (int i = 0; i < current_depth; ++i) {
		current_json = cJSON_GetObjectItem(current_json, (char[]) {keys_arr[i], '\0'} );
	}

	//If there's a "shell" or "app" value, take action
	cJSON *shell = cJSON_GetObjectItem(current_json, "shell");
	if (cJSON_IsString(shell)) {
		system(shell->valuestring);
		exit(1);
	}

	//Otherwise, call the program again but with the new key appended
	if (cJSON_IsObject(current_json) && current_json->child != NULL) {
		execlp("./run", "run", keys_arr, NULL);
	}

	//Print the JSON object
	char *json_string = cJSON_Print(current_json); 
	printf("%s",json_string);
	free(json_string);

	return;
}
