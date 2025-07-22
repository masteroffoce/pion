#ifndef UI_H
#define UI_H

typedef struct Key Key;
typedef struct KeyRow KeyRow;
typedef struct KeyBoard KeyBoard;

void read_keyboard();

int print_json(GPtrArray *keys_arr);
GPtrArray* presuffix_keys_arr(GPtrArray *keys_arr);

#endif
