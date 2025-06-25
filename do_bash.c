#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <cjson/cJSON.h>

void run_from_input(char *key) {
	pid_t pid = fork();
	if (pid == 0){
		if(strcmp(key, "F9") == 0) {
			system("firefox &");
		}
		if(strcmp(key, "F10") == 0) {
			execlp("kitty", "kitty", "--detach", NULL);
		}
		if(strcmp(key, "F11") == 0) {
			system("flatpak --user run com.revolutionarygamesstudio.ThriveLauncher &");
		}
		exit(0);
	}
	if (pid != 0) {
		printf("%s\n", key);
		return;
	}
}

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
	}

	//Print the JSON object
	char *json_string = cJSON_Print(current_json); 
	printf("%s",json_string);
	free(json_string);

	return;
}
