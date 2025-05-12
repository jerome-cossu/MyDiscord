#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include <winsock2.h> 
#include <ws2tcpip.h>
#include "home_page.h"
#include "chat_page.h"

static gboolean is_dark_mode = FALSE;

static void toggle_theme(GtkButton *button, gpointer user_data) {
    GtkSettings *settings = gtk_settings_get_default();
    is_dark_mode = !is_dark_mode;
    g_object_set(settings, "gtk-application-prefer-dark-theme", is_dark_mode, NULL);
    const char *label = is_dark_mode ? "Switch to light mode" : "Switch to dark mode";
    gtk_button_set_label(button, label);
}

static void on_signup_show_password_toggled(GtkCheckButton *check_button, gpointer user_data) {
    GtkEntry *entry = GTK_ENTRY(user_data);
    gboolean active = gtk_check_button_get_active(check_button);
    gtk_entry_set_visibility(entry, active);
}

static void on_login_show_password_toggled(GtkCheckButton *check_button, gpointer user_data) {
    GtkEntry *entry = GTK_ENTRY(user_data);
    gboolean active = gtk_check_button_get_active(check_button);
    gtk_entry_set_visibility(entry, active);
}

void send_signup_request(const char *email, const char *password) {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[1024];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        return;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        closesocket(sockfd);
        return;
    }

    snprintf(buffer, sizeof(buffer), "signup %s %s", email, password);
    send(sockfd, buffer, strlen(buffer), 0);

    recv(sockfd, buffer, sizeof(buffer), 0);
    printf("Server response: %s\n", buffer);

    closesocket(sockfd);
}

void send_login_request(const char *email, const char *password) {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[1024];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        return;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        closesocket(sockfd);
        return;
    }

    snprintf(buffer, sizeof(buffer), "login %s %s", email, password);
    send(sockfd, buffer, strlen(buffer), 0);

    recv(sockfd, buffer, sizeof(buffer), 0);
    printf("Server response: %s\n", buffer);

    closesocket(sockfd);
}

void on_signup_button_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *parent_window = gtk_widget_get_ancestor(GTK_WIDGET(button), GTK_TYPE_WINDOW);
    GtkWidget *signup_box = gtk_widget_get_parent(GTK_WIDGET(button));

    GtkWidget *name_entry = gtk_widget_get_first_child(signup_box);
    GtkWidget *surname_entry = gtk_widget_get_next_sibling(name_entry);
    GtkWidget *nickname_entry = gtk_widget_get_next_sibling(surname_entry);
    GtkWidget *email_entry = gtk_widget_get_next_sibling(nickname_entry);
    GtkWidget *password_entry = gtk_widget_get_next_sibling(email_entry);

    const char *email = gtk_entry_get_text(GTK_ENTRY(email_entry));
    const char *password = gtk_entry_get_text(GTK_ENTRY(password_entry));

    send_signup_request(email, password);

    GtkApplication *app = gtk_window_get_application(GTK_WINDOW(parent_window));
    gtk_window_destroy(GTK_WINDOW(parent_window));
    chat_page_activate(app, NULL);
}

void on_login_button_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *parent_window = gtk_widget_get_ancestor(GTK_WIDGET(button), GTK_TYPE_WINDOW);
    GtkWidget *login_box = gtk_widget_get_parent(GTK_WIDGET(button));

    GtkWidget *email_entry = gtk_widget_get_first_child(login_box);
    GtkWidget *password_entry = gtk_widget_get_next_sibling(email_entry);

    const char *email = gtk_entry_get_text(GTK_ENTRY(email_entry));
    const char *password = gtk_entry_get_text(GTK_ENTRY(password_entry));

    send_login_request(email, password);

    GtkApplication *app = gtk_window_get_application(GTK_WINDOW(parent_window));
    gtk_window_destroy(GTK_WINDOW(parent_window));
    chat_page_activate(app, NULL);
}

void home_page_activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window, *outer_box, *main_box;
    GtkWidget *left_box, *right_box, *signup_box, *login_box;
    GtkWidget *title_label, *theme_button, *separator, *bottom_box;
    GtkWidget *name_entry, *surname_entry, *nickname_entry, *email_entry, *signup_pass_entry, *signup_button;
    GtkWidget *signup_show_pass_check;
    GtkWidget *login_user_entry, *login_pass_entry, *login_button;
    GtkWidget *login_show_pass_check;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "MyDiscord");
    gtk_window_set_default_size(GTK_WINDOW(window), 1000, 600);
    gtk_widget_show(window);

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

    // --- Left Side (Signup) ---
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
    g_signal_connect(signup_button, "clicked", G_CALLBACK(on_signup_button_clicked), NULL);

    // --- Separator ---
    separator = gtk_separator_new(GTK_ORIENTATION_VERTICAL);
    gtk_widget_set_vexpand(separator, TRUE);
    gtk_box_append(GTK_BOX(main_box), separator);

    // --- Right Side (Login) ---
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

    login_button = gtk_button_new_with_label("Log In");
    gtk_widget_set_size_request(login_button, 200, -1);
    gtk_box_append(GTK_BOX(login_box), login_button);
    g_signal_connect(login_button, "clicked", G_CALLBACK(on_login_button_clicked), NULL);

    // --- Bottom (Theme Toggle) ---
    bottom_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_halign(bottom_box, GTK_ALIGN_END);
    gtk_widget_set_margin_end(bottom_box, 20);
    gtk_widget_set_margin_bottom(bottom_box, 10);
    gtk_box_append(GTK_BOX(outer_box), bottom_box);

    theme_button = gtk_button_new_with_label("Switch to dark mode");
    gtk_widget_set_size_request(theme_button, 200, -1);
    gtk_box_append(GTK_BOX(bottom_box), theme_button);
    g_signal_connect(theme_button, "clicked", G_CALLBACK(toggle_theme), NULL);

    gtk_window_present(GTK_WINDOW(window));
}
