#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

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
	}
	if (pid != 0) {
		printf("%s", key);
		return;
	}
}
