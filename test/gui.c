#include "gui.h"

static void on_send_button_clicked(GtkButton *button, gpointer user_data) {
    GtkEditable *entry = GTK_EDITABLE(user_data);  // Caster correctement en GtkEditable*
    const char *message = gtk_editable_get_text(entry); // Fonction GTK 4 pour récupérer le texte
    g_print("Message envoyé : %s\n", message);
    gtk_editable_set_text(entry, ""); // Vider le champ après envoi (optionnel)
}


// Callback pour fermer la fenêtre
static void on_close_button_clicked(GtkButton *button, gpointer user_data) {
    GtkWindow *window = GTK_WINDOW(user_data);
    gtk_window_close(window);
}

void activate_app(GtkApplication *app, gpointer user_data) {
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "MyDiscord");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_window_set_child(GTK_WINDOW(window), box);

    GtkWidget *label = gtk_label_new("Bienvenue sur MyDiscord !");
    gtk_box_append(GTK_BOX(box), label);

    GtkWidget *entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "Tapez votre message...");
    gtk_box_append(GTK_BOX(box), entry);

    GtkWidget *send_button = gtk_button_new_with_label("Envoyer");
    g_signal_connect(send_button, "clicked", G_CALLBACK(on_send_button_clicked), entry);
    gtk_box_append(GTK_BOX(box), send_button);

    GtkWidget *close_button = gtk_button_new_with_label("Fermer");
    g_signal_connect(close_button, "clicked", G_CALLBACK(on_close_button_clicked), window);
    gtk_box_append(GTK_BOX(box), close_button);

    gtk_window_present(GTK_WINDOW(window));
}
