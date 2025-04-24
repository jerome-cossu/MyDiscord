#include <gtk/gtk.h>
#include <libpq-fe.h>
#include <string.h>
#include <regex.h>
#include "home_page.h"

static gboolean is_dark_mode = FALSE;

// Fonction pour basculer entre le mode sombre et clair
static void toggle_theme(GtkButton *button, gpointer user_data) {
    GtkSettings *settings = gtk_settings_get_default();

    is_dark_mode = !is_dark_mode;
    g_object_set(settings, "gtk-application-prefer-dark-theme", is_dark_mode, NULL);

    const char *label = is_dark_mode ? "Switch to light mode" : "Switch to dark mode";
    gtk_button_set_label(button, label);
}

// Fonction pour gérer l'affichage du mot de passe pour la connexion
static void on_login_show_password_toggled(GtkCheckButton *check_button, gpointer user_data) {
    GtkEntry *entry = GTK_ENTRY(user_data);
    gboolean active = gtk_check_button_get_active(check_button);
    gtk_entry_set_visibility(entry, active);
}

// Fonction pour gérer l'affichage du mot de passe pour l'inscription
static void on_signup_show_password_toggled(GtkCheckButton *check_button, gpointer user_data) {
    GtkEntry *entry = GTK_ENTRY(user_data);
    gboolean active = gtk_check_button_get_active(check_button);
    gtk_entry_set_visibility(entry, active);
}

// Fonction de validation du mot de passe
static gboolean validate_password(const gchar *password) {
    regex_t regex;
    int reti;
    const char *pattern = "^(?=.*[A-Z])(?=.*[a-z])(?=.*\\d)(?=.*[!@#$%^&*()_+\\-\\=\\[\\]{};:'\",<.>/?])[^\\s]{10,}$";

    reti = regcomp(&regex, pattern, REG_EXTENDED);
    if (reti) {
        g_print("Could not compile regex\n");
        return FALSE;
    }

    reti = regexec(&regex, password, 0, NULL, 0);
    regfree(&regex);

    return (reti == 0);
}

// Fonction de clic sur le bouton de connexion
static void on_login_button_clicked(GtkButton *button, gpointer user_data) {
    GtkEntry **entries = (GtkEntry **)user_data;
    const gchar *user = gtk_editable_get_text(GTK_EDITABLE(entries[0]));
    const gchar *password = gtk_editable_get_text(GTK_EDITABLE(entries[1]));

    // Connexion à la base de données
    PGconn *conn = PQconnectdb("user=postgres dbname=mydiscord password=yourpassword host=localhost");

    if (PQstatus(conn) != CONNECTION_OK) {
        g_print("Connection to DB failed: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        return;
    }

    // Requête pour vérifier l'existence de l'utilisateur
    const char *query = "SELECT password FROM users WHERE email = $1 OR nickname = $2";

    const char *paramValues[2] = { user, user };

    PGresult *res = PQexecParams(conn, query, 2, NULL, paramValues, NULL, NULL, 0);

    // Vérification de l'existence de l'utilisateur et du mot de passe
    if (PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) == 0) {
        g_print("User not found or incorrect password.\n");
        PQclear(res);
        PQfinish(conn);
        return;
    }

    const char *stored_password = PQgetvalue(res, 0, 0);

    // Comparaison du mot de passe
    if (strcmp(stored_password, password) != 0) {
        g_print("Incorrect password.\n");
    } else {
        g_print("Login successful!\n");
    }

    PQclear(res);
    PQfinish(conn);
}

// Fonction de clic sur le bouton d'inscription
static void on_signup_button_clicked(GtkButton *button, gpointer user_data) {
    GtkEntry **entries = (GtkEntry **)user_data;
    const gchar *first_name = gtk_editable_get_text(GTK_EDITABLE(entries[0]));
    const gchar *last_name = gtk_editable_get_text(GTK_EDITABLE(entries[1]));
    const gchar *nickname = gtk_editable_get_text(GTK_EDITABLE(entries[2]));
    const gchar *email = gtk_editable_get_text(GTK_EDITABLE(entries[3]));
    const gchar *password = gtk_editable_get_text(GTK_EDITABLE(entries[4]));

    // Validation du mot de passe
    if (!validate_password(password)) {
        gtk_label_set_text(GTK_LABEL(user_data), "Password must be at least 10 characters long, contain 1 uppercase letter, 1 lowercase letter, 1 number, and 1 special character.");
        return;
    }

    // Connexion à la base de données
    PGconn *conn = PQconnectdb("user=postgres dbname=mydiscord password=yourpassword host=localhost");

    if (PQstatus(conn) != CONNECTION_OK) {
        g_print("Connection to DB failed: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        return;
    }

    // Préparation de la requête d'insertion
    const char *query = "INSERT INTO users (first_name, last_name, nickname, email, password, registration_date) "
                        "VALUES ($1, $2, $3, $4, $5, NOW())";

    const char *paramValues[5] = { first_name, last_name, nickname, email, password };

    PGresult *res = PQexecParams(conn, query, 5, NULL, paramValues, NULL, NULL, 0);

    // Vérification du succès de l'insertion
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        g_print("Insertion failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        return;
    } else {
        g_print("Account created successfully!\n");
    }

    // Commit de la transaction si nécessaire
    PGresult *commit_res = PQexec(conn, "COMMIT");
    if (PQresultStatus(commit_res) != PGRES_COMMAND_OK) {
        g_print("Commit failed: %s\n", PQerrorMessage(conn));
    } else {
        g_print("Transaction committed successfully.\n");
    }

    PQclear(res);
    PQclear(commit_res);
    PQfinish(conn);
}

// Fonction principale d'activation
void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *outer_box;
    GtkWidget *main_box;
    GtkWidget *left_box, *right_box;
    GtkWidget *signup_box, *login_box;
    GtkWidget *title_label;
    GtkWidget *theme_button;
    GtkWidget *separator;
    GtkWidget *bottom_box;

    GtkWidget *name_entry, *surname_entry, *nickname_entry, *email_entry, *signup_pass_entry, *signup_button;
    GtkWidget *signup_show_pass_check;

    GtkWidget *login_user_entry, *login_pass_entry, *login_button;
    GtkWidget *login_show_pass_check;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Sign up or Log in - MyDiscord");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    outer_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_window_set_child(GTK_WINDOW(window), outer_box);

    main_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 20);
    gtk_widget_set_hexpand(main_box, TRUE);
    gtk_widget_set_vexpand(main_box, TRUE);
    gtk_widget_set_margin_top(main_box, 40);
    gtk_widget_set_margin_bottom(main_box, 40);
    gtk_widget_set_margin_start(main_box, 80);
    gtk_widget_set_margin_end(main_box, 80);
    gtk_box_append(GTK_BOX(outer_box), main_box);

    left_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_widget_set_hexpand(left_box, TRUE);
    gtk_widget_set_valign(left_box, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(main_box), left_box);

    GtkWidget *signup_title = gtk_label_new("Create Account");
    gtk_widget_set_halign(signup_title, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(left_box), signup_title);

    signup_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_widget_set_halign(signup_box, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(left_box), signup_box);

    name_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(name_entry), "First Name");
    gtk_widget_set_size_request(name_entry, 200, -1);
    gtk_box_append(GTK_BOX(signup_box), name_entry);

    surname_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(surname_entry), "Last Name");
    gtk_widget_set_size_request(surname_entry, 200, -1);
    gtk_box_append(GTK_BOX(signup_box), surname_entry);

    nickname_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(nickname_entry), "Nickname");
    gtk_widget_set_size_request(nickname_entry, 200, -1);
    gtk_box_append(GTK_BOX(signup_box), nickname_entry);

    email_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(email_entry), "Email");
    gtk_widget_set_size_request(email_entry, 200, -1);
    gtk_box_append(GTK_BOX(signup_box), email_entry);

    signup_pass_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(signup_pass_entry),
        "Password (min. 10 chars, 1 upper, 1 lower, 1 digit, 1 special)");
    gtk_entry_set_visibility(GTK_ENTRY(signup_pass_entry), FALSE);
    gtk_widget_set_size_request(signup_pass_entry, 200, -1);
    gtk_box_append(GTK_BOX(signup_box), signup_pass_entry);

    signup_show_pass_check = gtk_check_button_new_with_label("Show Password");
    gtk_box_append(GTK_BOX(signup_box), signup_show_pass_check);
    g_signal_connect(signup_show_pass_check, "toggled",
                     G_CALLBACK(on_signup_show_password_toggled), signup_pass_entry);

    signup_button = gtk_button_new_with_label("Create Account");
    gtk_widget_set_size_request(signup_button, 200, -1);
    gtk_box_append(GTK_BOX(signup_box), signup_button);

    // Assignation de la fonction callback pour le bouton de création
    GtkEntry *signup_entries[5] = { GTK_ENTRY(name_entry), GTK_ENTRY(surname_entry), GTK_ENTRY(nickname_entry), GTK_ENTRY(email_entry), GTK_ENTRY(signup_pass_entry) };
    g_signal_connect(signup_button, "clicked", G_CALLBACK(on_signup_button_clicked), signup_entries);

    separator = gtk_separator_new(GTK_ORIENTATION_VERTICAL);
    gtk_widget_set_vexpand(separator, TRUE);
    gtk_box_append(GTK_BOX(main_box), separator);

    right_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_widget_set_hexpand(right_box, TRUE);
    gtk_widget_set_valign(right_box, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(main_box), right_box);

    title_label = gtk_label_new("Login to MyDiscord");
    gtk_widget_set_halign(title_label, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(right_box), title_label);

    login_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_widget_set_halign(login_box, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(right_box), login_box);

    login_user_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(login_user_entry), "Email or Nickname");
    gtk_widget_set_size_request(login_user_entry, 200, -1);
    gtk_box_append(GTK_BOX(login_box), login_user_entry);

    login_pass_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(login_pass_entry), "Password");
    gtk_entry_set_visibility(GTK_ENTRY(login_pass_entry), FALSE);
    gtk_widget_set_size_request(login_pass_entry, 200, -1);
    gtk_box_append(GTK_BOX(login_box), login_pass_entry);

    login_show_pass_check = gtk_check_button_new_with_label("Show Password");
    gtk_box_append(GTK_BOX(login_box), login_show_pass_check);
    g_signal_connect(login_show_pass_check, "toggled",
                     G_CALLBACK(on_login_show_password_toggled), login_pass_entry);

    login_button = gtk_button_new_with_label("Login");
    gtk_widget_set_size_request(login_button, 200, -1);
    gtk_box_append(GTK_BOX(login_box), login_button);

    // Assignation de la fonction callback pour le bouton de connexion
    GtkEntry *login_entries[2] = { GTK_ENTRY(login_user_entry), GTK_ENTRY(login_pass_entry) };
    g_signal_connect(login_button, "clicked", G_CALLBACK(on_login_button_clicked), login_entries);

    theme_button = gtk_button_new_with_label("Switch to dark mode");
    gtk_widget_set_size_request(theme_button, 200, -1);
    gtk_box_append(GTK_BOX(outer_box), theme_button);
    g_signal_connect(theme_button, "clicked", G_CALLBACK(toggle_theme), NULL);

    gtk_window_present(GTK_WINDOW(window));
}