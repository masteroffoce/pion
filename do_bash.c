#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <cjson/cJSON.h>
#include <gtk/gtk.h>
#include "do_bash.h"

void read_keyboard(KeyBoard *keyboard) {
	char file_path[] = "keyboard.layout";
	FILE *keyboard_file = fopen(file_path, "r");
	char buffer[1024];

	keyboard->height = 0;
	
	while(fgets(buffer, sizeof(buffer), keyboard_file)) {
		char *p = buffer;
		int col = 0;

		while (*p != '\0' && *p != '\n') {
			while (*p == ' ') p++; //Go to start of word
			char *start = p; //Record start of word
			while (*p != ' ' && *p != '\n' && *p != '\0') p++; //Go to end of word

			size_t len = p - start; //Record length of key name
			memcpy(keyboard->keyrows[keyboard->height].keys[col].key,start,len); //Copy key name to keyboard strut
			keyboard->keyrows[keyboard->height].keys[col].key[len] = '\0'; //Null-terminate string
			keyboard->keyrows[keyboard->height].length++; //Increase length of keyrow with added key
			strcpy(keyboard->keyrows[keyboard->height].keys[col].name,"cool");
			col++; 


			if (*p == '\n') { //If last key of line
				col = 0;
				keyboard->height++;
				keyboard->keyrows[keyboard->height].length = 0;
			}

			
		}
	}

	fclose(keyboard_file);
}

void reed_keyboard(KeyBoard keyboard) {
	char file_path[] = "keyboard.layout";
	FILE *keyboard_file = fopen(file_path, "r");
	char buffer[1024];
	//int buffer_len = fread(buffer, 1, sizeof(buffer), keyboard_file);
	//KeyBoard keyboard;
	keyboard.height = 0;
	int row = 0;
	int col = 0;
	
	Key keys[MAX_KEYS];
	int num_of_keys = 0;
	while(fgets(buffer, sizeof(buffer), keyboard_file)) {
		

		char *p = buffer;

		while (*p != '\0') {
			if (num_of_keys > MAX_KEYS) {
				printf("Too many keys!");
				break;
			}

			int spaces_before = 0;
			while (*p == ' ') {
				spaces_before++;
				p++;
			}

			if (*p == '\0') break;

			char *start = p;

			while (*p != '\0' && *p != ' ') { //Increment p while in word
				if (*p == '\n') {
					row++;
					keyboard.height++;
					col = -1;
				}
				p++;
			}

			int key_len = p - start;
			if (key_len > MAX_KEY_LEN-1) key_len = MAX_KEY_LEN-1;

			int spaces_after = 0;
			while (*p == ' ') {
				spaces_after++;
				p++;
			}

			memcpy(keys[num_of_keys].key, start, key_len);
			keys[num_of_keys].key[key_len] = '\0';
			keys[num_of_keys].before = spaces_before;
			keys[num_of_keys].after = spaces_after;

			num_of_keys++;
			p -= spaces_after;
			col++;

			//Add key to the big keyboard struct
			//printf("\n%d",key_len);
			memcpy(keyboard.keyrows[row].keys[col-1].key, start, key_len);
			keyboard.keyrows[row].keys[col-1].key[key_len] = '\0';
			keyboard.keyrows[row].keys[col-1].before = spaces_before;
			keyboard.keyrows[row].keys[col-1].after = spaces_after;
			keyboard.keyrows[row].length = col;
		}
	}
	fclose(keyboard_file);

	for (int i = 0; i < num_of_keys; i++) {
		
		//printf("Key: %s Before: %d After: %d\n", keys[i].key, keys[i].before, keys[i].after);
		
	}
}

void fill_keyboard(GPtrArray *keys_arr, KeyBoard keyboard) {
	char file_path[256];
	const char *xdg_config_home = getenv("HOME");
	snprintf(file_path, sizeof(file_path), "%s/.config/pion/layout.json", xdg_config_home);

	FILE *layout_file = fopen(file_path, "r");
	char buffer[65536];
	int len = fread(buffer, 1, sizeof(buffer), layout_file);
	(void)len;
	fclose(layout_file);

	cJSON *json = cJSON_Parse(buffer);

	//Find correct path, the correct list of keys
	for (guint i = 0; i < keys_arr->len; i++) {
		json = cJSON_GetObjectItem(json, (char *) keys_arr->pdata[i]);
	}

	printf("\n->");
	cJSON *key = json->child;
	while(key) {
		printf("%s ",key->string);
		//printf("%s",cJSON_GetObjectItem(key, "name")->string);
		key = key->next;
	}
}

int exec_json(GPtrArray *keys_arr) {
	
	//Get the filepath
	char file_path[256];
	const char *xdg_config_home = getenv("HOME");
	snprintf(file_path, sizeof(file_path), "%s/.config/pion/layout.json", xdg_config_home);

	//Open the config file
	FILE *layout_file = fopen(file_path, "r");
	char buffer[65536];
	int len = fread(buffer, 1, sizeof(buffer), layout_file);
	(void)len;
	fclose(layout_file);

	cJSON *json = cJSON_Parse(buffer);

	
	//Do a nested search
	cJSON *current_json = json;

	for (guint i = 0; i < keys_arr->len; ++i) {
		//current_json = cJSON_GetObjectItem(current_json, (char[]) {keys_arr[i], '\0'} );
		current_json = cJSON_GetObjectItem(current_json, (char *) keys_arr->pdata[i]);
	}

	//If there's a "shell" or "app" value, take action
	cJSON *shell = cJSON_GetObjectItem(current_json, "shell");
	if (cJSON_IsString(shell)) {
		int return_value = system(shell->valuestring);
		(void) return_value;
		exit(1);
	}

	//Otherwise, exit the program
	if (cJSON_IsObject(current_json) && current_json->child != NULL) {
		//execlp("./run", "run", keys_arr, NULL);
		return 0;
	}

	//Print the JSON object
	char *json_string = cJSON_Print(current_json); 
	free(json_string);

	return 1;
}

GPtrArray* presuffix_keys_arr(GPtrArray *keys_arr) {
	GPtrArray *fixxed;
	fixxed = g_ptr_array_new_with_free_func((GDestroyNotify)g_free);
	for (uint i = 0; i < keys_arr->len; i++) {
		char* keys_arr_elem = g_ptr_array_index(keys_arr, i); //Define current key

		//Add pre- and suffixes if the key isn't a letter or number
		char fixxed_string[12] = "";
		if (strlen(keys_arr_elem) > 1) {
			strcat(fixxed_string, "[");
		}
		strcat(fixxed_string, keys_arr_elem);
		if (strlen(keys_arr_elem) > 1) {
			strcat(fixxed_string, "]");
		}
		g_ptr_array_add(fixxed, g_strdup(fixxed_string));
	}
	return fixxed;
}
