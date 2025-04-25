#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "client.h"

#define SERVER_ADDR "127.0.0.1"
#define PORT 8080
#define BUFFER_SIZE 1024

// Fonction pour envoyer le message au serveur
void send_message_to_server(const char *message) {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        perror("WSAStartup failed");
        return;
    }

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        perror("Socket creation failed");
        WSACleanup();
        return;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        perror("Connection failed");
        closesocket(sock);
        WSACleanup();
        return;
    }

    send(sock, message, strlen(message), 0);
    closesocket(sock);
    WSACleanup();
}

// Fonction pour ajouter un message dans le GtkTextView
void append_message_to_display(GtkTextView *text_view, const char *message) {
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(text_view);
    GtkTextIter end_iter;
    gtk_text_buffer_get_end_iter(buffer, &end_iter);  // Obtenir la fin du texte actuel
    gtk_text_buffer_insert(buffer, &end_iter, message, -1);  // Ajouter le message
    gtk_text_buffer_insert(buffer, &end_iter, "\n", -1);  // Ajouter un saut de ligne
    gtk_text_view_scroll_to_iter(text_view, &end_iter, 0.0, TRUE, 0.0, 0.0);  // Faire défiler jusqu'à la fin
}

// Fonction pour activer l'application GTK
void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *box;
    GtkWidget *label;
    GtkWidget *entry;
    GtkWidget *send_button;
    GtkWidget *text_view;
    GtkWidget *receive_button;

    // Créer la fenêtre principale
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "MyDiscord");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);

    // Créer un conteneur vertical pour la mise en page
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_window_set_child(GTK_WINDOW(window), box);

    // Ajouter un label avec un message de bienvenue
    label = gtk_label_new("Bienvenue sur MyDiscord !");
    gtk_box_append(GTK_BOX(box), label);

    // Ajouter un champ de texte (entry) pour saisir le message
    entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "Tapez votre message...");
    gtk_box_append(GTK_BOX(box), entry);

    // Ajouter un bouton pour envoyer le message
    send_button = gtk_button_new_with_label("Envoyer");
    g_signal_connect(send_button, "clicked", G_CALLBACK(on_send_button_clicked), entry);
    gtk_box_append(GTK_BOX(box), send_button);

    // Ajouter un espace pour afficher les messages
    text_view = gtk_text_view_new();
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD);
    gtk_widget_set_size_request(text_view, 350, 150);  // Taille de l'espace d'affichage
    gtk_box_append(GTK_BOX(box), text_view);

    // Ajouter un bouton pour simuler la réception de messages
    receive_button = gtk_button_new_with_label("Recevoir un message");
    g_signal_connect(receive_button, "clicked", G_CALLBACK(on_receive_button_clicked), text_view);
    gtk_box_append(GTK_BOX(box), receive_button);

    // Afficher la fenêtre
    gtk_window_present(GTK_WINDOW(window));
}

// Fonction appelée lorsqu'on clique sur "Recevoir" pour simuler un message
void on_receive_button_clicked(GtkButton *button, gpointer user_data) {
    GtkTextView *text_view = GTK_TEXT_VIEW(user_data);
    const char *message = "Message reçu : Salut !";  // Message simulé

    append_message_to_display(text_view, message);
}

// Fonction appelée lorsqu'on clique sur "Envoyer"
void on_send_button_clicked(GtkButton *button, gpointer user_data) {
    GtkEditable *entry = GTK_EDITABLE(user_data);
    const char *message = gtk_editable_get_text(entry);

    if (strlen(message) == 0) {
        g_print("Aucun message à envoyer.\n");
        return;
    }

    // Envoyer le message au serveur
    send_message_to_server(message);

    // Afficher localement
    g_print("Message envoyé : %s\n", message);

    // Vider le champ de texte
    gtk_editable_set_text(entry, "");
}
