#include <gtk/gtk.h>
#include <gtk-layer-shell.h>
#include "do_bash.h"

gboolean call_bash(gpointer data) {
    gchar *key = (gchar *)data;
    
    run_from_input(key);
	
    //g_free(key);
    gtk_main_quit();
    return G_SOURCE_REMOVE;
}

gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
    const gchar *keyname = gdk_keyval_name(event->keyval);
    g_print("%s\n", keyname, event->keyval);
    //run_from_input(keyname);
    g_idle_add(call_bash, (gpointer)keyname);
    return TRUE;
}

int main(int argc, char **argv) {
    gtk_init(&argc, &argv);

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

    g_signal_connect(window, "key-press-event", G_CALLBACK(on_key_press), NULL);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(window), box);

    GtkWidget *label = gtk_label_new("Firefox\nKitty");
    gtk_box_pack_start(GTK_BOX(box), label, TRUE, TRUE, 0);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);
    gtk_main();

	 

	print_json("k");


    return 0;

}

