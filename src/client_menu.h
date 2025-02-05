#ifndef CLIENT_MENU_H
#define CLIENT_MENU_H

#include <gtk/gtk.h>
#include "widgets.h"
#include "outils.h"
#include "client.h"
#include "main_menu.h"

// Déclarations des callbacks
void on_add_client_clicked(GtkWidget *button, gpointer user_data);
void on_delete_client_clicked(GtkWidget *button, gpointer user_data);
void on_search_client_clicked(GtkWidget *button, gpointer user_data);
void on_modify_client_clicked(GtkWidget *button, gpointer user_data);
// Autres fonctions
void show_add_client_form(GtkWidget *form_box);
void show_delete_client_form(GtkWidget *form_box);
void show_modify_client_form(GtkWidget *form_box);
void show_display_clients_form(GtkWidget *form_box);

#endif
