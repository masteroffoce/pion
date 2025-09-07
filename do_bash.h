#ifndef UI_H
#define UI_H

#define MAX_KEYS 140
#define MAX_KEY_LEN 10
#define MAX_ROW_LEN 24
#define MAX_KEYBOARD_HEIGHT 6

void init_do_bash();

typedef struct {
	char name[50];
	char key[MAX_KEY_LEN];
	char shell_or_app[1024];
	int size;
	int before;
	int after;
} Key;

typedef struct {
	Key keys[MAX_ROW_LEN];
	int length;
} KeyRow;

typedef struct {
	KeyRow keyrows[MAX_KEYBOARD_HEIGHT];
	int height;
} KeyBoard;

void read_keyboard(KeyBoard *keyboard);
void fill_keyboard(GPtrArray *keys_arr, KeyBoard *keyboard);

int exec_json(GPtrArray *keys_arr);
GPtrArray* presuffix_keys_arr(GPtrArray *keys_arr);

#endif
