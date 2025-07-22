#include <gtk/gtk.h>
#include <gtk-layer-shell.h>
#include "do_bash.h"
#include <string.h>

#define BACK_KEY "BackSpace"
#define NUM_OF_KEYS 16

char *pressed_key = NULL;

typedef struct {
	GtkLabel *title_label;
	GPtrArray *keys_arr;
} AppData;

gboolean eval_key(GPtrArray *keys_arr) {
	if (print_json(keys_arr) == 0) {
		return FALSE;
	} else {
		return TRUE;
	}
}

//Convert GPtrArray to string
char * string_from_keys_arr(GPtrArray *keys_arr) { 
	size_t len = 1; //Make space for the null terminator
	for (uint i = 0; i < keys_arr->len; i++) {
		char *key = (char *)g_ptr_array_index(keys_arr, i);
		len += strlen(key);
	}

	char *buffer = malloc(len);

	buffer[0] = '\0';
	for (uint i = 0; i < keys_arr->len; i++) {
		char *key = (char *)g_ptr_array_index(keys_arr, i);
		strcat(buffer, key);
	}

	printf("%s\n", buffer);
	return buffer;
}

gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
	(void) widget;
	AppData *data = (AppData *)user_data;

	GPtrArray *keys_arr = data->keys_arr;

    const gchar *keyname = gdk_keyval_name(event->keyval);
	g_free(pressed_key);
	pressed_key = g_strdup(keyname);
	
	//If pressed key is backspace, remove the last pressed key
	if (strcmp(pressed_key, BACK_KEY) == 0) {
		//Remove last pressed key unless no elements in array
		if (keys_arr->len > 0) {
			g_ptr_array_remove_index(keys_arr, keys_arr->len - 1);
		} else {
			gtk_main_quit();
		}
	} else { //Else, add pressed key to list
		g_ptr_array_add(keys_arr, g_strdup(keyname));
	}

	GPtrArray* fixxed = presuffix_keys_arr(keys_arr);
	char * keys_so_far_string = string_from_keys_arr(fixxed);
	gtk_label_set_text(data->title_label, keys_so_far_string);

	//g_print("Key: %s\n", keyname);

	if (eval_key(keys_arr)) {
		gtk_main_quit();
	}

	return TRUE;
}

int main(int argc, char **argv) {

	read_keyboard();

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
    gtk_widget_set_size_request(window, 1500, 700);

    gtk_layer_init_for_window(GTK_WINDOW(window));
    gtk_layer_set_layer(GTK_WINDOW(window), GTK_LAYER_SHELL_LAYER_TOP);
    gtk_layer_set_keyboard_interactivity(GTK_WINDOW(window), TRUE);
    gtk_layer_set_exclusive_zone(GTK_WINDOW(window), 0);
    gtk_layer_set_anchor(GTK_WINDOW(window),
                         GTK_LAYER_SHELL_EDGE_TOP |
                         GTK_LAYER_SHELL_EDGE_LEFT,
                         TRUE);
    gtk_layer_set_margin(GTK_WINDOW(window), GTK_LAYER_SHELL_EDGE_TOP, 200);

    gtk_widget_add_events(window, GDK_KEY_PRESS_MASK);


	//Needs to be created now for the struct
    GtkWidget *title_label = gtk_label_new("");
	gtk_style_context_add_class(gtk_widget_get_style_context(title_label), "title");

	AppData data;
	data.title_label = GTK_LABEL(title_label);
	data.keys_arr = keys_arr;



    g_signal_connect(window, "key-press-event", G_CALLBACK(on_key_press), &data);


	//Start of something
	GtkWidget *grid = gtk_grid_new();

	//Make labels for keys
	GtkWidget *keys_labels[NUM_OF_KEYS];
	GtkWidget *box_labels[NUM_OF_KEYS];
	GtkWidget *app_labels[NUM_OF_KEYS];

	GtkWidget *name_labels[NUM_OF_KEYS];
	GtkWidget *word_labels[NUM_OF_KEYS];
	for (int i = 0; i < NUM_OF_KEYS; i++) {
		/*
		//Make box
		box_labels[i] = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

		//Make label
		keys_labels[i] = gtk_label_new("mjau");
		//app_labels[i] = gtk_label_new("voff");
		app_labels[i] = gtk_image_new_from_icon_name("gvim", GTK_ICON_SIZE_BUTTON);

		//Put label in box
		gtk_box_pack_start(GTK_BOX(box_labels[i]), keys_labels[i], TRUE, TRUE, 0);
		gtk_box_pack_start(GTK_BOX(box_labels[i]), app_labels[i], TRUE, TRUE, 0);

		//Attach box to grid
		gtk_grid_attach(GTK_GRID(grid), box_labels[i], i/4, i%4, 1, 1);

		//Apply CSS styles
		gtk_style_context_add_class(gtk_widget_get_style_context(box_labels[i]), "key");
		*/

		box_labels[i] = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
		gtk_style_context_add_class(gtk_widget_get_style_context(box_labels[i]), "key");

		app_labels[i] = gtk_image_new_from_icon_name("gvim", GTK_ICON_SIZE_BUTTON);
		gtk_box_pack_start(GTK_BOX(box_labels[i]), app_labels[i], TRUE, TRUE, 0);
		name_labels[i] = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
		gtk_box_pack_start(GTK_BOX(box_labels[i]), name_labels[i], TRUE, TRUE, 0);

		word_labels[i] = gtk_label_new("mjau");
		gtk_box_pack_start(GTK_BOX(name_labels[i]), word_labels[i], TRUE, TRUE, 0);
		gtk_style_context_add_class(gtk_widget_get_style_context(word_labels[i]), "word");
		keys_labels[i] = gtk_label_new("N");
		gtk_box_pack_start(GTK_BOX(name_labels[i]), keys_labels[i], TRUE, TRUE, 0);
		gtk_style_context_add_class(gtk_widget_get_style_context(keys_labels[i]), "letter");

		gtk_grid_attach(GTK_GRID(grid), box_labels[i], i/4, i%4, 1, 1);

	}

	GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_box_pack_start(GTK_BOX(box), title_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), grid, TRUE, TRUE, 0);

	gtk_container_add(GTK_CONTAINER(window), box);


	//End of something
	
	//CSS2

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);


    gtk_widget_show_all(window);
    gtk_main();

	for (guint i = 0; i < keys_arr->len; i++) {
		printf("Key %u: %s\n", i, (char *)g_ptr_array_index(keys_arr, i));
	}
	g_ptr_array_free(keys_arr, TRUE);

	//printf("Key: %s", pressed_key);
	 
    return 0;

}

