#include <gtk/gtk.h>
#include <gtk-layer-shell.h>
#include "do_bash.h"
#include <string.h>
#include <cjson/cJSON.h>

static char * BACK_KEY = "BackSpace";
static char * ABORT_KEY = "backslash";
static int WIDTH = 1700;
static int HEIGHT = 600;
static int MARGIN = 200;
static char QUIT_ON_WRONG_KEY = 0;
static char CENTER = 0;

char *pressed_key = NULL;

typedef struct {
	GtkLabel *title_label;
	GPtrArray *keys_arr;

	GtkWidget **keys_labels;
	GtkWidget **box_labels;
	GtkWidget **name_labels;
	GtkWidget **word_labels;

	int num_of_keys;
} AppData;

void init_settings() {
	FILE *settings_file = fopen("settings.json", "r");
	char buffer[4096];
	int len = fread(buffer, 1, sizeof(buffer), settings_file);
	(void)len;
	fclose(settings_file);

	const cJSON *json = cJSON_Parse(buffer);

	
	cJSON *window = cJSON_GetObjectItemCaseSensitive(json, "window");

	WIDTH = cJSON_GetObjectItemCaseSensitive(window, "width")->valueint;
	HEIGHT = cJSON_GetObjectItemCaseSensitive(window, "height")->valueint;
	MARGIN = cJSON_GetObjectItemCaseSensitive(window, "text")->valueint;
	CENTER = cJSON_GetObjectItemCaseSensitive(window, "center")->valueint;

	cJSON *keys = cJSON_GetObjectItemCaseSensitive(json, "keys");

	BACK_KEY = cJSON_GetObjectItemCaseSensitive(keys, "back")->valuestring;
	QUIT_ON_WRONG_KEY = cJSON_GetObjectItemCaseSensitive(keys, "quit_on_wrong")->valueint;
	ABORT_KEY = cJSON_GetObjectItemCaseSensitive(keys, "abort_key")->valuestring;

	free(keys);
	free(window);
}

gboolean eval_key(GPtrArray *keys_arr) {
	if (exec_json(keys_arr) == 0) {
		return FALSE;
	} else {
		return TRUE;
	}
}

//Convert GPtrArray to string
char * string_from_keys_arr(GPtrArray *keys_arr) { 
	size_t len = 1; //Make space for the null terminator
	for (unsigned int i = 0; i < keys_arr->len; i++) {
		char *key = (char *)g_ptr_array_index(keys_arr, i);
		len += strlen(key);
	}

	char *buffer = malloc(len);

	buffer[0] = '\0';
	for (unsigned int i = 0; i < keys_arr->len; i++) {
		char *key = (char *)g_ptr_array_index(keys_arr, i);
		strcat(buffer, key);
	}

	printf("%s\n", buffer);
	return buffer;
}

void hide_unallowed(cJSON* allowed_keys, GtkWidget** box_labels, GtkWidget** keys_labels, int num_of_keys) {
	if (!allowed_keys)
		return;
	cJSON *key = allowed_keys->child;
	while (key) {
		char* key_value = key->string;
		printf("/%s", key_value);

		for (int i = 0; i < num_of_keys; i++) {
			//gtk_widget_hide(box_labels[i]);
			//gtk_widget_set_visible(box_labels[i], FALSE);
			if (strcmp(key_value, gtk_label_get_text(GTK_LABEL(keys_labels[i]))) == 0)
				//gtk_widget_show(box_labels[i]);
				gtk_widget_set_visible(box_labels[i], TRUE);
		}



		key = key->next;
		free(key_value);
	}
}

/*
gboolean after_start(gpointer user_data) {
	printf("hi");
	return G_SOURCE_REMOVE;

}
*/

gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
	(void) widget;
	AppData *data = (AppData *)user_data;

	GPtrArray *keys_arr = data->keys_arr;
	
	//Placeholder, OK?
	KeyBoard keyboard;
	read_keyboard(&keyboard);
	cJSON* json = fill_keyboard(keys_arr, &keyboard);
	printf("\n");

	//printf("\\%s\\",keyboard.keyrows[4].keys[8].key);
	

	//gtk_label_set_text(GTK_LABEL(data->word_labels[0]), "PFW"); //This line emits a warning
	//Make grid...
	for (int row = 0; row < keyboard.height; row++) {
		for (int key = 0; key < keyboard.keyrows[row].length; key++) {
			//printf(" %s ",keyboard.keyrows[row].keys[key].key);


		}
	}

	const gchar *keyname = gdk_keyval_name(event->keyval);
	g_free(pressed_key);
	pressed_key = g_strdup(keyname);

	if (strcmp(pressed_key, ABORT_KEY) == 0)
		gtk_main_quit();

	
	//If pressed key is backspace, remove the last pressed key
	if (strcmp(pressed_key, BACK_KEY) == 0) {
		//Remove last pressed key unless no elements in array
		if (keys_arr->len > 0) {
			g_ptr_array_remove_index(keys_arr, keys_arr->len - 1);
		} else {
			//gtk_main_quit();
			if (QUIT_ON_WRONG_KEY == 1) {
				gtk_main_quit();
			}
		}
	} else { //Else, add pressed key to list. Key is not BACK_KEY
		//Not if non-relevant key
		g_ptr_array_add(keys_arr, g_strdup(keyname));
	}

	//Checks if the key is accepted
	int res = eval_key(keys_arr);
	if (res == 1) {
		if (QUIT_ON_WRONG_KEY == 1)
			gtk_main_quit();
		if (QUIT_ON_WRONG_KEY == 0) {
			//Remove just added element, this funcon call has led to nothing
			g_ptr_array_remove_index(keys_arr, keys_arr->len-1);
			//return TRUE; //Skip updating the label
		}
	}

	GPtrArray* fixxed = presuffix_keys_arr(keys_arr);
	char * keys_so_far_string = string_from_keys_arr(fixxed);
	gtk_label_set_text(data->title_label, keys_so_far_string);
	free(keys_so_far_string);

	//g_print("Key: %s\n", keyname);
	
	hide_unallowed(json, data->box_labels, data->keys_labels, data->num_of_keys);

	return TRUE;
}

int main(int argc, char **argv) {
	init_do_bash();
	init_settings();


	gtk_init(&argc, &argv);
	
	//CSS
	GtkCssProvider *provider = gtk_css_provider_new();
	gtk_css_provider_load_from_path(provider, "style.css", NULL);
	//
	GdkDisplay *display = gdk_display_get_default();
	GdkScreen *screen = gdk_display_get_default_screen(display);
	gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);


	GPtrArray *keys_arr = g_ptr_array_new_with_free_func(g_free);

	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "I hope no one sees this.");
	gtk_window_set_default_size(GTK_WINDOW(window), 300, 150);
	gtk_widget_set_size_request(window, WIDTH, HEIGHT);

	gtk_layer_init_for_window(GTK_WINDOW(window));
	gtk_layer_set_layer(GTK_WINDOW(window), GTK_LAYER_SHELL_LAYER_TOP);
	gtk_layer_set_keyboard_interactivity(GTK_WINDOW(window), TRUE);
	gtk_layer_set_exclusive_zone(GTK_WINDOW(window), 0);
	gtk_layer_set_margin(GTK_WINDOW(window), GTK_LAYER_SHELL_EDGE_TOP, MARGIN);
	if (CENTER == 0) {
		gtk_layer_set_anchor(GTK_WINDOW(window),
							 GTK_LAYER_SHELL_EDGE_TOP |
							 GTK_LAYER_SHELL_EDGE_LEFT,
							 TRUE);
	}


	gtk_widget_add_events(window, GDK_KEY_PRESS_MASK);


	//Needs to be created now for the struct
	GtkWidget *title_label = gtk_label_new("");
	gtk_style_context_add_class(gtk_widget_get_style_context(title_label), "title");


	//CSS for background
	gtk_style_context_add_class(gtk_widget_get_style_context(window), "screen");



	//Start of something
	GtkWidget *grid = gtk_grid_new();

	//Make labels for keys
	GtkWidget *keys_labels[MAX_KEYS];
	GtkWidget *box_labels[MAX_KEYS];

	GtkWidget *name_labels[MAX_KEYS];
	GtkWidget *word_labels[MAX_KEYS];

	KeyBoard keyboarid;
	read_keyboard(&keyboarid);

	int i = 0;
	for (int row = 0; row < keyboarid.height; row++) {
		int offset = 0;
			for (int col = 0; col < keyboarid.keyrows[row].length; col++) {
				box_labels[i] = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
				gtk_style_context_add_class(gtk_widget_get_style_context(box_labels[i]), "key");
				
				keys_labels[i] = gtk_label_new(keyboarid.keyrows[row].keys[col].key);
				gtk_box_pack_start(GTK_BOX(box_labels[i]), keys_labels[i], TRUE, TRUE, 0);
				gtk_style_context_add_class(gtk_widget_get_style_context(keys_labels[i]), "letter");
				name_labels[i] = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
				gtk_box_pack_start(GTK_BOX(box_labels[i]), name_labels[i], TRUE, TRUE, 0);

				word_labels[i] = gtk_label_new(keyboarid.keyrows[row].keys[col].name);
				gtk_box_pack_start(GTK_BOX(name_labels[i]), word_labels[i], TRUE, TRUE, 0);
				gtk_style_context_add_class(gtk_widget_get_style_context(word_labels[i]), "word");

				int keywidth = keyboarid.keyrows[row].keys[col].size;
				gtk_grid_attach(GTK_GRID(grid), box_labels[i], col + offset, row, keywidth, 1);
				offset += keywidth - 1;
				gtk_widget_set_hexpand(box_labels[i], TRUE);
				i++;
				
				if (i == 14) {
					//gtk_widget_hide(box_labels[i]);
					//gtk_widget_hide(keys_labels[i]);
					//gtk_widget_hide(name_labels[i]);
					//gtk_widget_hide(word_labels[i]);
				}
		}
	}

	AppData data;
	data.title_label = GTK_LABEL(title_label);
	data.keys_arr = keys_arr;
	data.box_labels = box_labels;
	data.name_labels = name_labels;
	data.word_labels = word_labels;
	data.keys_labels = keys_labels;
	data.num_of_keys = i;
	g_signal_connect(window, "key-press-event", G_CALLBACK(on_key_press), &data);


	GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	gtk_box_pack_start(GTK_BOX(box), title_label, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box), grid, TRUE, TRUE, 0);

	gtk_container_add(GTK_CONTAINER(window), box);


	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

	//g_timeout_add(0, after_start, NULL);

	gtk_widget_show_all(window);

	/*
	cJSON* json = fill_keyboard(keys_arr, &keyboarid);
	hide_unallowed(json, box_labels, keys_labels, i);
	*/

	gtk_main();

	for (guint i = 0; i < keys_arr->len; i++) {
		printf("Key %u: %s\n", i, (char *)g_ptr_array_index(keys_arr, i));
	}
	g_ptr_array_free(keys_arr, TRUE);

	//printf("Key: %s", pressed_key);
	 
	return 0;

}

