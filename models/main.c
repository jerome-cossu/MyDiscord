#include <gtk/gtk.h>

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *main_box;
    GtkWidget *title_label;
    GtkWidget *username_entry;
    GtkWidget *password_entry;
    GtkWidget *login_button;
    GtkWidget *entry_box;

    // Display the main window
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Connexion - MyDiscord");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

    main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_widget_set_margin_top(main_box, 30);
    gtk_widget_set_margin_bottom(main_box, 30);
    gtk_widget_set_margin_start(main_box, 30);
    gtk_widget_set_margin_end(main_box, 30);
    gtk_window_set_child(GTK_WINDOW(window), main_box);

    // Title label
    title_label = gtk_label_new("Login to mydiscord");
    gtk_widget_set_halign(title_label, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(main_box), title_label);

    // Entry box
    entry_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_halign(entry_box, GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(entry_box, 200, -1);
    gtk_box_append(GTK_BOX(main_box), entry_box);

    // Username and password entries
    username_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(username_entry), "Email or Nickname");
    gtk_box_append(GTK_BOX(entry_box), username_entry);

    password_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(password_entry), "Password");
    gtk_entry_set_visibility(GTK_ENTRY(password_entry), FALSE); 
    gtk_box_append(GTK_BOX(entry_box), password_entry);

    // Login button
    login_button = gtk_button_new_with_label("Connexion");
    gtk_box_append(GTK_BOX(entry_box), login_button);

    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("com.mydiscord.login", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
