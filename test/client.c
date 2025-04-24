#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>  // Remplacer sys/socket.h par winsock2.h
#include <ws2tcpip.h>

#define SERVER_ADDR "127.0.0.1"
#define PORT 8080
#define BUFFER_SIZE 1024

// Fonction pour envoyer le message au serveur
static void on_send_button_clicked(GtkButton *button, gpointer user_data) {
    GtkEditable *entry = GTK_EDITABLE(user_data);  // Caster correctement en GtkEditable*
    const char *message = gtk_editable_get_text(entry); // Fonction GTK pour récupérer le texte

    if (strlen(message) == 0) {
        g_print("Aucun message à envoyer.\n");
        return;
    }

    // Initialisation de Winsock
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        perror("WSAStartup failed");
        return;
    }

    // Créer le socket
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        perror("Socket creation failed");
        WSACleanup();
        return;
    }

    // Préparer l'adresse du serveur
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);

    // Connexion au serveur
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        perror("Connection failed");
        closesocket(sock);
        WSACleanup();
        return;
    }

    // Envoyer le message au serveur
    send(sock, message, strlen(message), 0);

    // Afficher un message localement
    g_print("Message envoyé : %s\n", message);

    // Fermer le socket et nettoyer Winsock
    closesocket(sock);
    WSACleanup();

    // Vider le champ de texte
    gtk_editable_set_text(entry, "");
}

// Fonction pour activer l'application GTK
static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *box;
    GtkWidget *label;
    GtkWidget *entry;
    GtkWidget *send_button;

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

    // Afficher la fenêtre
    gtk_window_present(GTK_WINDOW(window));
}

// Fonction principale pour démarrer l'application GTK
int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    // Créer l'application GTK
    // app = gtk_application_new("com.mycompany.MyDiscord", G_APPLICATION_FLAGS_NONE);
    app = gtk_application_new("com.mycompany.MyDiscord", G_APPLICATION_DEFAULT_FLAGS);

    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    // Exécuter l'application GTK
    status = g_application_run(G_APPLICATION(app), argc, argv);

    g_object_unref(app);
    return status;
}
