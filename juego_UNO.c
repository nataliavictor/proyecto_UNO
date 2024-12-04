// -------------------------------------------------------------------------------------------------------------------
// Proyecto Final - Platafotmas Abiertas
// Criselda Orozco - B85762 y Natalia Victor - B9843

// Este es un juego interactivo en C con interfaz gráfica creada utilizando la biblioteca GTK-3 y la aplicación Glade.
// El juego se trata de un prototipo del popular juego de mesa UNO. Este implementa las mismas reglas que el juego original.
// Al ejecutarlo aparece una ventana con dos botones, PLAY y EXIT, el último cierra el programa, mientras que 
// el primero abre otra ventana con cuatro botones más, tres de ellos para elegir el número de jugadores, que va de dos a
// cuatro, y el último boton BACK el cual devuelve a la ventana inicial. Una vez seleccionado el número de jugadores aparece
// la ventana de juego, en esta se encuentra el mazo de cartas, el descarte y la repartición de las respectivas siete
// cartas por jugador. Sólo las cartas del jugador actual son las que aparecen vueltas, las cartas de los demás jugadores
// aparecen tapadas.
// ---------------------------------------------------------------------------------------------------------------------

#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <stdlib.h>
#include <time.h>

// Variables macro
#define MAX_CARTAS_JUGADOR 7 
#define NUM_JUGADORES 4
#define MIN_JUGADORES 2

// Definición de la estructura GameData
typedef struct {
    GList *mazo;
    GList *descarte;
    GtkWidget *btn_mazo;
    GtkWidget *btn_descarte;
    GtkWidget *flowboxes[NUM_JUGADORES]; 
    int jugador_actual;
    int num_jugadores_activos;
    GtkWidget *main_window;   // Ventana principal
    GtkWidget *player_window; // Ventana de selección de jugadores
    GtkWidget *game_window;   // Ventana del juego
} GameData;

// Definición de la estructura Carta 
typedef struct {
    const char *ruta_carta;
    char color[10];
    char tipo[10];
} Carta;

Carta carta_actual; // Variable global para almacenar la última carta jugada

// Prototipos de funciones
void on_card_clicked(GtkWidget *widget, gpointer data);
void on_btn_mazo_clicked(GtkWidget *widget, gpointer data);
void actualizar_cartas_visibles(GameData *game_data);
void robar_cartas(GameData *game_data, int num_cartas);
void invertir_orden_juego(GameData *game_data);
void saltar_jugador(GameData *game_data);
void reiniciar_juego(GameData *game_data);
gboolean hay_movimientos_validos(GameData *game_data);
void mostrar_seleccion_color(GameData *game_data);
void elegir_color(GtkWidget *widget, gpointer data);
void manejar_encadenamiento(GameData *game_data, const char *tipo_carta);
gboolean hay_carta_encadenable(GameData *game_data);
void avanzar_turno(GameData *game_data);
gboolean es_carta_valida(const char *tipo_carta, const char *color_carta, const Carta *carta_actual);
void mostrar_ventana_una_carta(GameData *game_data);
void reciclar_descarte(GameData *game_data);
void on_felicidades_response(GtkDialog *dialog, gint response_id, gpointer data);
void mostrar_cambio_color(GameData *game_data, const char *color);

// Lista de nombres de todas las cartas UNO en la carpeta imagenes
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

// Inicializar el mazo de cartas con las imagens de la carpeta "imagenes"
void inicializar_mazo(GameData *game_data) {
    liberar_mazo(game_data);
    char ruta_carta[100];
    for (int i = 0; i < num_cartas; i++) {
        snprintf(ruta_carta, sizeof(ruta_carta), "images/%s", cartas[i]);
        game_data->mazo = g_list_append(game_data->mazo, g_strdup(ruta_carta)); //Ruta
    }
}

// Obtener una carta aleatoria del mazo. Si el mazo está vacío, intenta reciclar las cartas del descarte.
const char *obtener_carta_del_mazo(GameData *game_data) {
    if (!game_data->mazo) {
        reciclar_descarte(game_data);

        if (!game_data->mazo) {
            return NULL; // No hay cartas disponibles
        }
    }

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
        
        g_object_set_data_full(G_OBJECT(boton), "ruta_carta", g_strdup(carta), g_free); // Asocia la ruta de la carta al botón
        g_object_set_data(G_OBJECT(boton), "jugador", GINT_TO_POINTER(game_data->jugador_actual));

        g_signal_connect(boton, "clicked", G_CALLBACK(on_card_clicked), game_data);
        gtk_flow_box_insert(GTK_FLOW_BOX(game_data->flowboxes[game_data->jugador_actual]), boton, -1);
        actualizar_cartas_visibles(game_data);

        gtk_widget_show(boton);
    }
}

// Asignar las cartas iniciales a cada jugador al comienzo del juego
void asignar_cartas_iniciales(GameData *game_data, int num_jugadores) {
    if (num_jugadores < MIN_JUGADORES) {
        return;
    }
    limpiar_flowboxes(game_data);
    for (int jugador = 0; jugador < num_jugadores; jugador++) {
        for (int i = 0; i < MAX_CARTAS_JUGADOR; i++) {
            const char *carta = obtener_carta_del_mazo(game_data);
            if (!carta) {
                return;
            }

            GtkWidget *boton = gtk_button_new();
            gtk_widget_set_size_request(boton, 35, 50);

            if (jugador == game_data->jugador_actual) {
                configurar_boton_con_imagen(GTK_BUTTON(boton), carta, FALSE);
                gtk_widget_set_sensitive(boton, TRUE);
            } else {
                configurar_boton_con_imagen(GTK_BUTTON(boton), "images/uno_card.png", FALSE);
                gtk_widget_set_sensitive(boton, FALSE);
            }

            g_object_set_data_full(G_OBJECT(boton), "ruta_carta", g_strdup(carta), g_free);
            g_object_set_data(G_OBJECT(boton), "jugador", GINT_TO_POINTER(jugador));

            g_signal_connect(boton, "clicked", G_CALLBACK(on_card_clicked), game_data);

            GtkWidget *flowbox_child = gtk_flow_box_child_new();
            gtk_container_add(GTK_CONTAINER(flowbox_child), boton);
            gtk_flow_box_insert(GTK_FLOW_BOX(game_data->flowboxes[jugador]), flowbox_child, -1);
            gtk_widget_show_all(flowbox_child);
        }
    }
    const char *carta_inicial; // Seleccionar una carta inicial válida
    char tipo_inicial[10], color_inicial[10];
    do {
        carta_inicial = obtener_carta_del_mazo(game_data);
        if (!carta_inicial) {
            return;
        }
        sscanf(carta_inicial, "images/%[^_]_%[^_.]", tipo_inicial, color_inicial);
    } while (strcmp(tipo_inicial, "+4") == 0); // Evitar que sea un +4

    configurar_boton_con_imagen(GTK_BUTTON(game_data->btn_descarte), carta_inicial, FALSE);

    strncpy(carta_actual.color, color_inicial, sizeof(carta_actual.color));
    strncpy(carta_actual.tipo, tipo_inicial, sizeof(carta_actual.tipo));
    carta_actual.ruta_carta = carta_inicial;

    // En caso de que la carta inicial sea especial
    if (strcmp(tipo_inicial, "+2") == 0) {
        robar_cartas(game_data, 2);
    } else if (strcmp(tipo_inicial, "reversa") == 0) {
        invertir_orden_juego(game_data);
    } else if (strcmp(tipo_inicial, "salta") == 0) {
        avanzar_turno(game_data); 
    }
}

// Muestrar un diálogo informativo cuando el color del juego ha cambiado.
void mostrar_cambio_color(GameData *game_data, const char *color) {
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(game_data->game_window),
                                               GTK_DIALOG_DESTROY_WITH_PARENT,
                                               GTK_MESSAGE_INFO,
                                               GTK_BUTTONS_OK,
                                               "El color ha cambiado a: %s",
                                               color);

    // Conectar el botón de OK para cerrar el diálogo
    g_signal_connect(dialog, "response", G_CALLBACK(gtk_widget_destroy), NULL);
    gtk_widget_show(dialog);
}

// Verificar si el jugador actual ha ganado el juego
gboolean verificar_ganador(GameData *game_data) {
    GList *children = gtk_container_get_children(GTK_CONTAINER(game_data->flowboxes[game_data->jugador_actual]));
    gboolean sin_cartas = (children == NULL); // Si no hay hijos, el jugador no tiene cartas
    g_list_free(children);

    if (sin_cartas) {
        // Cierra las ventanas del juego y selección de jugadores
        gtk_widget_hide(game_data->game_window); // Cierra la ventana del juego
        gtk_widget_hide(game_data->player_window); // Cierra la ventana de selección de jugadores

        // Muestra el mensaje de felicitaciones
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(game_data->main_window),
                                                   GTK_DIALOG_DESTROY_WITH_PARENT,
                                                   GTK_MESSAGE_INFO,
                                                   GTK_BUTTONS_OK,
                                                   "¡Felicidades! El jugador %d ha ganado el juego.",
                                                   game_data->jugador_actual + 1);

        g_signal_connect(dialog, "response", G_CALLBACK(on_felicidades_response), game_data);
        gtk_widget_show(dialog);
    }
    return sin_cartas;
}

// Maneja la respuesta del cuadro de diálogo de felicitaciones cuando el jugador gana
void on_felicidades_response(GtkDialog *dialog, gint response_id, gpointer data) {
    GameData *game_data = (GameData *)data;
    gtk_widget_destroy(GTK_WIDGET(dialog)); // Cierra el cuadro de diálogo
    gtk_widget_show_all(game_data->main_window);// Muestra la ventana principal
}

// Maneja el clic en una carta, valida la jugada, actualiza el estado del juego y ejecuta acciones especiales
void on_card_clicked(GtkWidget *widget, gpointer data) {
    GameData *game_data = (GameData *)data;
    const char *ruta_carta = g_object_get_data(G_OBJECT(widget), "ruta_carta");
    if (!ruta_carta) {
        return;
    }
    char color_jugado[10], tipo_jugado[10];
    sscanf(ruta_carta, "images/%[^_]_%[^_.]", tipo_jugado, color_jugado);// Extraer color y tipo de la carta (ignorar sufijos como "_2", "_3")

    if (!es_carta_valida(tipo_jugado, color_jugado, &carta_actual)) { // Validar si la carta es jugable
        return;
    }

    strncpy(carta_actual.color, strcmp(tipo_jugado, "comodin") == 0 ? "comodin" : color_jugado, sizeof(carta_actual.color));
    strncpy(carta_actual.tipo, tipo_jugado, sizeof(carta_actual.tipo));
    carta_actual.ruta_carta = ruta_carta;// Actualizar el estado de la carta actual

    configurar_boton_con_imagen(GTK_BUTTON(game_data->btn_descarte), ruta_carta, FALSE); // Mostrar la carta en el descarte
    GtkWidget *flowbox_child = gtk_widget_get_parent(widget);
    gtk_widget_destroy(flowbox_child);

    GList *children = gtk_container_get_children(GTK_CONTAINER(game_data->flowboxes[game_data->jugador_actual]));
    if (g_list_length(children) == 1) {
        mostrar_ventana_una_carta(game_data);
    }
    g_list_free(children);

    if (verificar_ganador(game_data)) { // Verifica si el jugador ha ganado
        return; 
    }

    // Manejar acciones de las cartas especiales
    if (strcmp(tipo_jugado, "+2") == 0 || strcmp(tipo_jugado, "+4") == 0) {
        manejar_encadenamiento(game_data, tipo_jugado);
        return;
    } else if (strcmp(tipo_jugado, "comodin") == 0) {
        mostrar_seleccion_color(game_data);
        return;
    } else if (strcmp(tipo_jugado, "reversa") == 0) {
        invertir_orden_juego(game_data);
    } else if (strcmp(tipo_jugado, "salta") == 0) {
        saltar_jugador(game_data);
        return;
    }

    game_data->descarte = g_list_append(game_data->descarte, g_strdup(ruta_carta));
    avanzar_turno(game_data);
}

// Verifica si una carta jugada es válida según el tipo y color de la carta actual
gboolean es_carta_valida(const char *tipo_carta, const char *color_carta, const Carta *carta_actual) {
    char tipo_jugado[10], color_jugado[10];
    sscanf(tipo_carta, "%[^_]", tipo_jugado);
    sscanf(color_carta, "%[^_]", color_jugado);
    if (strcmp(tipo_jugado, "comodin") == 0 || strcmp(tipo_jugado, "+4") == 0) { // Siempre se puede jugar un comodín o +4
        return TRUE; 
    }
    if (strcmp(color_jugado, carta_actual->color) == 0 || strcmp(tipo_jugado, carta_actual->tipo) == 0) {
        return TRUE;
    }
    return FALSE;
}

// Permite al jugador actual robar un número específico de cartas del mazo
void robar_cartas(GameData *game_data, int num_cartas) {
    for (int i = 0; i < num_cartas; i++) {
        const char *carta = obtener_carta_del_mazo(game_data);
        if (!carta) {
            return;
        }

        GtkWidget *boton = gtk_button_new();
        configurar_boton_con_imagen(GTK_BUTTON(boton), carta, FALSE);

        g_object_set_data_full(G_OBJECT(boton), "ruta_carta", g_strdup(carta), g_free);
        gtk_flow_box_insert(GTK_FLOW_BOX(game_data->flowboxes[game_data->jugador_actual]), boton, -1);

        g_signal_connect(boton, "clicked", G_CALLBACK(on_card_clicked), game_data);
        gtk_widget_show(boton);

        GList *children = gtk_container_get_children(GTK_CONTAINER(game_data->flowboxes[game_data->jugador_actual]));
        if (g_list_length(children) == 1) {
            mostrar_ventana_una_carta(game_data);
        }
        g_list_free(children);

        if (verificar_ganador(game_data)) {
            return; // Detener el flujo si el juego se reinicia
        }
    }
    actualizar_cartas_visibles(game_data);
}

// Mostrar un cuadro de diálogo que notifica cuando un jugador tiene solo una carta
void mostrar_ventana_una_carta(GameData *game_data) {
    GtkWidget *dialog = gtk_message_dialog_new(NULL,
                                               GTK_DIALOG_DESTROY_WITH_PARENT,
                                               GTK_MESSAGE_WARNING,
                                               GTK_BUTTONS_OK,
                                               "Jugador %d tiene UNA carta.",
                                               game_data->jugador_actual + 1);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

static gboolean orden_invertido = FALSE;

// Invertir el orden de los turnos de los jugadores
void invertir_orden_juego(GameData *game_data) {
    orden_invertido = !orden_invertido; // Invertir el orden
}

// Avanza al siguiente turno
void avanzar_turno(GameData *game_data) {
    if (orden_invertido) {
        game_data->jugador_actual = (game_data->jugador_actual - 1 + game_data->num_jugadores_activos) % game_data->num_jugadores_activos;
    } else {
        game_data->jugador_actual = (game_data->jugador_actual + 1) % game_data->num_jugadores_activos;
    }
    actualizar_cartas_visibles(game_data);
}

// Salta el turno del siguiente jugador, avanzando dos turnos si hay más de un jugador
void saltar_jugador(GameData *game_data) {
    if (game_data->num_jugadores_activos > 1) {
        avanzar_turno(game_data); // Avanzar una vez para saltar
        avanzar_turno(game_data); // Avanzar nuevamente al siguiente jugador
    } else {
        g_warning("No hay suficientes jugadores para saltar.");
    }
}

// Manejar el encadenamiento de cartas especiales como "+2" o "+4" 
void manejar_encadenamiento(GameData *game_data, const char *tipo_carta) {
    static int cartas_a_robar = 0;

    // Incrementa las cartas acumuladas según el tipo de carta jugada
    if (strcmp(tipo_carta, "+2") == 0) {
        cartas_a_robar += 2;
    } else if (strcmp(tipo_carta, "+4") == 0) {
        cartas_a_robar += 4;
    }
    avanzar_turno(game_data); // Pasar al siguiente jugador
    // Verificar si el siguiente jugador puede encadenar el mismo tipo de carta
    if (hay_carta_encadenable(game_data)) {
        return; // Esperar a que el jugador decida encadenar
    }

    // Si no puede encadenar, el jugador roba las cartas acumuladas
    robar_cartas(game_data, cartas_a_robar);
    cartas_a_robar = 0; // Reiniciar acumulación

    // Si la carta era un +4, mostrar la selección de color
    if (strcmp(tipo_carta, "+4") == 0) {
        mostrar_seleccion_color(game_data);
        return; // Esperar a que el jugador seleccione un color
    }
    avanzar_turno(game_data);
}

// Reiniciar el juego
void reiniciar_juego(GameData *game_data) {
    limpiar_flowboxes(game_data);
    liberar_mazo(game_data);
    inicializar_mazo(game_data);

    // Seleccionar la primera carta válida (no especial)
    const char *carta_inicial;
    do {
        carta_inicial = obtener_carta_del_mazo(game_data);
    } while (g_str_has_prefix(carta_inicial, "+") || g_str_has_prefix(carta_inicial, "comodin") ||
             g_str_has_prefix(carta_inicial, "reversa") || g_str_has_prefix(carta_inicial, "salta"));

    configurar_boton_con_imagen(GTK_BUTTON(game_data->btn_descarte), carta_inicial, FALSE);

    // Extraer color y tipo de la carta inicial
    sscanf(carta_inicial, "images/%[^_]_%[^.].png", carta_actual.tipo, carta_actual.color);
    carta_actual.ruta_carta = carta_inicial;

    asignar_cartas_iniciales(game_data, game_data->num_jugadores_activos);
    game_data->jugador_actual = 0;

    actualizar_cartas_visibles(game_data);
}

// Actualizar las cartas visibles de todos los jugadores en la interfaz, habilitando solo las cartas del jugador actual
void actualizar_cartas_visibles(GameData *game_data) {
    for (int jugador = 0; jugador < game_data->num_jugadores_activos; jugador++) {
        GList *children = gtk_container_get_children(GTK_CONTAINER(game_data->flowboxes[jugador]));
        for (GList *child = children; child != NULL; child = child->next) {
            GtkWidget *flowbox_child = GTK_WIDGET(child->data);
            GtkWidget *boton = gtk_bin_get_child(GTK_BIN(flowbox_child));

            const char *ruta_carta = g_object_get_data(G_OBJECT(boton), "ruta_carta");

            if (!ruta_carta) {
                continue; // Salta botones nulos
            }
            // Mostrar las cartas solo si es el turno del jugador
            if (jugador == game_data->jugador_actual) {
                configurar_boton_con_imagen(GTK_BUTTON(boton), ruta_carta, FALSE);
                gtk_widget_set_sensitive(boton, TRUE); // Habilitar clic
            } else {
                configurar_boton_con_imagen(GTK_BUTTON(boton), "images/uno_card.png", FALSE);
                gtk_widget_set_sensitive(boton, FALSE); // Deshabilitar clic
            }
        }
        g_list_free(children);
    }
}

// Muestra una ventana de selección de color donde el jugador puede elegir el color para una carta +4.
void mostrar_seleccion_color(GameData *game_data) {
    GtkWidget *color_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(color_window), "Selecciona un color");
    gtk_window_set_default_size(GTK_WINDOW(color_window), 200, 100);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_container_add(GTK_CONTAINER(color_window), box);

    const char *colores[] = {"rojo", "azul", "verde", "amarillo"};
    for (int i = 0; i < 4; i++) {
        GtkWidget *btn_color = gtk_button_new_with_label(colores[i]);

        // Conectar el botón para elegir color
        g_signal_connect(btn_color, "clicked", G_CALLBACK(elegir_color), game_data);
        gtk_box_pack_start(GTK_BOX(box), btn_color, TRUE, TRUE, 5);
    }
    gtk_widget_show_all(color_window);
}

// Función que maneja la selección de color por parte del jugador cuando se juega una carta +4.
void elegir_color(GtkWidget *widget, gpointer data) {
    GameData *game_data = (GameData *)data;
    const char *color_seleccionado = gtk_button_get_label(GTK_BUTTON(widget));

    strncpy(carta_actual.color, color_seleccionado, sizeof(carta_actual.color));
    mostrar_cambio_color(game_data, color_seleccionado); // Mostrar la ventana informando el cambio de color

    gtk_widget_hide(gtk_widget_get_toplevel(widget)); // Cerrar la ventana de selección de color
    avanzar_turno(game_data);
}

// Botón "Play"
void on_btn_play_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *player_window = (GtkWidget *)data;
    gtk_widget_show_all(player_window);
}

// Botón "Back"
void on_btn_back_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *main_window = (GtkWidget *)data;
    gtk_widget_hide(gtk_widget_get_toplevel(widget));
    gtk_widget_show_all(main_window);
}

// Interceptar cierre con botón "X"
gboolean on_window_delete_event(GtkWidget *widget, GdkEvent *event, gpointer data) {
    GameData *game_data = (GameData *)data;
    reiniciar_juego(game_data);
    gtk_widget_hide(widget);
    return TRUE;
}

// Función que verifica si el jugador actual tiene movimientos válidos disponibles para jugar
gboolean hay_movimientos_validos(GameData *game_data) {
    GList *children = gtk_container_get_children(GTK_CONTAINER(game_data->flowboxes[game_data->jugador_actual]));
    for (GList *child = children; child != NULL; child = child->next) {
        GtkWidget *boton = gtk_bin_get_child(GTK_BIN(GTK_WIDGET(child->data)));
        const char *ruta_carta = g_object_get_data(G_OBJECT(boton), "ruta_carta");

        char color_carta[10], tipo_carta[10];
        sscanf(ruta_carta, "images/%[^_]_%[^.].png", tipo_carta, color_carta);

        // Valida si coincide con el color, tipo, o es un comodín
        if (strcmp(carta_actual.color, color_carta) == 0 || 
            strcmp(carta_actual.tipo, tipo_carta) == 0 || 
            strcmp(tipo_carta, "comodin") == 0 || 
            strcmp(tipo_carta, "+4") == 0) {
            g_list_free(children);
            return TRUE; // Hay movimiento válido
        }
    }
    g_list_free(children);
    return FALSE; // No hay movimientos válidos
}

// Verificar si el jugador actual tiene cartas acumulativas (+2 o +4) que puedan encadenarse con la carta actual.
gboolean hay_carta_encadenable(GameData *game_data) {
    if (strcmp(carta_actual.tipo, "+2") != 0 && strcmp(carta_actual.tipo, "+4") != 0) {
        return FALSE; // Si no es acumulativa, no hay posibilidad de encadenar
    }

    GList *children = gtk_container_get_children(GTK_CONTAINER(game_data->flowboxes[game_data->jugador_actual]));
    gboolean encadenable = FALSE;
    for (GList *child = children; child != NULL; child = child->next) {
        GtkWidget *boton = gtk_bin_get_child(GTK_BIN(GTK_WIDGET(child->data)));
        const char *ruta_carta = g_object_get_data(G_OBJECT(boton), "ruta_carta");

        if (!ruta_carta) continue; // Evitar punteros nulos
        char tipo_carta[10]; // Extraer tipo de la carta ignorando sufijos
        sscanf(ruta_carta, "images/%[^_]_%*s", tipo_carta);

        if (strcmp(tipo_carta, carta_actual.tipo) == 0) { // Verificar si el tipo coincide con la carta actual (solo acumulativas)
            encadenable = TRUE;
            break; // Salimos si encontramos una carta válida
        }
    }
    g_list_free(children);
    return encadenable;
}

// Fynción que recicla las cartas del descarte al mazo, excluyendo la última carta jugada
void reciclar_descarte(GameData *game_data) {
    if (!game_data->descarte || g_list_length(game_data->descarte) <= 1) {
        return;
    }
    
    GList *ultima_carta_nodo = g_list_last(game_data->descarte); // Guarda la última carta jugada y exclúyela del reciclaje
    const char *ultima_carta = (const char *)ultima_carta_nodo->data;
    GList *nuevas_cartas = g_list_copy_deep(game_data->descarte, (GCopyFunc)g_strdup, NULL);

    nuevas_cartas = g_list_remove(nuevas_cartas, ultima_carta); // Remueve la última carta de la lista reciclada

    for (GList *nodo = nuevas_cartas; nodo != NULL; nodo = nodo->next) { // Añadir las cartas recicladas al mazo
        game_data->mazo = g_list_append(game_data->mazo, nodo->data);
    }
    g_list_free(nuevas_cartas);

    for (int i = 0; i < g_list_length(game_data->mazo) * 2; i++) { // Mezclar el mazo
        int index = rand() % g_list_length(game_data->mazo);
        gpointer carta = g_list_nth_data(game_data->mazo, index);
        game_data->mazo = g_list_remove(game_data->mazo, carta);
        game_data->mazo = g_list_append(game_data->mazo, carta);
    }
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
    game_data.main_window = main_window;
    game_data.player_window = player_window;
    game_data.game_window = game_window;


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
