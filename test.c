#include <gtk/gtk.h>
#include <gtk-layer-shell.h>

int main(int argc, char **argv) {
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Safe GTK Layer Shell");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 50);

    gtk_layer_init_for_window(GTK_WINDOW(window));
    gtk_layer_set_layer(GTK_WINDOW(window), GTK_LAYER_SHELL_LAYER_TOP);
    gtk_layer_set_keyboard_interactivity(GTK_WINDOW(window), FALSE);
    gtk_layer_set_exclusive_zone(GTK_WINDOW(window), 0);
    gtk_layer_set_anchor(GTK_WINDOW(window),
                         GTK_LAYER_SHELL_EDGE_TOP |
                         GTK_LAYER_SHELL_EDGE_LEFT |
                         GTK_LAYER_SHELL_EDGE_RIGHT,
                         TRUE);

    GtkWidget *button = gtk_button_new_with_label("Hello, world!");
    g_signal_connect(button, "clicked", G_CALLBACK(gtk_main_quit), NULL);
    gtk_container_add(GTK_CONTAINER(window), button);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}

