#include <gtk/gtk.h>

// Función para manejar el clic en el botón "Play"
void on_btn_play_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *player_window = (GtkWidget *)data;
    gtk_widget_show_all(player_window); // Muestra la ventana de selección
}

// Función para manejar el clic en el botón "Back"
void on_btn_back_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *main_window = (GtkWidget *)data;
    gtk_widget_hide(gtk_widget_get_toplevel(widget)); // Oculta la ventana actual
    gtk_widget_show_all(main_window); // Muestra la ventana principal
}

int main(int argc, char *argv[]) {
    GtkBuilder *builder;
    GtkWidget *main_window, *player_window;
    GtkWidget *btn_play, *btn_back;
    GError *error = NULL;

    gtk_init(&argc, &argv);

    builder = gtk_builder_new();
    if (!gtk_builder_add_from_file(builder, "glade_proyecto.glade", &error)) {
        g_printerr("Error cargando archivo Glade: %s\n", error->message);
        g_clear_error(&error);
        return 1;
    }

    // Obtener widgets
    main_window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    player_window = GTK_WIDGET(gtk_builder_get_object(builder, "player_window"));

    // Botones
    btn_play = GTK_WIDGET(gtk_builder_get_object(builder, "btn_play"));
    btn_back = GTK_WIDGET(gtk_builder_get_object(builder, "btn_back"));

    // Conectar señales
    g_signal_connect(btn_play, "clicked", G_CALLBACK(on_btn_play_clicked), player_window);
    g_signal_connect(btn_back, "clicked", G_CALLBACK(on_btn_back_clicked), main_window);

    gtk_widget_show_all(main_window); // Muestra la ventana principal
    gtk_main();

    return 0;
}
