#ifndef GUI_H
#define GUI_H

#include <gtk/gtk.h>

extern GtkWidget *message_box; // Déclarer la boîte où les messages seront affichés

void activate(GtkApplication *app, gpointer user_data);
void display_message(const char *message);  // Déclarer la fonction d'affichage des messages

#endif
