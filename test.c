#include <gtk/gtk.h>
#include <gtk-layer-shell.h>
#include "do_bash.h"
#include <string.h>

char *pressed_key = NULL;

typedef struct {
	GtkLabel *title_label;
	GPtrArray *keys_arr;
} AppData;

gboolean eval_key(const char *keyname, GPtrArray *keys_arr) {
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
	g_ptr_array_add(keys_arr, g_strdup(keyname));
	gtk_label_set_text(data->title_label, string_from_keys_arr(keys_arr));

	//g_print("Key: %s\n", keyname);

	if (eval_key(keyname, keys_arr)) {
		gtk_main_quit();
	}

	return TRUE;
}

int main(int argc, char **argv) {
    gtk_init(&argc, &argv);

	GPtrArray *keys_arr = g_ptr_array_new_with_free_func(g_free);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "GTK Layer Shell Popup");
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
    GtkWidget *title_label = gtk_label_new("Hi");

	AppData data;
	data.title_label = GTK_LABEL(title_label);
	data.keys_arr = keys_arr;



    g_signal_connect(window, "key-press-event", G_CALLBACK(on_key_press), &data);

	//Start of something
	GtkWidget *grid = gtk_grid_new();

	GtkWidget *label1 = gtk_label_new("Hello");
	GtkWidget *label2 = gtk_label_new("Hello");
	GtkWidget *label3 = gtk_label_new("Hi");
	GtkWidget *label4 = gtk_label_new("Hello");
	gtk_grid_attach(GTK_GRID(grid), label1, 0, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), label2, 0, 1, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), label3, 1, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), label4, 1, 1, 1, 1);
	
	GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_box_pack_start(GTK_BOX(box), title_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), grid, TRUE, TRUE, 0);

	gtk_container_add(GTK_CONTAINER(window), box);


	//End of something
	
	//CSS
	GtkCssProvider *css_provider = gtk_css_provider_new();
	GdkScreen *screen = gdk_screen_get_default();
	gtk_css_provider_load_from_file(css_provider, g_file_new_for_path("style.css"), NULL);

	GtkStyleContext *window_context = gtk_widget_get_style_context(window);
	gtk_style_context_add_provider(window_context, GTK_STYLE_PROVIDER(css_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
	GtkStyleContext *title_context = gtk_widget_get_style_context(title_label);
	gtk_style_context_add_provider(title_context, GTK_STYLE_PROVIDER(css_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

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

