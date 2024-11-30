#include <gtk/gtk.h>

// Función para manejar el clic en el botón "Play"
void on_btn_play_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *player_window = (GtkWidget *)data;
    if (GTK_IS_WINDOW(player_window)) {
        gtk_widget_show_all(player_window); // Mostrar la ventana de selección de jugadores
        g_print("Ventana 'player_window' mostrada correctamente.\n");
    } else {
        g_printerr("Error: 'player_window' no es un GtkWindow.\n");
    }
}

// Función para manejar el clic en el botón "Back"
void on_btn_back_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *main_window = (GtkWidget *)data;
    if (GTK_IS_WINDOW(main_window)) {
        gtk_widget_hide(gtk_widget_get_toplevel(widget)); // Ocultar player_window
        gtk_widget_show_all(main_window); // Mostrar la ventana principal
        g_print("Ventana principal mostrada correctamente.\n");
    } else {
        g_printerr("Error: 'main_window' no es un GtkWindow.\n");
    }
}

// Función para manejar el clic en el botón "Exit"
void on_btn_exit_clicked(GtkWidget *widget, gpointer data) {
    g_print("Saliendo del juego.\n");
    gtk_main_quit(); // Termina el bucle principal y cierra la aplicación
}

int main(int argc, char *argv[]) {
    GtkBuilder *builder;
    GtkWidget *main_window, *player_window;
    GtkWidget *btn_play, *btn_back, *btn_exit;
    GError *error = NULL;

    g_print("Iniciando la aplicación.\n");
    gtk_init(&argc, &argv);

    // Cargar archivo Glade
    builder = gtk_builder_new();
    if (!gtk_builder_add_from_file(builder, "glade_proyecto.glade", &error)) {
        g_printerr("Error cargando archivo Glade: %s\n", error->message);
        g_clear_error(&error);
        return 1;
    }

    // Obtener widgets
    main_window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    player_window = GTK_WIDGET(gtk_builder_get_object(builder, "player_window"));
    btn_play = GTK_WIDGET(gtk_builder_get_object(builder, "btn_play"));
    btn_back = GTK_WIDGET(gtk_builder_get_object(builder, "btn_back"));
    btn_exit = GTK_WIDGET(gtk_builder_get_object(builder, "btn_exit"));

    // Manejar el evento 'delete-event' para evitar destruir player_window
    g_signal_connect(player_window, "delete-event", G_CALLBACK(gtk_widget_hide_on_delete), NULL);

    // Conectar señales
    g_signal_connect(btn_play, "clicked", G_CALLBACK(on_btn_play_clicked), player_window);
    g_signal_connect(btn_back, "clicked", G_CALLBACK(on_btn_back_clicked), main_window);
    g_signal_connect(btn_exit, "clicked", G_CALLBACK(on_btn_exit_clicked), NULL);

    // Mostrar ventana principal
    gtk_widget_show_all(main_window);
    gtk_window_present(GTK_WINDOW(main_window)); // Forzar el foco

    gtk_main();

    return 0;
}
