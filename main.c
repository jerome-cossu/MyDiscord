#include <gtk/gtk.h>
#include "home_page.h"
#include "chat_page.h" // Ce fichier d'en-tête suffit pour utiliser chat_page

int main(int argc, char *argv[]) {
    GtkApplication *app;
    int status;

    // Crée l'application GTK
    app = gtk_application_new("org.gtk.MyDiscord", G_APPLICATION_DEFAULT_FLAGS);

    // Connecte le signal 'activate' à la fonction home_page_activate
    g_signal_connect(app, "activate", G_CALLBACK(home_page_activate), NULL);

    // Lancer l'application
    status = g_application_run(G_APPLICATION(app), argc, argv);

    // Libère la mémoire allouée pour l'application
    g_object_unref(app);

    return status;
}
