#include <gtk/gtk.h>
#include "home_page.h"  // Inclure le fichier d'en-tête de home_page.c

static gboolean is_dark_mode = TRUE;
static void toggle_theme(GtkButton *button, gpointer user_data) {
    GtkSettings *settings = gtk_settings_get_default();
    is_dark_mode = !is_dark_mode;
    g_object_set(settings, "gtk-application-prefer-dark-theme", is_dark_mode, NULL);
    const char *label = is_dark_mode ? "Switch to light mode" : "Switch to dark mode";
    gtk_button_set_label(button, label);
}

static void on_logout_clicked(GtkButton *button, gpointer user_data) {
    GtkApplication *app = GTK_APPLICATION(user_data);
    GtkWidget *current_window = gtk_widget_get_ancestor(GTK_WIDGET(button), GTK_TYPE_WINDOW);

    // Fermer la fenêtre de chat
    if (current_window) gtk_window_destroy(GTK_WINDOW(current_window));

    // Appeler la fonction home_page_activate pour revenir à la page d'accueil
    home_page_activate(app, NULL);
}

void show_chat_page(GtkWidget *widget, gpointer user_data) {
    GtkApplication *app = GTK_APPLICATION(user_data);
    GtkWidget *current_window = gtk_widget_get_ancestor(widget, GTK_TYPE_WINDOW);
    if (current_window) gtk_window_destroy(GTK_WINDOW(current_window));

    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "MyDiscord - Chat");
    gtk_window_set_default_size(GTK_WINDOW(window), 1000, 600);

    // Layout principal horizontal
    GtkWidget *main_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_window_set_child(GTK_WINDOW(window), main_hbox);

    // ----- Sidebar gauche (serveurs) -----
    GtkWidget *sidebar = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_widget_set_size_request(sidebar, 60, -1);
    gtk_widget_set_margin_top(sidebar, 10);
    gtk_widget_set_margin_bottom(sidebar, 10);
    gtk_widget_set_margin_start(sidebar, 5);
    gtk_widget_set_margin_end(sidebar, 5);
    gtk_widget_set_name(sidebar, "sidebar");
    gtk_box_append(GTK_BOX(main_hbox), sidebar);

    for (int i = 0; i < 5; i++) {
        GtkWidget *server_button = gtk_button_new_with_label("S");
        gtk_widget_set_size_request(server_button, 40, 40);
        gtk_box_append(GTK_BOX(sidebar), server_button);
    }

    // ----- Colonne centrale (liste des salons) -----
    GtkWidget *channels_column = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_widget_set_size_request(channels_column, 200, -1);
    gtk_box_append(GTK_BOX(main_hbox), channels_column);

    GtkWidget *channels_label = gtk_label_new("Text Channels");
    gtk_widget_set_margin_top(channels_label, 10);
    gtk_box_append(GTK_BOX(channels_column), channels_label);

    for (int i = 0; i < 5; i++) {
        char channel_name[32];
        snprintf(channel_name, sizeof(channel_name), "# Salon %d", i + 1);
        GtkWidget *channel_button = gtk_button_new_with_label(channel_name);
        gtk_box_append(GTK_BOX(channels_column), channel_button);
    }

    // ----- Zone principale (messages + input) -----
    GtkWidget *main_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_widget_set_vexpand(main_vbox, TRUE);
    gtk_widget_set_hexpand(main_vbox, TRUE);
    gtk_box_append(GTK_BOX(main_hbox), main_vbox);

    // Zone scrollable des messages
    GtkWidget *scroll = gtk_scrolled_window_new();
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_box_append(GTK_BOX(main_vbox), scroll);

    GtkWidget *chat_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_top(chat_box, 10);
    gtk_widget_set_margin_bottom(chat_box, 10);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), chat_box);

    // Barre d'envoi de message
    GtkWidget *input_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_set_margin_top(input_box, 5);
    gtk_widget_set_margin_bottom(input_box, 5);
    gtk_widget_set_margin_start(input_box, 10);
    gtk_widget_set_margin_end(input_box, 10);
    gtk_box_append(GTK_BOX(main_vbox), input_box);

    GtkWidget *message_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(message_entry), "Type your message...");
    gtk_widget_set_hexpand(message_entry, TRUE);
    gtk_box_append(GTK_BOX(input_box), message_entry);

    GtkWidget *send_button = gtk_button_new_with_label("Send");
    gtk_box_append(GTK_BOX(input_box), send_button);

    // ----- Conteneur vertical pour les boutons de thème et de déconnexion -----
    GtkWidget *footer_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_widget_set_halign(footer_box, GTK_ALIGN_END);  // Aligner à droite
    gtk_box_append(GTK_BOX(main_vbox), footer_box);

    // ----- Bouton de thème -----
    GtkWidget *theme_button = gtk_button_new_with_label("Switch to light mode");
    gtk_widget_set_size_request(theme_button, 80, 30);
    gtk_widget_set_margin_top(theme_button, 10);
    gtk_widget_set_margin_end(theme_button, 10);  // Taille uniforme avec le bouton de déconnexion
    gtk_box_append(GTK_BOX(footer_box), theme_button);
    g_signal_connect(theme_button, "clicked", G_CALLBACK(toggle_theme), NULL);

    // ----- Bouton de déconnexion -----
    GtkWidget *logout_button = gtk_button_new_with_label("Log Out");
    gtk_widget_set_size_request(logout_button, 80, 30);  // Taille uniforme avec le bouton de changement de mode
    gtk_widget_set_margin_top(logout_button, 10);
    gtk_widget_set_margin_bottom(logout_button, 10);
    gtk_widget_set_margin_end(logout_button, 10);
    gtk_box_append(GTK_BOX(footer_box), logout_button);

    // Connecte le bouton "Se déconnecter" à la fonction qui ramène à la page d'accueil
    g_signal_connect(logout_button, "clicked", G_CALLBACK(on_logout_clicked), app);

    gtk_window_present(GTK_WINDOW(window));
}
