#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <stdlib.h>
#include <time.h>

#define MAX_CARTAS_JUGADOR 7
#define NUM_JUGADORES 4
#define MIN_JUGADORES 2

typedef struct {
    GList *mazo;
    GList *descarte;
    GtkWidget *btn_mazo;
    GtkWidget *btn_descarte;
    GtkWidget *flowboxes[NUM_JUGADORES];
    int jugador_actual;
    int num_jugadores_activos; // Nuevo: Número de jugadores activos
} GameData;

// Prototipos de funciones
void on_card_clicked(GtkWidget *widget, gpointer data);
void on_btn_mazo_clicked(GtkWidget *widget, gpointer data);
void actualizar_cartas_visibles(GameData *game_data);

// Lista de nombres de todas las cartas UNO
const char *cartas[] = {
    // Cartas numéricas
    "0_amarillo.png", "0_azul.png", "0_rojo.png", "0_verde.png",
    "1_amarillo.png", "1_amarillo_2.png", "1_azul.png", "1_azul_2.png",
    "1_rojo.png", "1_rojo_2.png", "1_verde.png", "1_verde_2.png",
    "2_amarillo.png", "2_amarillo_2.png", "2_azul.png", "2_azul_2.png",
    "2_rojo.png", "2_rojo_2.png", "2_verde.png", "2_verde_2.png",
    "3_amarillo.png", "3_amarillo_2.png", "3_azul.png", "3_azul_2.png",
    "3_rojo.png", "3_rojo_2.png", "3_verde.png", "3_verde_2.png",
    "4_amarillo.png", "4_amarillo_2.png", "4_azul.png", "4_azul_2.png",
    "4_rojo.png", "4_rojo_2.png", "4_verde.png", "4_verde_2.png",
    "5_amarillo.png", "5_amarillo_2.png", "5_azul.png", "5_azul_2.png",
    "5_rojo.png", "5_rojo_2.png", "5_verde.png", "5_verde_2.png",
    "6_amarillo.png", "6_amarillo_2.png", "6_azul.png", "6_azul_2.png",
    "6_rojo.png", "6_rojo_2.png", "6_verde.png", "6_verde_2.png",
    "7_amarillo.png", "7_amarillo_2.png", "7_azul.png", "7_azul_2.png",
    "7_rojo.png", "7_rojo_2.png", "7_verde.png", "7_verde_2.png",
    "8_amarillo.png", "8_amarillo_2.png", "8_azul.png", "8_azul_2.png",
    "8_rojo.png", "8_rojo_2.png", "8_verde.png", "8_verde_2.png",
    "9_amarillo.png", "9_amarillo_2.png", "9_azul.png", "9_azul_2.png",
    "9_rojo.png", "9_rojo_2.png", "9_verde.png", "9_verde_2.png",
    // Cartas de acción (+2, reversa, salta)
    "+2_amarillo.png", "+2_amarillo_2.png", "+2_azul.png", "+2_azul_2.png",
    "+2_rojo.png", "+2_rojo_2.png", "+2_verde.png", "+2_verde_2.png",
    "reversa_amarillo.png", "reversa_amarillo_2.png", "reversa_azul.png", "reversa_azul_2.png",
    "reversa_rojo.png", "reversa_rojo_2.png", "reversa_verde.png", "reversa_verde_2.png",
    "salta_amarillo.png", "salta_amarillo_2.png", "salta_azul.png", "salta_azul_2.png",
    "salta_rojo.png", "salta_rojo_2.png", "salta_verde.png", "salta_verde_2.png",
    // Cartas comodines
    "comodin_1.png", "comodin_2.png", "comodin_3.png", "comodin_4.png",
    "+4_comodin_1.png", "+4_comodin_2.png", "+4_comodin_3.png", "+4_comodin_4.png"
};

const int num_cartas = sizeof(cartas) / sizeof(cartas[0]);

// Función para configurar un botón con imagen
void configurar_boton_con_imagen(GtkButton *boton, const char *ruta_imagen, gboolean rotar) {
    gtk_button_set_label(boton, NULL); // Quitar el texto del botón
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(ruta_imagen, NULL);
    if (pixbuf) {
        pixbuf = gdk_pixbuf_scale_simple(pixbuf, 35, 50, GDK_INTERP_BILINEAR); // Tamaño horizontal
        if (rotar) {
            // Rotar la imagen solo si se solicita
            GdkPixbuf *pixbuf_rotada = gdk_pixbuf_rotate_simple(pixbuf, GDK_PIXBUF_ROTATE_CLOCKWISE);
            g_object_unref(pixbuf);
            pixbuf = pixbuf_rotada;
        }
        GtkWidget *imagen = gtk_image_new_from_pixbuf(pixbuf);
        gtk_button_set_image(boton, imagen);
        gtk_button_set_always_show_image(boton, TRUE);
        g_object_unref(pixbuf);
    }
}

// Vaciar las cartas existentes en las flowboxes
void limpiar_flowboxes(GameData *game_data) {
    for (int jugador = 0; jugador < NUM_JUGADORES; jugador++) {
        GList *children = gtk_container_get_children(GTK_CONTAINER(game_data->flowboxes[jugador]));
        for (GList *child = children; child != NULL; child = child->next) {
            gtk_widget_destroy(GTK_WIDGET(child->data));
        }
        g_list_free(children);
    }
}

// Liberar mazo existente
void liberar_mazo(GameData *game_data) {
    if (game_data->mazo) {
        g_list_free_full(game_data->mazo, g_free);
        game_data->mazo = NULL;
    }
}

// Inicializar mazo
void inicializar_mazo(GameData *game_data) {
    liberar_mazo(game_data);
    char ruta_carta[100];
    for (int i = 0; i < num_cartas; i++) {
        snprintf(ruta_carta, sizeof(ruta_carta), "images/%s", cartas[i]);
        game_data->mazo = g_list_append(game_data->mazo, g_strdup(ruta_carta));
    }
}

// Obtener carta del mazo
const char *obtener_carta_del_mazo(GameData *game_data) {
    if (!game_data->mazo) return NULL;
    GList *nodo = g_list_nth(game_data->mazo, rand() % g_list_length(game_data->mazo));
    const char *carta = (const char *)nodo->data;
    game_data->mazo = g_list_remove(game_data->mazo, carta);
    return carta;
}


// Manejo del botón "mazo" (robar carta)
void on_btn_mazo_clicked(GtkWidget *widget, gpointer data) {
    GameData *game_data = (GameData *)data;

    const char *carta = obtener_carta_del_mazo(game_data);
    if (carta) {
        GtkWidget *boton = gtk_button_new();
        configurar_boton_con_imagen(GTK_BUTTON(boton), carta, FALSE);

        // Asocia la ruta de la carta al botón
        g_object_set_data_full(G_OBJECT(boton), "ruta_carta", g_strdup(carta), g_free);
        g_object_set_data(G_OBJECT(boton), "jugador", GINT_TO_POINTER(game_data->jugador_actual));

        // Conectar el evento "clicked" para las cartas nuevas
        g_signal_connect(boton, "clicked", G_CALLBACK(on_card_clicked), game_data);

        // Añadir la carta al flowbox del jugador actual
        gtk_flow_box_insert(GTK_FLOW_BOX(game_data->flowboxes[game_data->jugador_actual]), boton, -1);

        // Actualiza la visibilidad de las cartas
        actualizar_cartas_visibles(game_data);

        gtk_widget_show(boton);
    }
}

void asignar_cartas_iniciales(GameData *game_data, int num_jugadores) {
    limpiar_flowboxes(game_data);

    for (int jugador = 0; jugador < num_jugadores; jugador++) {
        for (int i = 0; i < MAX_CARTAS_JUGADOR; i++) {
            const char *carta = obtener_carta_del_mazo(game_data);
            if (!carta) {
                g_warning("No hay más cartas en el mazo.");
                return;
            }

            GtkWidget *boton = gtk_button_new();
            gtk_widget_set_size_request(boton, 35, 50);

            // Configura la visibilidad según el jugador
            if (jugador == game_data->jugador_actual) {
                configurar_boton_con_imagen(GTK_BUTTON(boton), carta, FALSE);
                gtk_widget_set_sensitive(boton, TRUE); // Habilitar clic
            } else {
                configurar_boton_con_imagen(GTK_BUTTON(boton), "images/uno_card.png", FALSE);
                gtk_widget_set_sensitive(boton, FALSE); // Deshabilitar clic
            }

            // Asocia la ruta de la carta y el jugador al botón
            g_object_set_data_full(G_OBJECT(boton), "ruta_carta", g_strdup(carta), g_free);
            g_object_set_data(G_OBJECT(boton), "jugador", GINT_TO_POINTER(jugador));

            g_signal_connect(boton, "clicked", G_CALLBACK(on_card_clicked), game_data);

            GtkWidget *flowbox_child = gtk_flow_box_child_new();
            gtk_container_add(GTK_CONTAINER(flowbox_child), boton);
            gtk_flow_box_insert(GTK_FLOW_BOX(game_data->flowboxes[jugador]), flowbox_child, -1);
            gtk_widget_show_all(flowbox_child);
        }
    }
}

void on_card_clicked(GtkWidget *widget, gpointer data) {
    GameData *game_data = (GameData *)data;

    int jugador = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(widget), "jugador"));
    if (jugador != game_data->jugador_actual) {
        g_print("No es tu turno. Jugador actual: %d\n", game_data->jugador_actual + 1);
        return;
    }

    const char *ruta_carta = g_object_get_data(G_OBJECT(widget), "ruta_carta");
    if (!ruta_carta) {
        g_warning("Ruta de la carta es NULL en on_card_clicked. Jugador actual: %d", game_data->jugador_actual);
        return;
    }

    // Jugar la carta (mostrar en el descarte)
    configurar_boton_con_imagen(GTK_BUTTON(game_data->btn_descarte), ruta_carta, FALSE);

    // Eliminar la carta jugada del flowbox
    GtkWidget *flowbox_child = gtk_widget_get_parent(widget);
    gtk_widget_destroy(flowbox_child);

    // Avanzar al siguiente jugador
    game_data->jugador_actual = (game_data->jugador_actual + 1) % game_data->num_jugadores_activos;

    // Actualizar la visibilidad de las cartas
    actualizar_cartas_visibles(game_data);

    g_print("Carta descartada: %s. Turno del jugador %d\n", ruta_carta, game_data->jugador_actual + 1);
}

void actualizar_cartas_visibles(GameData *game_data) {
    for (int jugador = 0; jugador < game_data->num_jugadores_activos; jugador++) {
        GList *children = gtk_container_get_children(GTK_CONTAINER(game_data->flowboxes[jugador]));
        for (GList *child = children; child != NULL; child = child->next) {
            GtkWidget *flowbox_child = GTK_WIDGET(child->data);
            GtkWidget *boton = gtk_bin_get_child(GTK_BIN(flowbox_child));

            // Obtén la ruta de la carta asociada al botón
            const char *ruta_carta = g_object_get_data(G_OBJECT(boton), "ruta_carta");

            // Mostrar carta o tapa según el jugador
            if (jugador == game_data->jugador_actual) {
                if (ruta_carta) {
                    configurar_boton_con_imagen(GTK_BUTTON(boton), ruta_carta, FALSE);
                } else {
                    g_warning("Ruta de la carta es NULL para jugador %d.", jugador);
                }
                gtk_widget_set_sensitive(boton, TRUE); // Habilitar clic
            } else {
                configurar_boton_con_imagen(GTK_BUTTON(boton), "images/uno_card.png", FALSE);
                gtk_widget_set_sensitive(boton, FALSE); // Deshabilitar clic
            }
        }
        g_list_free(children);
    }
}

// Función para manejar el clic en el botón "Play"
void on_btn_play_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *player_window = (GtkWidget *)data;
    gtk_widget_show_all(player_window);
}

// Función para manejar el clic en el botón "Back"
void on_btn_back_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *main_window = (GtkWidget *)data;
    gtk_widget_hide(gtk_widget_get_toplevel(widget));
    gtk_widget_show_all(main_window);
}

void reiniciar_juego(GameData *game_data) {
    // Limpia los flowboxes de los jugadores
    limpiar_flowboxes(game_data);

    // Inicializa el mazo
    inicializar_mazo(game_data);

    // Asegura que el descarte tenga una carta inicial válida
    const char *carta_inicial;
    do {
        carta_inicial = obtener_carta_del_mazo(game_data);
    } while (g_str_has_prefix(carta_inicial, "comodin")); // Evita comodines como carta inicial

    configurar_boton_con_imagen(GTK_BUTTON(game_data->btn_descarte), carta_inicial, FALSE);

    // Inicializa las cartas de los jugadores
    asignar_cartas_iniciales(game_data, game_data->num_jugadores_activos);

    // Configura el turno del primer jugador
    game_data->jugador_actual = 0;

    // Actualiza la visibilidad de las cartas
    actualizar_cartas_visibles(game_data);

    g_print("Juego reiniciado. Turno del jugador 1.\n");
}

// Interceptar cierre con botón "X"
gboolean on_window_delete_event(GtkWidget *widget, GdkEvent *event, gpointer data) {
    GameData *game_data = (GameData *)data;

    // Reinicia el juego al cerrar la ventana
    reiniciar_juego(game_data);

    // Esconde la ventana en lugar de destruirla
    gtk_widget_hide(widget);
    return TRUE;
}

// Selección de jugadores
void on_select_players(GtkButton *button, gpointer data) {
    GtkWidget **params = (GtkWidget **)data;
    GtkWidget *game_window = params[0];
    GameData *game_data = (GameData *)params[1];
    const char *label = gtk_button_get_label(GTK_BUTTON(button));
    int num_jugadores = atoi(label);

    game_data->num_jugadores_activos = num_jugadores;

    inicializar_mazo(game_data);
    asignar_cartas_iniciales(game_data, num_jugadores);
    actualizar_cartas_visibles(game_data); // Asegúrate de que las cartas se vean correctamente
    gtk_widget_show_all(game_window);
}

// MAIN

int main(int argc, char *argv[]) {
    GtkBuilder *builder;
    GtkWidget *main_window, *player_window, *game_window;
    GtkWidget *btn_play, *btn_exit, *btn_back;
    GtkWidget *flowboxes[NUM_JUGADORES];
    GtkWidget *btn_select_players[NUM_JUGADORES];
    GtkWidget *btn_mazo, *btn_descarte;

    GameData game_data = {NULL, NULL, NULL, NULL, {NULL}, 0};

    gtk_init(&argc, &argv);

    // Inicializar semilla para números aleatorios
    srand((unsigned int)time(NULL));

    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "glade_proyecto.glade", NULL);

    main_window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    player_window = GTK_WIDGET(gtk_builder_get_object(builder, "player_window"));
    game_window = GTK_WIDGET(gtk_builder_get_object(builder, "game_window"));
    btn_play = GTK_WIDGET(gtk_builder_get_object(builder, "btn_play"));
    btn_exit = GTK_WIDGET(gtk_builder_get_object(builder, "btn_exit"));
    btn_back = GTK_WIDGET(gtk_builder_get_object(builder, "btn_back"));
    btn_mazo = GTK_WIDGET(gtk_builder_get_object(builder, "btn_mazo"));
    btn_descarte = GTK_WIDGET(gtk_builder_get_object(builder, "btn_descarte"));

    game_data.btn_mazo = btn_mazo;
    game_data.btn_descarte = btn_descarte;

    gtk_window_set_default_size(GTK_WINDOW(main_window), 600, 400);
    gtk_window_set_resizable(GTK_WINDOW(main_window), FALSE);
    gtk_window_set_default_size(GTK_WINDOW(game_window), 200, 400);
    gtk_window_set_resizable(GTK_WINDOW(game_window), FALSE);

    // Conectar eventos de cierre de ventana
    g_signal_connect(main_window, "delete-event", G_CALLBACK(on_window_delete_event), &game_data);
    g_signal_connect(player_window, "delete-event", G_CALLBACK(on_window_delete_event), &game_data);
    g_signal_connect(game_window, "delete-event", G_CALLBACK(on_window_delete_event), &game_data);

    for (int i = 0; i < NUM_JUGADORES; i++) {
        char id[30];
        snprintf(id, sizeof(id), "flowbox_jugador_%d", i + 1);
        flowboxes[i] = GTK_WIDGET(gtk_builder_get_object(builder, id));
        game_data.flowboxes[i] = flowboxes[i];
    }

    g_signal_connect(btn_play, "clicked", G_CALLBACK(on_btn_play_clicked), player_window);
    g_signal_connect(btn_exit, "clicked", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(btn_back, "clicked", G_CALLBACK(on_btn_back_clicked), main_window);
    g_signal_connect(btn_mazo, "clicked", G_CALLBACK(on_btn_mazo_clicked), &game_data);

    for (int i = 1; i < NUM_JUGADORES; i++) {
        char id[20];
        snprintf(id, sizeof(id), "btn_%d", i + 1);
        btn_select_players[i] = GTK_WIDGET(gtk_builder_get_object(builder, id));
        GtkWidget *params[] = {game_window, (GtkWidget *)&game_data};
        g_signal_connect(btn_select_players[i], "clicked", G_CALLBACK(on_select_players), params);
    }

    inicializar_mazo(&game_data);
    reiniciar_juego(&game_data); // Reinicia el juego al inicio
    gtk_widget_show_all(main_window);
    gtk_main();

    return 0;
}
