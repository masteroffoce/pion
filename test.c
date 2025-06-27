#include <gtk/gtk.h>
#include <gtk-layer-shell.h>
#include "do_bash.h"
#include <string.h>

char *pressed_key = NULL;

gboolean eval_key(const char *keyname, GPtrArray *keys_arr) {
	if (print_json(keys_arr) == 0) {
		return FALSE;
	} else {
		return TRUE;
	}
}

gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
	(void) widget;
	GPtrArray *keys_arr = user_data;

    const gchar *keyname = gdk_keyval_name(event->keyval);
	g_free(pressed_key);
	pressed_key = g_strdup(keyname);

	g_ptr_array_add(keys_arr, g_strdup(keyname));
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
    gtk_widget_set_size_request(window, 300, 150);

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

    g_signal_connect(window, "key-press-event", G_CALLBACK(on_key_press), keys_arr);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(window), box);

    GtkWidget *label = gtk_label_new("Firefox\nKitty");
    gtk_box_pack_start(GTK_BOX(box), label, TRUE, TRUE, 0);

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

