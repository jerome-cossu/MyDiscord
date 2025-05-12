#include <gtk/gtk.h>
#include "home_page.h"
#include "chat_page.h"

int main(int argc, char *argv[]) {
    GtkApplication *app;
    int status;

    // Create the GTK application
    app = gtk_application_new("org.gtk.MyDiscord", G_APPLICATION_DEFAULT_FLAGS);

    // Connect the 'activate' signal to the home_page_activate function
    g_signal_connect(app, "activate", G_CALLBACK(home_page_activate), NULL);

    // Launch the application
    status = g_application_run(G_APPLICATION(app), argc, argv);

    // Free the memory allocated for the application
    g_object_unref(app);

    return status;
}
